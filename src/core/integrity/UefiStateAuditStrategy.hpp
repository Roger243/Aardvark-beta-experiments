#pragma once

#include "src/core/integrity/IntegrityAuditStrategy.hpp"
#include "src/core/integrity/UefiBaselineConfig.hpp"
#include "src/core/integrity/UefiStateSerializer.hpp"
#include "src/core/integrity/UefiVariableReader.hpp"

namespace win_ai_agent::nexus_pro {

class UefiStateAuditStrategy final : public IntegrityAuditStrategy {
 public:
  explicit UefiStateAuditStrategy(std::string baseline_path);
  [[nodiscard]] std::expected<nlohmann::json, WinErrorStruct> RunAudit() override;

 private:
  std::string baseline_path_;
  UefiBaselineConfig baseline_;
  UefiVariableReader reader_;
  UefiStateSerializer serializer_;
};

}  // namespace win_ai_agent::nexus_pro
