#pragma once

#include <string>
#include <vector>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

struct PrivilegeState {
    std::string privilege_name;
    bool enabled{false};
};

class IPrivilegeChecker {
public:
    virtual ~IPrivilegeChecker() = default;

    virtual Common::SecurityContext CurrentSecurityContext() const = 0;
    virtual bool IsRunningAsLocalSystem() const = 0;
    virtual bool IsRunningAsAdministrator() const = 0;
    virtual std::vector<PrivilegeState> EnumerateTokenPrivileges() const = 0;
    virtual bool HasPrivilege(const std::string& privilege_name) const = 0;
};

}  // namespace EnterpriseSuite::Modules
