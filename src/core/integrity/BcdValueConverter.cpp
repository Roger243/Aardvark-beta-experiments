#include "src/core/integrity/BcdValueConverter.hpp"

namespace win_ai_agent::nexus_pro {

std::string BcdValueConverter::ElementTypeToString(std::uint32_t type) {
  switch (type) {
    case 0x16000049:
      return "BcdLibraryBoolean_AllowPrereleaseSignatures";
    case 0x16000048:
      return "BcdOSLoaderBoolean_TestSigning";
    default:
      return "Unknown element 0x" + std::to_string(type);
  }
}

std::string BcdValueConverter::FlagValueToString(std::uint64_t value) {
  return value == 0 ? "disabled" : "enabled";
}

}  // namespace win_ai_agent::nexus_pro
