#include "appid.h"

#include <windows.h>

#include <propkey.h>
#include <shlwapi.h>
#include <shobjidl.h>

#include <array>
#include <atomic>
#include <memory>
#include <new>
#include <ranges>
#include <span>
#include <string>
#include <string_view>

#include "detours.h"

#include "utils.h"

// Chrome derives its AppUserModelIDs in chrome/browser/shell_integration_win.cc
// (GetAppUserModelIdForBrowser, GetExpectedAppId). For a portable copy they
// are not unique: copies whose data folders share a name get the same ID, and
// a .lnk without `--user-data-dir` is given the installed Chrome's ID. Chrome
// hands its IDs to Windows through the process, each window's property store,
// the jump list (chrome/browser/win/jumplist_updater.cc) and .lnk files
// (base/win/shortcut.cc; since 157, MigrateTaskbarShortcutLocation rewrites
// taskbar pins every 30 days in MigrateShortcutsInPath). Every one of these is
// hooked below and maps the ID with `MapAppId`, so pins, windows and the jump
// list (New window, New Incognito window, ...) all land in one taskbar group.

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

constexpr std::wstring_view kAppIdPrefix = L"ChromePlusNext.";
const std::wstring& GetCustomAppUserModelID() {
  static const std::wstring custom_appid = [] {
    constexpr wchar_t hex[] = L"0123456789ABCDEF";
    auto hash = Fnv1aHash(GetAppDir());
    std::wstring result{kAppIdPrefix};
    result.reserve(kAppIdPrefix.size() + 16);
    for (int i = 60; i >= 0; i -= 4) {
      result += hex[(hash >> i) & 0xF];
    }
    DebugLog(L"AppUserModelID: {}", result);
    return result;
  }();
  return custom_appid;
}

// Web app IDs ("<base>._crx_<app id>[.<profile>]", see
// GenerateApplicationNameFromAppId in
// chrome/browser/web_applications/web_app_helpers.cc) keep Chrome's value, so
// each app keeps its own taskbar group and matches the shortcuts Chrome
// creates for it. Everything else (browser windows of every profile, DevTools,
// picture-in-picture, `--app=<url>` windows) shares this copy's ID. An
// `--app=<url>` ID ("<base>.<host>_<path>", GenerateApplicationNameFromURL)
// has no marker to tell it from a browser ID, and Chrome creates no shortcut
// for it.
bool IsWebAppId(std::wstring_view id) {
  const auto dot = id.find(L'.');
  return dot != std::wstring_view::npos &&
         id.substr(dot + 1).starts_with(L"_crx_");
}

bool IsWebAppId(const PROPVARIANT& pv) {
  return pv.vt == VT_LPWSTR && pv.pwszVal && IsWebAppId(pv.pwszVal);
}

LPCWSTR MapAppId(LPCWSTR id) {
  return id && IsWebAppId(id) ? id : GetCustomAppUserModelID().c_str();
}

struct PropVariantDeleter {
  void operator()(PROPVARIANT* pv) const {
    PropVariantClear(pv);
    delete pv;
  }
};
using ScopedPropVariant = std::unique_ptr<PROPVARIANT, PropVariantDeleter>;

// `InitPropVariantFromString` is not used because it significantly increases
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

// CLSID_ShellLink serves every .lnk the process touches, so only links whose
// target is in this copy's directory (chrome.exe, chrome_proxy.exe) are mapped.
bool TargetsAppDir(IShellLinkW* link) {
  std::array<wchar_t, MAX_PATH> path{};
  if (link->GetPath(path.data(), static_cast<int>(path.size()), nullptr, 0) !=
      S_OK) {
    return false;
  }
  ::PathRemoveFileSpecW(path.data());
  return ::CompareStringOrdinal(path.data(), -1, GetAppDir().c_str(), -1,
                                TRUE) == CSTR_EQUAL;
}

// Forwards to the wrapped object, which it owns one reference to. A
// QueryInterface for anything but `Interface` reaches the wrapped object.
template <typename Interface>
class Wrapper : public Interface {
 public:
  explicit Wrapper(Interface* real) : real_(real) {}
  virtual ~Wrapper() { real_->Release(); }

  Wrapper(const Wrapper&) = delete;
  Wrapper& operator=(const Wrapper&) = delete;

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
    if (!ppv) {
      return E_POINTER;
    }
    if (riid == IID_IUnknown || riid == __uuidof(Interface)) {
      *ppv = static_cast<Interface*>(this);
      AddRef();
      return S_OK;
    }
    return real_->QueryInterface(riid, ppv);
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

 protected:
  Interface* real_;

 private:
  std::atomic<ULONG> ref_count_{1};
};

// Wraps a window's property store, or a .lnk's when `link` is set.
class PropertyStoreWrapper final : public Wrapper<IPropertyStore> {
 public:
  PropertyStoreWrapper(IPropertyStore* real, IShellLinkW* link)
      : Wrapper(real), link_(link) {}

  ~PropertyStoreWrapper() override {
    if (link_) {
      link_->Release();
    }
  }

  HRESULT STDMETHODCALLTYPE GetCount(DWORD* cProps) override {
    return real_->GetCount(cProps);
  }

  HRESULT STDMETHODCALLTYPE GetAt(DWORD iProp, PROPERTYKEY* pkey) override {
    return real_->GetAt(iProp, pkey);
  }

  // A link's mapped value is what lets Chrome's own pin migration repair a pin
  // that an earlier version rewrote to Chrome's ID: the read no longer matches
  // what Chrome expects, so it writes the ID again, through `SetValue`.
  HRESULT STDMETHODCALLTYPE GetValue(REFPROPERTYKEY key,
                                     PROPVARIANT* pv) override {
    if (!IsEqualPropertyKey(key, PKEY_AppUserModel_ID) || !Maps()) {
      return real_->GetValue(key, pv);
    }
    if (SUCCEEDED(real_->GetValue(key, pv))) {
      if (IsWebAppId(*pv)) {
        return S_OK;
      }
      PropVariantClear(pv);
    }
    auto appid_pv = MakeAppIdVariant();
    *pv = *appid_pv;
    appid_pv->vt = VT_EMPTY;
    return S_OK;
  }

  HRESULT STDMETHODCALLTYPE SetValue(REFPROPERTYKEY key,
                                     REFPROPVARIANT propvar) override {
    if (IsEqualPropertyKey(key, PKEY_AppUserModel_ID) && !IsWebAppId(propvar) &&
        Maps()) {
      auto pv = MakeAppIdVariant();
      return real_->SetValue(key, *pv);
    }
    return real_->SetValue(key, propvar);
  }

  HRESULT STDMETHODCALLTYPE Commit() override { return real_->Commit(); }

 private:
  bool Maps() const { return !link_ || TargetsAppDir(link_); }

  IShellLinkW* link_;
};

// Chrome reaches a link's property store through IShellLink::QueryInterface
// (base/win/shortcut.cc), so that is where the store is wrapped.
class ShellLinkWrapper final : public Wrapper<IShellLinkW> {
 public:
  using Wrapper::Wrapper;

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
    if (!ppv || riid != IID_IPropertyStore) {
      return Wrapper::QueryInterface(riid, ppv);
    }
    IPropertyStore* store = nullptr;
    HRESULT hr = real_->QueryInterface(IID_PPV_ARGS(&store));
    if (FAILED(hr)) {
      *ppv = nullptr;
      return hr;
    }
    real_->AddRef();
    auto* wrapper = new (std::nothrow) PropertyStoreWrapper(store, real_);
    if (!wrapper) {
      real_->Release();
      *ppv = store;
      return hr;
    }
    *ppv = static_cast<IPropertyStore*>(wrapper);
    return hr;
  }

  HRESULT STDMETHODCALLTYPE GetPath(LPWSTR pszFile,
                                    int cch,
                                    WIN32_FIND_DATAW* pfd,
                                    DWORD fFlags) override {
    return real_->GetPath(pszFile, cch, pfd, fFlags);
  }
  HRESULT STDMETHODCALLTYPE GetIDList(PIDLIST_ABSOLUTE* ppidl) override {
    return real_->GetIDList(ppidl);
  }
  HRESULT STDMETHODCALLTYPE SetIDList(PCIDLIST_ABSOLUTE pidl) override {
    return real_->SetIDList(pidl);
  }
  HRESULT STDMETHODCALLTYPE GetDescription(LPWSTR pszName, int cch) override {
    return real_->GetDescription(pszName, cch);
  }
  HRESULT STDMETHODCALLTYPE SetDescription(LPCWSTR pszName) override {
    return real_->SetDescription(pszName);
  }
  HRESULT STDMETHODCALLTYPE GetWorkingDirectory(LPWSTR pszDir,
                                                int cch) override {
    return real_->GetWorkingDirectory(pszDir, cch);
  }
  HRESULT STDMETHODCALLTYPE SetWorkingDirectory(LPCWSTR pszDir) override {
    return real_->SetWorkingDirectory(pszDir);
  }
  HRESULT STDMETHODCALLTYPE GetArguments(LPWSTR pszArgs, int cch) override {
    return real_->GetArguments(pszArgs, cch);
  }
  HRESULT STDMETHODCALLTYPE SetArguments(LPCWSTR pszArgs) override {
    return real_->SetArguments(pszArgs);
  }
  HRESULT STDMETHODCALLTYPE GetHotkey(WORD* pwHotkey) override {
    return real_->GetHotkey(pwHotkey);
  }
  HRESULT STDMETHODCALLTYPE SetHotkey(WORD wHotkey) override {
    return real_->SetHotkey(wHotkey);
  }
  HRESULT STDMETHODCALLTYPE GetShowCmd(int* piShowCmd) override {
    return real_->GetShowCmd(piShowCmd);
  }
  HRESULT STDMETHODCALLTYPE SetShowCmd(int iShowCmd) override {
    return real_->SetShowCmd(iShowCmd);
  }
  HRESULT STDMETHODCALLTYPE GetIconLocation(LPWSTR pszIconPath,
                                            int cch,
                                            int* piIcon) override {
    return real_->GetIconLocation(pszIconPath, cch, piIcon);
  }
  HRESULT STDMETHODCALLTYPE SetIconLocation(LPCWSTR pszIconPath,
                                            int iIcon) override {
    return real_->SetIconLocation(pszIconPath, iIcon);
  }
  HRESULT STDMETHODCALLTYPE SetRelativePath(LPCWSTR pszPathRel,
                                            DWORD dwReserved) override {
    return real_->SetRelativePath(pszPathRel, dwReserved);
  }
  HRESULT STDMETHODCALLTYPE Resolve(HWND hwnd, DWORD fFlags) override {
    return real_->Resolve(hwnd, fFlags);
  }
  HRESULT STDMETHODCALLTYPE SetPath(LPCWSTR pszFile) override {
    return real_->SetPath(pszFile);
  }
};

// `DeleteList` is not mapped, so deleting one profile's list cannot clear the
// list that all profiles share.
class DestinationListWrapper final : public Wrapper<ICustomDestinationList> {
 public:
  using Wrapper::Wrapper;

  HRESULT STDMETHODCALLTYPE SetAppID(LPCWSTR pszAppID) override {
    return real_->SetAppID(MapAppId(pszAppID));
  }
  HRESULT STDMETHODCALLTYPE BeginList(UINT* pcMinSlots,
                                      REFIID riid,
                                      void** ppv) override {
    return real_->BeginList(pcMinSlots, riid, ppv);
  }
  HRESULT STDMETHODCALLTYPE AppendCategory(LPCWSTR pszCategory,
                                           IObjectArray* poa) override {
    return real_->AppendCategory(pszCategory, poa);
  }
  HRESULT STDMETHODCALLTYPE
  AppendKnownCategory(KNOWNDESTCATEGORY category) override {
    return real_->AppendKnownCategory(category);
  }
  HRESULT STDMETHODCALLTYPE AddUserTasks(IObjectArray* poa) override {
    return real_->AddUserTasks(poa);
  }
  HRESULT STDMETHODCALLTYPE CommitList() override {
    return real_->CommitList();
  }
  HRESULT STDMETHODCALLTYPE GetRemovedDestinations(REFIID riid,
                                                   void** ppv) override {
    return real_->GetRemovedDestinations(riid, ppv);
  }
  HRESULT STDMETHODCALLTYPE DeleteList(LPCWSTR pszAppID) override {
    return real_->DeleteList(pszAppID);
  }
  HRESULT STDMETHODCALLTYPE AbortList() override { return real_->AbortList(); }
};

template <typename T, typename Interface>
void Wrap(void** ppv) {
  if (auto* wrapper = new (std::nothrow) T(static_cast<Interface*>(*ppv))) {
    *ppv = static_cast<Interface*>(wrapper);
  }
}

auto RawCoCreateInstance = CoCreateInstance;

HRESULT WINAPI MyCoCreateInstance(REFCLSID rclsid,
                                  LPUNKNOWN outer,
                                  DWORD context,
                                  REFIID riid,
                                  LPVOID* ppv) {
  HRESULT hr = RawCoCreateInstance(rclsid, outer, context, riid, ppv);
  if (FAILED(hr) || outer || !ppv || !*ppv) {
    return hr;
  }
  if (rclsid == __uuidof(ShellLink) && riid == __uuidof(IShellLinkW)) {
    Wrap<ShellLinkWrapper, IShellLinkW>(ppv);
  } else if (rclsid == __uuidof(DestinationList) &&
             riid == __uuidof(ICustomDestinationList)) {
    Wrap<DestinationListWrapper, ICustomDestinationList>(ppv);
  }
  return hr;
}

auto RawSetCurrentProcessExplicitAppUserModelID =
    SetCurrentProcessExplicitAppUserModelID;

HRESULT WINAPI MySetCurrentProcessExplicitAppUserModelID(PCWSTR appid) {
  return RawSetCurrentProcessExplicitAppUserModelID(MapAppId(appid));
}

auto RawSHGetPropertyStoreForWindow = SHGetPropertyStoreForWindow;

HRESULT WINAPI MySHGetPropertyStoreForWindow(HWND hwnd,
                                             REFIID riid,
                                             void** ppv) {
  HRESULT hr = RawSHGetPropertyStoreForWindow(hwnd, riid, ppv);
  if (SUCCEEDED(hr) && ppv && *ppv && riid == IID_IPropertyStore) {
    if (IsChromeWindow(hwnd)) {
      auto* wrapper = new (std::nothrow) PropertyStoreWrapper(
          static_cast<IPropertyStore*>(*ppv), /*link=*/nullptr);
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
  DetourAttach(&RawSetCurrentProcessExplicitAppUserModelID,
               MySetCurrentProcessExplicitAppUserModelID);
  DetourAttach(&RawSHGetPropertyStoreForWindow, MySHGetPropertyStoreForWindow);
  DetourAttach(&RawCoCreateInstance, MyCoCreateInstance);
  auto status = DetourTransactionCommit();
  if (status != NO_ERROR) {
    DebugLog(L"SetAppId failed {}", status);
  }
}
