#pragma once

#include <optional>
#include <string>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

class IConfigManager {
public:
    virtual ~IConfigManager() = default;

    virtual Common::OperationResult LoadFromDisk(const std::string& file_path) = 0;
    virtual Common::OperationResult SaveToDisk(const std::string& file_path) const = 0;
    virtual std::optional<std::string> GetValue(const std::string& key) const = 0;
    virtual Common::OperationResult SetValue(const std::string& key, const std::string& value) = 0;
    virtual Common::OperationResult RemoveValue(const std::string& key) = 0;
};

}  // namespace EnterpriseSuite::Modules
