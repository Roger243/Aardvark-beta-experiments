#pragma once

#include <functional>
#include <string>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

struct ConfigChangeEvent {
    std::string source;
    std::string path;
    std::string key;
    std::string old_value;
    std::string new_value;
};

class ISystemConfigMonitor {
public:
    using EventHandler = std::function<void(const ConfigChangeEvent&)>;

    virtual ~ISystemConfigMonitor() = default;

    virtual Common::OperationResult StartMonitoring() = 0;
    virtual Common::OperationResult StopMonitoring() = 0;
    virtual Common::OperationResult AddRegistryWatch(const std::string& registry_path) = 0;
    virtual Common::OperationResult AddFileWatch(const std::string& file_path) = 0;
    virtual void SetEventHandler(EventHandler handler) = 0;
};

}  // namespace EnterpriseSuite::Modules
