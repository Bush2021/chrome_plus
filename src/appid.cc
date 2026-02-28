#include "appid.h"

#include <windows.h>

#include <propkey.h>
#include <shobjidl.h>

#include <atomic>
#include <memory>
#include <ranges>
#include <span>
#include <string>

#include "detours.h"

#include "utils.h"

namespace {

uint64_t Fnv1aHash(std::wstring_view input) {
  uint64_t hash = 14695981039346656037ULL;
  auto bytes = std::as_bytes(std::span{input});
  for (auto b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

constexpr std::wstring_view APPID_PREFIX = L"ChromePlusNext.";
const std::wstring& GetCustomAppUserModelID() {
  static const std::wstring custom_appid = [] {
    constexpr wchar_t hex[] = L"0123456789ABCDEF";
    auto hash = Fnv1aHash(GetAppDir());
    std::wstring result{APPID_PREFIX};
    result.reserve(APPID_PREFIX.size() + 16);
    for (int i = 60; i >= 0; i -= 4) {
      result += hex[(hash >> i) & 0xF];
    }
    DebugLog(L"AppUserModelID: {}", result);
    return result;
  }();
  return custom_appid;
}

struct PropVariantDeleter {
  void operator()(PROPVARIANT* pv) const {
    PropVariantClear(pv);
    delete pv;
  }
};
using ScopedPropVariant = std::unique_ptr<PROPVARIANT, PropVariantDeleter>;

// `InitPropVariantFromString` is not used because it signiticantly increases
// the size of the binary (~25KB).
ScopedPropVariant MakeAppIdVariant() {
  auto pv = ScopedPropVariant(new PROPVARIANT{});
  pv->vt = VT_EMPTY;
  const auto& id = GetCustomAppUserModelID();
  const size_t char_count = id.size() + 1;
  const size_t byte_len = char_count * sizeof(wchar_t);
  pv->pwszVal = static_cast<LPWSTR>(CoTaskMemAlloc(byte_len));
  if (pv->pwszVal) {
    pv->vt = VT_LPWSTR;
    std::span<wchar_t> dest_span{pv->pwszVal, char_count};
    auto result = std::ranges::copy(id, dest_span.begin());
    *result.out = L'\0';
  }
  return pv;
}

class PropertyStoreWrapper final : public IPropertyStore {
 public:
  explicit PropertyStoreWrapper(IPropertyStore* real_store)
      : real_store_(real_store) {}

  ~PropertyStoreWrapper() {
    if (real_store_) {
      real_store_->Release();
    }
  }

  PropertyStoreWrapper(const PropertyStoreWrapper&) = delete;
  PropertyStoreWrapper& operator=(const PropertyStoreWrapper&) = delete;

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
    if (!ppv) {
      return E_POINTER;
    }
    if (riid == IID_IUnknown || riid == IID_IPropertyStore) {
      *ppv = static_cast<IPropertyStore*>(this);
      AddRef();
      return S_OK;
    }
    return real_store_->QueryInterface(riid, ppv);
  }

  ULONG STDMETHODCALLTYPE AddRef() override {
    return ref_count_.fetch_add(1, std::memory_order_relaxed) + 1;
  }

  ULONG STDMETHODCALLTYPE Release() override {
    auto count = ref_count_.fetch_sub(1, std::memory_order_acq_rel) - 1;
    if (count == 0) {
      delete this;
    }
    return count;
  }

  HRESULT STDMETHODCALLTYPE GetCount(DWORD* cProps) override {
    return real_store_->GetCount(cProps);
  }

  HRESULT STDMETHODCALLTYPE GetAt(DWORD iProp, PROPERTYKEY* pkey) override {
    return real_store_->GetAt(iProp, pkey);
  }

  HRESULT STDMETHODCALLTYPE GetValue(REFPROPERTYKEY key,
                                     PROPVARIANT* pv) override {
    if (IsEqualPropertyKey(key, PKEY_AppUserModel_ID)) {
      auto appid_pv = MakeAppIdVariant();
      *pv = *appid_pv;
      appid_pv->vt = VT_EMPTY;
      return S_OK;
    }
    return real_store_->GetValue(key, pv);
  }

  HRESULT STDMETHODCALLTYPE SetValue(REFPROPERTYKEY key,
                                     REFPROPVARIANT propvar) override {
    if (IsEqualPropertyKey(key, PKEY_AppUserModel_ID)) {
      auto pv = MakeAppIdVariant();
      return real_store_->SetValue(key, *pv);
    }
    return real_store_->SetValue(key, propvar);
  }

  HRESULT STDMETHODCALLTYPE Commit() override { return real_store_->Commit(); }

 private:
  IPropertyStore* real_store_;
  std::atomic<ULONG> ref_count_{1};
};

static auto RawSetCurrentProcessExplicitAppUserModelID =
    SetCurrentProcessExplicitAppUserModelID;

HRESULT WINAPI MySetCurrentProcessExplicitAppUserModelID(PCWSTR appid) {
  return RawSetCurrentProcessExplicitAppUserModelID(
      GetCustomAppUserModelID().c_str());
}

static auto RawSHGetPropertyStoreForWindow = SHGetPropertyStoreForWindow;

HRESULT WINAPI MySHGetPropertyStoreForWindow(HWND hwnd,
                                             REFIID riid,
                                             void** ppv) {
  HRESULT hr = RawSHGetPropertyStoreForWindow(hwnd, riid, ppv);
  if (SUCCEEDED(hr) && ppv && *ppv && riid == IID_IPropertyStore) {
    if (IsChromeWindow(hwnd)) {
      auto* wrapper = new (std::nothrow)
          PropertyStoreWrapper(static_cast<IPropertyStore*>(*ppv));
      if (wrapper) {
        *ppv = static_cast<IPropertyStore*>(wrapper);
      }
    }
  }
  return hr;
}

}  // namespace

void SetAppId() {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourAttach(
      reinterpret_cast<LPVOID*>(&RawSetCurrentProcessExplicitAppUserModelID),
      reinterpret_cast<void*>(MySetCurrentProcessExplicitAppUserModelID));
  DetourAttach(reinterpret_cast<LPVOID*>(&RawSHGetPropertyStoreForWindow),
               reinterpret_cast<void*>(MySHGetPropertyStoreForWindow));
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"SetAppId failed {}", status);
  }
}
