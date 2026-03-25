#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace WinAgent::KernelEventMonitor {

struct SyscallInfo {
    std::uint32_t id;
    std::string_view name;
};

// Diagnostic-only static map for known process lifecycle-related syscalls.
inline constexpr std::array<SyscallInfo, 4> kLifecycleSyscalls{{
    {0x26, "NtCreateUserProcess"},
    {0x2C, "NtCreateThreadEx"},
    {0x50, "NtTerminateProcess"},
    {0x53, "NtTerminateThread"},
}};

} // namespace WinAgent::KernelEventMonitor
