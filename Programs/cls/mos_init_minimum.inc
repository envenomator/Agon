; Title:        mos_init_minimum.inc
;               Minimum ez80asm startup code for MOS assembly files, no parameter parsing
; Author:       Jeroen Venema
; Last update:  06/10/2023
;
     jp _init

    .align 64
    .db "MOS"
    .db 00h
    .db 01h

_init:	PUSH    AF			; Preserve the registers
		PUSH	BC
		PUSH	DE
		PUSH	IX
		PUSH	IY
		LD      A, MB			; Save MB
		PUSH 	AF
		XOR 	A
		LD  	MB, A           ; Clear to zero so MOS API calls know how to use 24-bit addresses.
			
		CALL	_main			; Start user code
		
		POP 	AF
		LD	    MB, A
		POP	    IY			    ; Restore registers
		POP	    IX
		POP	    DE
		POP	    BC
		POP	    AF
		RET