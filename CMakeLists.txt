cmake_minimum_required(VERSION 3.23)
project(WindowsAIAutonomousAgent VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WIN_AGENT_BUILD_TESTS "Build tests" OFF)

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

if(MSVC)
    target_compile_options(agent_modules PRIVATE /W4 /MT /std:c++20)
    target_compile_options(ai_agent PRIVATE /W4 /MT /std:c++20)
    target_link_options(ai_agent PRIVATE /SUBSYSTEM:WINDOWS)
endif()

if(WIN_AGENT_BUILD_TESTS)
    enable_testing()
endif()
