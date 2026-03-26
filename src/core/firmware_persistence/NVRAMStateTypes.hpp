#pragma once

#include <string>

namespace win_ai_agent::nexus_pro {

struct NvramMetadataRecord {
  std::string key;
  std::string value;
  bool read_only{true};
};

}  // namespace win_ai_agent::nexus_pro
