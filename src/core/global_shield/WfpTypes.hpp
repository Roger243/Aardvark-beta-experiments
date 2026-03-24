#pragma once

#include <string>

namespace win_ai_agent::nexus_pro {

struct WfpConnectEvent {
  std::string remote_ip;
  unsigned short remote_port{0};
  std::string process_path;
};

}  // namespace win_ai_agent::nexus_pro
