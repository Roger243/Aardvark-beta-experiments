#include "src/core/integrity/BcdStoreReader.hpp"

#include "src/core/integrity/BcdValueConverter.hpp"

#ifdef _WIN32
#include <windows.h>
#include <bcd.h>
#endif

namespace win_ai_agent::nexus_pro {

std::expected<std::vector<BcdElementRecord>, WinErrorStruct> BcdStoreReader::ReadBootManagerElements() const {
#ifdef _WIN32
  HANDLE store{};
  if (::BcdOpenStore(nullptr, &store) != ERROR_SUCCESS) {
    return std::unexpected(MakeWinError("BcdOpenStore"));
  }

  std::vector<BcdElementRecord> records;
  // Placeholder for complete BcdEnumerateElements flow.
  records.push_back(BcdElementRecord{0x16000048, 0, BcdValueConverter::ElementTypeToString(0x16000048)});
  ::CloseHandle(store);
  return records;
#else
  return std::unexpected(MakeWinError("BcdOpenStore"));
#endif
}

}  // namespace win_ai_agent::nexus_pro
