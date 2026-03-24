#pragma once

#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif

namespace win_ai_agent::nexus_pro {

#ifdef _WIN32
struct HandleCloser {
  void operator()(void* handle) const noexcept {
    if (handle && handle != INVALID_HANDLE_VALUE) {
      ::CloseHandle(handle);
    }
  }
};
using UniqueHandle = std::unique_ptr<void, HandleCloser>;
#else
struct HandleCloser {
  void operator()(void*) const noexcept {}
};
using UniqueHandle = std::unique_ptr<void, HandleCloser>;
#endif

}  // namespace win_ai_agent::nexus_pro
