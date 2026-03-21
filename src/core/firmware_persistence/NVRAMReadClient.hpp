#pragma once

#include "src/core/firmware_persistence/NVRAMStateTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

class NVRAMReadClient {
 public:
  [[nodiscard]] std::expected<NvramMetadataRecord, WinErrorStruct> ReadMetadata(const std::wstring& variable_name,
                                                                                 const std::wstring& variable_guid) const;
  [[nodiscard]] std::expected<void, WinErrorStruct> WriteMetadataBlocked() const;
};

}  // namespace win_ai_agent::nexus_pro
