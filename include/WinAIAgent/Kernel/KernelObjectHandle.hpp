#ifndef WIN_AI_AGENT_KERNEL_KERNEL_OBJECT_HANDLE_HPP
#define WIN_AI_AGENT_KERNEL_KERNEL_OBJECT_HANDLE_HPP

#ifdef _WIN32
#include <Windows.h>
#endif

namespace win_ai_agent::kernel {

#ifdef _WIN32
struct ZwKernelHandleTraits {
  static void Close(HANDLE handle) noexcept { ::CloseHandle(handle); }
  static constexpr HANDLE kInvalid = nullptr;
};

template <typename THandle, typename TTraits>
class KernelObjectHandle final {
 public:
  explicit KernelObjectHandle(THandle handle = TTraits::kInvalid) noexcept : handle_(handle) {}
  ~KernelObjectHandle() { Close(); }

  KernelObjectHandle(const KernelObjectHandle&) = delete;
  KernelObjectHandle& operator=(const KernelObjectHandle&) = delete;

  KernelObjectHandle(KernelObjectHandle&& other) noexcept : handle_(other.Release()) {}

  KernelObjectHandle& operator=(KernelObjectHandle&& other) noexcept {
    if (this != &other) {
      Reset(other.Release());
    }
    return *this;
  }

  [[nodiscard]] bool IsValid() const noexcept { return handle_ != TTraits::kInvalid; }
  [[nodiscard]] THandle Get() const noexcept { return handle_; }

  THandle Release() noexcept {
    THandle current = handle_;
    handle_ = TTraits::kInvalid;
    return current;
  }

  void Reset(THandle handle = TTraits::kInvalid) noexcept {
    Close();
    handle_ = handle;
  }

  void Close() noexcept {
    if (IsValid()) {
      TTraits::Close(handle_);
      handle_ = TTraits::kInvalid;
    }
  }

 private:
  THandle handle_;
};

using KernelHandle = KernelObjectHandle<HANDLE, ZwKernelHandleTraits>;
#else
struct ZwKernelHandleTraits {};
template <typename THandle, typename TTraits>
class KernelObjectHandle final {
 public:
  explicit KernelObjectHandle(THandle = nullptr) noexcept {}
};
#endif

}  // namespace win_ai_agent::kernel

#endif  // WIN_AI_AGENT_KERNEL_KERNEL_OBJECT_HANDLE_HPP
