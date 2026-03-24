cmake_minimum_required(VERSION 3.23)
project(WindowsAIAutonomousAgent VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WIN_AGENT_BUILD_TESTS "Build tests" OFF)
option(WIN_AGENT_BUILD_ENTERPRISE_INTERFACES "Build Enterprise System Management Suite interfaces" ON)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(agent_modules STATIC ${SOURCES})

target_include_directories(agent_modules
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(agent_modules
    PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
)

if(WIN32)
    target_link_libraries(agent_modules
        PRIVATE
            winhttp
            fwpuclnt
            wtsapi32
            user32
            advapi32
            setupapi
            cfgmgr32
            vssapi
            secur32
            ntdll
            crypt32
            psapi
            credui
    )
endif()

add_executable(ai_agent agent.cpp)
target_link_libraries(ai_agent PRIVATE agent_modules)

add_executable(ai_agent_service service_main.cpp)
target_link_libraries(ai_agent_service PRIVATE agent_modules)

# -----------------------------
# Managed WinAgent auto-discovery
# -----------------------------
# Discover every C# source file in the repository and mirror its folder layout in VS.
file(GLOB_RECURSE CSHARP_SOURCES
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "*.cs"
)

if(CSHARP_SOURCES)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CSHARP_SOURCES})
endif()

# Explicitly locate Program.cs for managed entrypoint wiring.
set(WINAGENT_PROGRAM_CS "")
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Program.cs")
    set(WINAGENT_PROGRAM_CS "Program.cs")
endif()

# C# targets are only supported by Visual Studio generators.
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    enable_language(CSharp)

    if(CSHARP_SOURCES)
        if(WINAGENT_PROGRAM_CS)
            add_executable(winagent_managed ${WINAGENT_PROGRAM_CS} ${CSHARP_SOURCES})
        else()
            # Keep sources visible/organized in the IDE even when Program.cs is not present yet.
            add_library(winagent_managed INTERFACE)
            target_sources(winagent_managed INTERFACE ${CSHARP_SOURCES})
            message(WARNING "Program.cs not found at repository root; managed executable target not created.")
        endif()

        if(TARGET winagent_managed AND WINAGENT_PROGRAM_CS)
            # Framework/Core references used by WMI, DPAPI, Registry, and common base APIs.
            set_target_properties(winagent_managed PROPERTIES
                DOTNET_TARGET_FRAMEWORK "net8.0-windows"
                VS_DOTNET_REFERENCES "System;System.Core;System.Management;System.Security;Microsoft.Win32.Registry"
            )
        endif()
    endif()
endif()

if(MSVC)
    target_compile_options(agent_modules PRIVATE /W4 /MT /std:c++20)
    target_compile_options(ai_agent PRIVATE /W4 /MT /std:c++20)
    target_compile_options(ai_agent_service PRIVATE /W4 /MT /std:c++20)
    target_link_options(ai_agent PRIVATE /SUBSYSTEM:WINDOWS)
    target_link_options(ai_agent_service PRIVATE /SUBSYSTEM:WINDOWS)
endif()

if(WIN_AGENT_BUILD_TESTS)
    enable_testing()
endif()

if(WIN_AGENT_BUILD_ENTERPRISE_INTERFACES)
    add_subdirectory(EnterpriseSystemManagementSuite)
endif()
