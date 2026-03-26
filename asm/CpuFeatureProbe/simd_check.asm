%include "constants.inc"

global winagent_cpuid_leaf7_ebx
section .text

; uint32_t winagent_cpuid_leaf7_ebx()
winagent_cpuid_leaf7_ebx:
    push rbx
    mov eax, 7
    xor ecx, ecx
    cpuid
    mov eax, ebx
    pop rbx
    ret
