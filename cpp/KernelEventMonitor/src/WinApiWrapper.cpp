#include <windows.h>

#include <string>

namespace WinAgent::KernelEventMonitor {

std::wstring GetLastErrorMessage()
{
    const auto code = ::GetLastError();
    if (code == 0) {
        return L"No error.";
    }

    LPWSTR raw = nullptr;
    const DWORD size = ::FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&raw),
        0,
        nullptr);

    std::wstring message = (size > 0 && raw != nullptr) ? std::wstring(raw, size) : L"Unknown Win32 error.";

    if (raw != nullptr) {
        ::LocalFree(raw);
    }

    return message;
}

} // namespace WinAgent::KernelEventMonitor
