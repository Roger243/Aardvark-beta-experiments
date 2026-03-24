cmake_minimum_required(VERSION 3.23)
project(WinAIAgent_Nexus_Pro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(nlohmann_json REQUIRED)

file(GLOB_RECURSE NEXUS_PRO_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp")

add_library(win_ai_agent_nexus_pro STATIC ${NEXUS_PRO_SOURCES})

target_include_directories(win_ai_agent_nexus_pro
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(win_ai_agent_nexus_pro
    PRIVATE
      nlohmann_json::nlohmann_json
)

if(WIN32)
  target_link_libraries(win_ai_agent_nexus_pro
      PRIVATE
        winhttp
        wtsapi32
        psapi
        fwpuclnt
        d2d1
        ntdll
        advapi32
        user32)
endif()
