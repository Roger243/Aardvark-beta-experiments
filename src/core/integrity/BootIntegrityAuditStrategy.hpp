#pragma once

#include "src/core/integrity/BcdStoreReader.hpp"
#include "src/core/integrity/IntegrityAuditStrategy.hpp"

namespace win_ai_agent::nexus_pro {

class BootIntegrityAuditStrategy final : public IntegrityAuditStrategy {
 public:
  [[nodiscard]] std::expected<nlohmann::json, WinErrorStruct> RunAudit() override;

 private:
  BcdStoreReader reader_;
};

}  // namespace win_ai_agent::nexus_pro
