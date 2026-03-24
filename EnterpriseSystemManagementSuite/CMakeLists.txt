cmake_minimum_required(VERSION 3.23)

project(EnterpriseSystemManagementSuite VERSION 1.0.0 LANGUAGES CXX)

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
