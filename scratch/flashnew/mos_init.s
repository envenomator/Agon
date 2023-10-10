; Title:        mos_init.inc
;               ez80asm startup code for MOS assembly files
; Author:       Jeroen Venema
; Last update:  06/10/2023
;
argv_ptrs_max:	EQU	16			; Maximum number of arguments allowed in argv

	JP _init

_exec_name:
    .DB "flash.bin",0
_header:
	.ALIGN 64
	.DB "MOS"
	.DB 00h
	.DB 01h

_init:
	PUSH	AF				; Preserve the registers
	PUSH	BC
	PUSH	DE
	PUSH	IX
	PUSH	IY
	LD		A, MB			; Save MB
	PUSH	AF
	XOR		A
	LD		MB, A			; Clear to zero so MOS API calls know how to use 24-bit addresses.
		
	LD		IX, argv_ptrs	; The argv array pointer address
	PUSH	IX
	CALL	_parse_params	; Parse the parameters
	POP		IX				; IX: argv	
	LD		B, 0			;  C: argc
	CALL	_main			; Start user code
	
	POP		AF
	LD		MB, A
	POP		IY			    ; Restore registers
	POP		IX
	POP		DE
	POP		BC
	POP		AF
	RET

; Parse the parameter string into a C array
; Parameters
; - HL: Address of parameter string
; - IX: Address for array pointer storage
; Returns:
; -  C: Number of parameters parsed
;
_parse_params:	
	LD		BC, _exec_name
	LD		(IX+0), BC			; ARGV[0] = the executable name
	LEA		IX, IX+3
	CALL	_skip_spaces		; Skip HL past any leading spaces
;
	LD		BC, 1				; C: ARGC = 1 - also clears out top 16 bits of BCU
	LD		B, argv_ptrs_max - 1; B: Maximum number of argv_ptrs
;
@@:	
	PUSH	BC			; Stack ARGC	
	PUSH	HL			; Stack start address of token
	CALL	_get_token	; Get the next token
	LD		A, C		; A: Length of the token in characters
	POP		DE			; Start address of token (was in HL)
	POP		BC			; ARGC
	OR		A			; Check for A=0 (no token found) OR at end of string
	RET		Z
;
	LD		(IX+0), DE	; Store the pointer to the token
	PUSH	HL			; DE=HL
	POP		DE
	CALL	_skip_spaces; And skip HL past any spaces onto the next character
	XOR		A
	LD		(DE), A		; Zero-terminate the token
	LEA		IX, IX+3	; Advance to next pointer position
	INC		C			; Increment ARGC
	LD		A, C		; Check for C >= A
	CP		B
	JR		C, @b		; And loop
	RET

; Get the next token
; Parameters:
; - HL: Address of parameter string
; Returns:
; - HL: Address of first character after token
; -  C: Length of token (in characters)
;
_get_token:
	LD	C, 0		; Initialise length
@@:	LD	A, (HL)		; Get the character from the parameter string
	OR	A			; Exit if 0 (end of parameter string in MOS)
	RET	Z
	CP	13			; Exit if CR (end of parameter string in BBC BASIC)
	RET	Z
	CP	' '			; Exit if space (end of token)
	RET	Z
	INC	HL			; Advance to next character
	INC	C		; Increment length
	JR	@B	
	
; Skip spaces in the parameter string
; Parameters:
; - HL: Address of parameter string
; Returns:
; - HL: Address of next none-space character
;    F: Z if at end of string, otherwise NZ if there are more tokens to be parsed
;
_skip_spaces:	
    LD	A, (HL)		; Get the character from the parameter string	
	CP	' '			; Exit if not space
	RET	NZ
	INC	HL			; Advance to next character
	JR	_skip_spaces; Increment length

; Storage for the argv array pointers
;
argv_ptrs:	BLKP	argv_ptrs_max, 0