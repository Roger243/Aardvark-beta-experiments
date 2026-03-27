%include "constants.inc"

extern winagent_cpuid_leaf1
extern winagent_cpuid_leaf7_ebx

global winagent_cpu_feature_mask
section .text

; uint32_t winagent_cpu_feature_mask()
; conservative CPU capability fingerprint for diagnostics
winagent_cpu_feature_mask:
    push rbx
    xor ebx, ebx

    call winagent_cpuid_leaf1
    mov r8, rax

    ; SSE2 (EDX bit 26)
    mov r9, r8
    shr r9, 32
    bt r9d, 26
    jnc .check_sse42
    or ebx, FEAT_SSE2

.check_sse42:
    ; SSE4.2 (ECX bit 20)
    bt r8d, 20
    jnc .check_avx
    or ebx, FEAT_SSE4_2

.check_avx:
    ; AVX (ECX bit 28)
    bt r8d, 28
    jnc .check_aes
    or ebx, FEAT_AVX

.check_aes:
    ; AES-NI (ECX bit 25)
    bt r8d, 25
    jnc .leaf7
    or ebx, FEAT_AESNI

.leaf7:
    call winagent_cpuid_leaf7_ebx

    ; AVX2 (EBX bit 5)
    bt eax, 5
    jnc .check_sha
    or ebx, FEAT_AVX2

.check_sha:
    ; SHA extensions (EBX bit 29)
    bt eax, 29
    jnc .done
    or ebx, FEAT_SHA

.done:
    mov eax, ebx
    pop rbx
    ret
