#pragma once

#include "src/core/firmware_persistence/BootOrderReader.hpp"
#include "src/core/firmware_persistence/BootOrderVerifier.hpp"
#include "src/core/firmware_persistence/FlashJsonPackager.hpp"
#include "src/core/firmware_persistence/FirmwarePrivilegeInspector.hpp"
#include "src/core/firmware_persistence/NVRAMGuidProvider.hpp"
#include "src/core/firmware_persistence/NVRAMReadClient.hpp"
#include "src/core/firmware_persistence/SecureBootAuditBridge.hpp"

#include <expected>

namespace win_ai_agent::nexus_pro {

class FlashIntegrityReportBuilder {
 public:
  [[nodiscard]] std::expected<FlashIntegrityReport, WinErrorStruct> Build() const;
};

}  // namespace win_ai_agent::nexus_pro
