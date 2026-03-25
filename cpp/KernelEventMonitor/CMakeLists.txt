add_library(kernel_event_monitor STATIC
    src/Core.cpp
    src/WinApiWrapper.cpp
    src/EtwSession.cpp
    src/EventParser.cpp
)

target_include_directories(kernel_event_monitor
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(kernel_event_monitor PUBLIC cxx_std_20)

if(WIN32)
    target_link_libraries(kernel_event_monitor PRIVATE advapi32)
endif()
