cmake_minimum_required(VERSION 3.23)

project(EnterpriseSystemManagementSuite VERSION 1.0.0 LANGUAGES CXX)

option(ENTERPRISE_SUITE_BUILD_IMPL "Build EnterpriseSuite Windows implementations" ON)

add_library(enterprise_system_management_suite_interfaces INTERFACE)
add_library(EnterpriseSuite::Interfaces ALIAS enterprise_system_management_suite_interfaces)

target_compile_features(enterprise_system_management_suite_interfaces INTERFACE cxx_std_20)

target_include_directories(enterprise_system_management_suite_interfaces
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_options(enterprise_system_management_suite_interfaces INTERFACE /W4 /permissive-)
else()
    target_compile_options(enterprise_system_management_suite_interfaces INTERFACE -Wall -Wextra -Wpedantic)
endif()

if(ENTERPRISE_SUITE_BUILD_IMPL)
    add_library(enterprise_system_management_suite_impl STATIC
        src/Modules/LocalCommServer.cpp
        src/Modules/CommandParser.cpp
    )
    add_library(EnterpriseSuite::Impl ALIAS enterprise_system_management_suite_impl)

    target_link_libraries(enterprise_system_management_suite_impl
        PUBLIC
            EnterpriseSuite::Interfaces
    )

    target_compile_features(enterprise_system_management_suite_impl PUBLIC cxx_std_20)

    target_include_directories(enterprise_system_management_suite_impl
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
    )

    find_package(nlohmann_json REQUIRED)
    target_link_libraries(enterprise_system_management_suite_impl PRIVATE nlohmann_json::nlohmann_json)

    if(WIN32)
        target_link_libraries(enterprise_system_management_suite_impl PRIVATE advapi32)
    endif()
endif()
