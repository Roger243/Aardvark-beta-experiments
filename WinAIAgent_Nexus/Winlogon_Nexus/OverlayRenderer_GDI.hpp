#ifndef WIN_AI_AGENT_NEXUS_WINLOGON_NEXUS_OVERLAYRENDERER_GDI_HPP
#define WIN_AI_AGENT_NEXUS_WINLOGON_NEXUS_OVERLAYRENDERER_GDI_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::winlogon_nexus {

/**
 * @brief Stub interface for OverlayRenderer_GDI.
 */
class OverlayRenderer_GDI final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::winlogon_nexus

#endif
