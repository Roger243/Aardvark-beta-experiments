%include "constants.inc"

global winagent_cpuid_leaf1
section .text

; uint64_t winagent_cpuid_leaf1()
; returns: eax low 32 bits = ECX features, high 32 bits = EDX features
winagent_cpuid_leaf1:
    push rbx
    mov eax, 1
    xor ecx, ecx
    cpuid

    mov eax, ecx
    mov edx, edx
    shl rdx, 32
    or rax, rdx

    pop rbx
    ret
