#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace win_ai_agent::nexus_pro {

struct BcdElementRecord {
  std::uint32_t type{0};
  std::uint64_t raw_value{0};
  std::string description;
};

struct BootIntegrityFinding {
  bool testsigning_enabled{false};
  bool nointegritychecks_enabled{false};
  std::vector<BcdElementRecord> scanned_elements;
};

}  // namespace win_ai_agent::nexus_pro
