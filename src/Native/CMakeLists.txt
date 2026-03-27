file(GLOB_RECURSE WINAGENT_NATIVE_HEADERS CONFIGURE_DEPENDS "*/include/*.h")
file(GLOB_RECURSE WINAGENT_NATIVE_SOURCES CONFIGURE_DEPENDS "*/src/*.cpp")

add_library(winagent_native_modules STATIC ${WINAGENT_NATIVE_SOURCES} ${WINAGENT_NATIVE_HEADERS})
target_include_directories(winagent_native_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(winagent_native_modules PUBLIC cxx_std_20)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${WINAGENT_NATIVE_HEADERS} ${WINAGENT_NATIVE_SOURCES})
