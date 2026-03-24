#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "EnterpriseSuite/Modules/LocalCommServer.h"

namespace EnterpriseSuite::Modules {

class NamedPipeLocalCommServer final : public ILocalCommServer {
public:
    NamedPipeLocalCommServer();
    ~NamedPipeLocalCommServer() override;

    Common::OperationResult Start(const std::string& endpoint_name) override;
    Common::OperationResult Stop() override;
    bool IsListening() const override;
    void SetMessageHandler(MessageHandler handler) override;

private:
    void ServerLoop();

    std::string BuildPipeName(const std::string& endpoint_name) const;

    std::atomic<bool> listening_{false};
    std::string endpoint_name_;
    MessageHandler handler_;

    mutable std::mutex mutex_;
    std::thread server_thread_;
};

}  // namespace EnterpriseSuite::Modules
