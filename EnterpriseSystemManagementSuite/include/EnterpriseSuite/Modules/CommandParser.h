#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

struct ParsedCommand {
    std::string verb;
    std::vector<std::string> positional_args;
    std::unordered_map<std::string, std::string> named_args;
};

class ICommandParser {
public:
    virtual ~ICommandParser() = default;

    virtual Common::OperationResult Validate(const std::string& raw_command) const = 0;
    virtual ParsedCommand Parse(const std::string& raw_command) const = 0;
    virtual bool IsAllowlisted(const ParsedCommand& command) const = 0;
};

}  // namespace EnterpriseSuite::Modules
