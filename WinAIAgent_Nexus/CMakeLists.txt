cmake_minimum_required(VERSION 3.23)
project(WinAIAgent_Nexus VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(nlohmann_json REQUIRED)

set(NEXUS_SOURCES
    Core_Engine/AgentOrchestrator.cpp
    Core_Engine/InternalMessageBus.cpp
    Core_Engine/ServiceController.cpp
    Core_Engine/GuardDog_Safety.cpp
    Core_Engine/GlobalExceptionManager.cpp

    Kernel_Bridge/ProcessMonitor.cpp
    Kernel_Bridge/MemoryAuditor.cpp
    Kernel_Bridge/SafetyFilter_Hardened.cpp
    Kernel_Bridge/HandleScanner.cpp
    Kernel_Bridge/KernelRemediation.cpp

    Registry_Subsystem/RegWatcher_Async.cpp
    Registry_Subsystem/RegTransaction_Safe.cpp
    Registry_Subsystem/RegBackup_Manager.cpp
    Registry_Subsystem/UserPreferences_Engine.cpp
    Registry_Subsystem/SystemPolicy_Auditor.cpp

    Winlogon_Nexus/DesktopSwitcher.cpp
    Winlogon_Nexus/OverlayRenderer_GDI.cpp
    Winlogon_Nexus/TokenManipulator.cpp
    Winlogon_Nexus/LogonEvent_Hook.cpp
    Winlogon_Nexus/CredentialBridge.cpp

    AI_Network_Vault/OpenAI_Client_WinHttp.cpp
    AI_Network_Vault/TelemetryPackager_JSON.cpp
    AI_Network_Vault/ScriptSandbox_Executor.cpp
    AI_Network_Vault/SecureVault_AES.cpp
    AI_Network_Vault/LogAggregator_ETW.cpp
)

add_library(win_ai_agent_nexus STATIC ${NEXUS_SOURCES})

target_include_directories(win_ai_agent_nexus
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(win_ai_agent_nexus
    PRIVATE
        nlohmann_json::nlohmann_json
)

if(WIN32)
    target_link_libraries(win_ai_agent_nexus
        PRIVATE
            advapi32
            user32
            wtsapi32
            psapi
            winhttp
            wevtapi
    )
endif()
