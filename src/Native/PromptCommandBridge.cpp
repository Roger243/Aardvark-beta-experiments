#include <windows.h>
#include <atomic>
#include <functional>
#include <stdexcept>

namespace WinAgent::Native {

class PromptCommandBridge {
public:
    using OpenCommandBarCallback = std::function<void()>;

    explicit PromptCommandBridge(OpenCommandBarCallback callback)
        : callback_(std::move(callback)) {
        if (!callback_) {
            throw std::invalid_argument("PromptCommandBridge callback cannot be empty");
        }
    }

    ~PromptCommandBridge() {
        Stop();
    }

    void Start() {
        if (running_.exchange(true)) {
            return;
        }

        if (!RegisterHotKey(nullptr, kHotKeyId, MOD_ALT | MOD_NOREPEAT, VK_SPACE)) {
            running_ = false;
            throw std::runtime_error("Failed to register Alt+Space hotkey for AI Command Bar");
        }
    }

    void Stop() {
        if (!running_.exchange(false)) {
            return;
        }

        UnregisterHotKey(nullptr, kHotKeyId);
    }

    bool PumpOnce() {
        MSG msg{};
        if (PeekMessage(&msg, nullptr, WM_HOTKEY, WM_HOTKEY, PM_REMOVE) == 0) {
            return false;
        }

        if (msg.message == WM_HOTKEY && msg.wParam == kHotKeyId) {
            callback_();
            return true;
        }

        return false;
    }

private:
    static constexpr int kHotKeyId = 0xA117;

    std::atomic<bool> running_{false};
    OpenCommandBarCallback callback_;
};

} // namespace WinAgent::Native

