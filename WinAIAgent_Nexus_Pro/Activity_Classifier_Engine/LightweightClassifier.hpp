#ifndef WIN_AI_AGENT_NEXUS_PRO_ACTIVITY_CLASSIFIER_ENGINE_LIGHTWEIGHTCLASSIFIER_HPP
#define WIN_AI_AGENT_NEXUS_PRO_ACTIVITY_CLASSIFIER_ENGINE_LIGHTWEIGHTCLASSIFIER_HPP

#include <any>
#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::activity_classifier_engine {

class LightweightClassifier final {
 public:
  std::expected<std::any, std::string> Execute(const std::any& input) const;
};

}  // namespace win_ai_agent_nexus_pro::activity_classifier_engine

#endif
