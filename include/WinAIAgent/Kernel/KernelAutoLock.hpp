#ifndef WIN_AI_AGENT_KERNEL_KERNEL_AUTO_LOCK_HPP
#define WIN_AI_AGENT_KERNEL_KERNEL_AUTO_LOCK_HPP

namespace win_ai_agent::kernel {

template <typename TLock>
class KernelAutoLock final {
 public:
  explicit KernelAutoLock(TLock& lock) noexcept : lock_(lock) { lock_.Lock(); }
  ~KernelAutoLock() { lock_.Unlock(); }

  KernelAutoLock(const KernelAutoLock&) = delete;
  KernelAutoLock& operator=(const KernelAutoLock&) = delete;

 private:
  TLock& lock_;
};

template <typename TLock>
class KernelSharedAutoLock final {
 public:
  explicit KernelSharedAutoLock(TLock& lock) noexcept : lock_(lock) { lock_.LockShared(); }
  ~KernelSharedAutoLock() { lock_.UnlockShared(); }

  KernelSharedAutoLock(const KernelSharedAutoLock&) = delete;
  KernelSharedAutoLock& operator=(const KernelSharedAutoLock&) = delete;

 private:
  TLock& lock_;
};

}  // namespace win_ai_agent::kernel

#endif  // WIN_AI_AGENT_KERNEL_KERNEL_AUTO_LOCK_HPP
