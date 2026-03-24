#include "EnterpriseSuite/Modules/CommandParserImpl.h"

#include <sstream>
#include <utility>

#include <nlohmann/json.hpp>

namespace EnterpriseSuite::Modules {
namespace {

constexpr std::size_t kMaxPacketBytes = 64 * 1024;

std::string ToString(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    }
    return value.dump();
}

}  // namespace

JsonCommandParser::JsonCommandParser()
    : allowlist_(DefaultAllowlist()) {}

JsonCommandParser::JsonCommandParser(std::unordered_map<std::string, CommandRule> allowlist)
    : allowlist_(std::move(allowlist)) {}

Common::OperationResult JsonCommandParser::Validate(const std::string& raw_command) const {
    return ValidateStructureAndAllowlist(raw_command);
}

ParsedCommand JsonCommandParser::Parse(const std::string& raw_command) const {
    const auto validation_result = ValidateStructureAndAllowlist(raw_command);
    if (!validation_result.success) {
        return {};
    }

    const auto root = nlohmann::json::parse(raw_command);
    ParsedCommand command;
    command.verb = root.at("command").get<std::string>();

    if (root.contains("args") && root.at("args").is_object()) {
        for (const auto& [key, value] : root.at("args").items()) {
            command.named_args.emplace(key, ToString(value));
        }
    }

    return command;
}

bool JsonCommandParser::IsAllowlisted(const ParsedCommand& command) const {
    const std::scoped_lock lock(mutex_);
    return allowlist_.contains(command.verb);
}

Common::OperationResult JsonCommandParser::ValidateStructureAndAllowlist(const std::string& raw_command) const {
    if (raw_command.empty()) {
        return {.success = false, .code = "EMPTY_PACKET", .message = "Incoming packet is empty."};
    }

    if (raw_command.size() > kMaxPacketBytes) {
        return {.success = false,
                .code = "PACKET_TOO_LARGE",
                .message = "Incoming packet exceeds the 64 KiB local IPC limit."};
    }

    nlohmann::json root;
    try {
        root = nlohmann::json::parse(raw_command);
    } catch (const nlohmann::json::exception& ex) {
        return {.success = false, .code = "INVALID_JSON", .message = ex.what()};
    }

    if (!root.is_object()) {
        return {.success = false, .code = "INVALID_FORMAT", .message = "Packet must be a JSON object."};
    }

    if (!root.contains("command") || !root.at("command").is_string()) {
        return {.success = false, .code = "MISSING_COMMAND", .message = "Packet must contain string field 'command'."};
    }

    if (root.contains("args") && !root.at("args").is_object()) {
        return {.success = false, .code = "INVALID_ARGS", .message = "Optional field 'args' must be an object."};
    }

    const std::string verb = root.at("command").get<std::string>();

    const std::scoped_lock lock(mutex_);
    const auto allowlist_it = allowlist_.find(verb);
    if (allowlist_it == allowlist_.end()) {
        return {.success = false, .code = "COMMAND_NOT_ALLOWLISTED", .message = "Command is not in diagnostic allowlist."};
    }

    const auto& rule = allowlist_it->second;

    std::unordered_set<std::string> provided;
    if (root.contains("args")) {
        for (const auto& [key, _] : root.at("args").items()) {
            provided.insert(key);
            if (!rule.required_args.contains(key) && !rule.optional_args.contains(key)) {
                std::ostringstream oss;
                oss << "Argument '" << key << "' is not allowed for command '" << verb << "'.";
                return {.success = false, .code = "ARGUMENT_NOT_ALLOWLISTED", .message = oss.str()};
            }
        }
    }

    for (const auto& required : rule.required_args) {
        if (!provided.contains(required)) {
            std::ostringstream oss;
            oss << "Required argument '" << required << "' is missing for command '" << verb << "'.";
            return {.success = false, .code = "MISSING_REQUIRED_ARGUMENT", .message = oss.str()};
        }
    }

    return {.success = true, .code = "OK", .message = "Packet is valid."};
}

std::unordered_map<std::string, JsonCommandParser::CommandRule> JsonCommandParser::DefaultAllowlist() {
    return {
        {"get_service_status", {.required_args = {}, .optional_args = {"include_dependencies"}}},
        {"get_system_health", {.required_args = {}, .optional_args = {"detail_level"}}},
        {"list_recent_events", {.required_args = {"max_items"}, .optional_args = {"severity"}}},
        {"read_config_value", {.required_args = {"key"}, .optional_args = {}}},
        {"run_diagnostics", {.required_args = {}, .optional_args = {"scope"}}},
    };
}

}  // namespace EnterpriseSuite::Modules
