IFDEF _M_IX86
.MODEL FLAT, C
ENDIF

INCLUDE KNSoft\NDK\Assembly\Header.inc
INCLUDE KNSoft\NDK\Assembly\NDK.inc

.CODE

AsmMacro PROC

    .ERRE $MAKEDWORD2(1, 2, 3, 4) EQ 004030201h

    mov eax, 1
    ret

AsmMacro ENDP

END
