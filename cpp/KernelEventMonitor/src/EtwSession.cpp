#include "Core.h"

namespace WinAgent::KernelEventMonitor {

namespace {
EventCallback g_callback;
}

bool StartEtwSession(const EventCallback& callback)
{
    // Safe scaffold: wiring is intentionally minimal.
    // Production integration should subscribe to kernel process/thread providers
    // and parse payloads through EventParser.cpp.
    g_callback = callback;
    return true;
}

void StopEtwSession()
{
    g_callback = nullptr;
}

} // namespace WinAgent::KernelEventMonitor
