#include "src/core/firmware_persistence/FlashIntegrityReport.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<FlashIntegrityReport, WinErrorStruct> FlashIntegrityReportBuilder::Build() const {
  NVRAMGuidProvider guid;
  NVRAMReadClient nvram;
  const auto meta = nvram.ReadMetadata(L"WinAIAgentMeta", guid.AgentNamespaceGuid());
  if (!meta) return std::unexpected(meta.error());

  BootOrderReader boot_reader;
  const auto boot = boot_reader.ReadBootOrder();
  if (!boot) return std::unexpected(boot.error());

  SecureBootAuditBridge secure_bridge;
  const auto secure = secure_bridge.BuildAudit();
  if (!secure) return std::unexpected(secure.error());

  FirmwarePrivilegeInspector inspector;
  const auto priv = inspector.InspectCurrentToken();
  if (!priv) return std::unexpected(priv.error());

  FlashJsonPackager packager;
  return packager.Build(
      {{"key", meta->key}, {"value", meta->value}, {"read_only", meta->read_only}},
      {{"entry_count", boot->entries.size()}},
      *secure,
      {{"has_system_environment_privilege", priv->has_system_environment_privilege}, {"detail", priv->detail}});
}

}  // namespace win_ai_agent::nexus_pro
