#ifndef CHROME_PLUS_SRC_COM_INITIALIZER_H_
#define CHROME_PLUS_SRC_COM_INITIALIZER_H_

#include <objbase.h>

class ComInitializer {
 public:
  ComInitializer() {
    const HRESULT hr = CoInitialize(nullptr);
    initialized_ = SUCCEEDED(hr) || hr == RPC_E_CHANGED_MODE;
    should_uninitialize_ = (hr == S_OK || hr == S_FALSE);
  }
  ~ComInitializer() {
    if (should_uninitialize_) {
      CoUninitialize();
    }
  }
  ComInitializer(const ComInitializer&) = delete;
  ComInitializer& operator=(const ComInitializer&) = delete;
  ComInitializer(ComInitializer&&) = delete;
  ComInitializer& operator=(ComInitializer&&) = delete;

  [[nodiscard]] bool IsInitialized() const { return initialized_; }

 private:
  bool initialized_ = false;
  bool should_uninitialize_ = false;
};

#endif  // CHROME_PLUS_SRC_COM_INITIALIZER_H_
