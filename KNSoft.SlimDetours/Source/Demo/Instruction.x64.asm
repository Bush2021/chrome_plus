INCLUDE Instruction.inc

.CODE

ALIGN 16
SimpleInstructionFunc1X64 PROC

    xor     rax, rax
    mov     rax, PRESET_RETURN_VALUE
    ret

SimpleInstructionFunc1X64 ENDP

END