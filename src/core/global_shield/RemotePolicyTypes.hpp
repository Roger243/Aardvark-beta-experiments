#pragma once

#include <string>

namespace win_ai_agent::nexus_pro {

struct RemotePolicyDocument {
  std::string payload;
  std::string rsa_signature;
};

}  // namespace win_ai_agent::nexus_pro
