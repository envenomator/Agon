	.ASSUME ADL=1
	.ORG $0B0000

	.include "mos_api.inc"
	.include "mos_init.s"
	.include "functions.s"

_main:
    LD HL, 567
    CALL printDec
    CALL printNewline

    LD HL, 5
    CALL printDec
    CALL printNewline

    LD HL, 16777215
    CALL printDec
    CALL printNewline

    LD HL, 0
    CALL printDec
    CALL printNewline

    LD HL, 1
    CALL printDec
    CALL printNewline

endprogram:
    LD HL,0
    RET