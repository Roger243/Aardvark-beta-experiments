if(NOT CMAKE_ASM_NASM_COMPILER)
    message(FATAL_ERROR "NASM compiler was not found. Install NASM or disable WIN_AGENT_BUILD_ASM_MODULES.")
endif()

add_library(CpuFeatureProbe SHARED
    cpuid_info.asm
    simd_check.asm
    module.asm
)

set_target_properties(CpuFeatureProbe PROPERTIES
    OUTPUT_NAME "CpuFeatureProbe"
)
