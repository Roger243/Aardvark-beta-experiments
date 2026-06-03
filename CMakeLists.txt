cmake_minimum_required(VERSION 3.20)
project(winagent_ai LANGUAGES CXX)

add_executable(winagent-ai
    src/main.cpp
    src/logistic_regression.cpp
)

target_include_directories(winagent-ai PRIVATE include)
target_compile_features(winagent-ai PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(winagent-ai PRIVATE /W4 /WX /permissive-)
else()
    target_compile_options(winagent-ai PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
