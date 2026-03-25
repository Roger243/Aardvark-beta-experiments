#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace WinAgent::KernelEventMonitor {

enum class EventType : std::uint8_t {
    ProcessStart,
    ProcessStop,
    ThreadStart,
    ThreadStop,
    Unknown
};

struct KernelEvent {
    EventType type{EventType::Unknown};
    std::uint32_t processId{0};
    std::uint32_t threadId{0};
    std::wstring imageName;
    std::uint64_t timestampQpc{0};
};

using EventCallback = std::function<void(const KernelEvent&)>;

class Core {
public:
    Core();
    ~Core();

    Core(const Core&) = delete;
    Core& operator=(const Core&) = delete;

    bool Start(EventCallback callback);
    void Stop();
    bool IsRunning() const noexcept;

private:
    bool running_{false};
};

} // namespace WinAgent::KernelEventMonitor
