#include "Core.h"

#include <utility>

namespace WinAgent::KernelEventMonitor {

bool StartEtwSession(const EventCallback& callback);
void StopEtwSession();

Core::Core() = default;
Core::~Core()
{
    Stop();
}

bool Core::Start(EventCallback callback)
{
    if (running_) {
        return true;
    }

    running_ = StartEtwSession(callback);
    return running_;
}

void Core::Stop()
{
    if (!running_) {
        return;
    }

    StopEtwSession();
    running_ = false;
}

bool Core::IsRunning() const noexcept
{
    return running_;
}

} // namespace WinAgent::KernelEventMonitor
