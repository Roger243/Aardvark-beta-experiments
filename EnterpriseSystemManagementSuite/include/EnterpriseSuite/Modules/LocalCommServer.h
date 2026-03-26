#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

struct LocalMessage {
    std::string client_id;
    std::vector<std::uint8_t> payload;
};

class ILocalCommServer {
public:
    using MessageHandler = std::function<std::vector<std::uint8_t>(const LocalMessage&)>;

    virtual ~ILocalCommServer() = default;

    virtual Common::OperationResult Start(const std::string& endpoint_name) = 0;
    virtual Common::OperationResult Stop() = 0;
    virtual bool IsListening() const = 0;
    virtual void SetMessageHandler(MessageHandler handler) = 0;
};

}  // namespace EnterpriseSuite::Modules
