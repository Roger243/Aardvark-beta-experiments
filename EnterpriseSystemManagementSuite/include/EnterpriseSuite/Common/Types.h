#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

namespace EnterpriseSuite::Common {

enum class Severity {
    kTrace,
    kInfo,
    kWarning,
    kError,
    kCritical
};

struct OperationResult {
    bool success{false};
    std::string code;
    std::string message;
};

struct SecurityContext {
    std::string account_name;
    std::string sid;
    bool is_local_system{false};
    bool is_administrator{false};
};

struct TimeWindow {
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
};

}  // namespace EnterpriseSuite::Common
