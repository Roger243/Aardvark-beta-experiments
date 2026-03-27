#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "EnterpriseSuite/Modules/CommandParser.h"

namespace EnterpriseSuite::Modules {

class JsonCommandParser final : public ICommandParser {
public:
    struct CommandRule {
        std::unordered_set<std::string> required_args;
        std::unordered_set<std::string> optional_args;
    };

    JsonCommandParser();
    explicit JsonCommandParser(std::unordered_map<std::string, CommandRule> allowlist);

    Common::OperationResult Validate(const std::string& raw_command) const override;
    ParsedCommand Parse(const std::string& raw_command) const override;
    bool IsAllowlisted(const ParsedCommand& command) const override;

private:
    Common::OperationResult ValidateStructureAndAllowlist(const std::string& raw_command) const;
    static std::unordered_map<std::string, CommandRule> DefaultAllowlist();

    mutable std::mutex mutex_;
    std::unordered_map<std::string, CommandRule> allowlist_;
};

}  // namespace EnterpriseSuite::Modules
