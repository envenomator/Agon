; Print a zero-terminated string
; HL: Pointer to string
printString:
	PUSH	BC
	LD		BC,0
	LD 	 	A,0
	RST.LIL 18h
	POP		BC
	RET
; print a VDU sequence
; HL: Pointer to VDU sequence - <1 byte length> <data>
sendVDUsequence:
	PUSH	BC
	LD		BC, 0
	LD		C, (HL)
	RST.LIL	18h
	POP		BC
	RET
; Print Newline sequence to VDP
printNewline:
	LD	A, '\r'
	RST.LIL 10h
	LD	A, '\n'
	RST.LIL 10h
	RET
; Print a 24-bit HEX number
; HLU: Number to print
printHex24:
	PUSH	HL
	LD		HL, 2
	ADD		HL, SP
	LD		A, (HL)
	POP		HL
	CALL	printHex8
; Print a 16-bit HEX number
; HL: Number to print
printHex16:
	LD		A,H
	CALL	printHex8
	LD		A,L
; Print an 8-bit HEX number
; A: Number to print
printHex8:
	LD		C,A
	RRA 
	RRA 
	RRA 
	RRA 
	CALL	@F
	LD		A,C
@@:
	AND		0Fh
	ADD		A,90h
	DAA
	ADC		A,40h
	DAA
	RST.LIL	10h
	RET

; Print a 0x HEX prefix
DisplayHexPrefix:
	LD	A, '0'
	RST.LIL 10h
	LD	A, 'x'
	RST.LIL 10h
	RET

; Prints the decimal value in HL without leading zeroes
; HL : Value to print
printDec:
	LD	 DE, _printDecBuffer
	CALL Num2String
	LD	 HL, _printDecBuffer
	CALL printString
	RET
_printDecBuffer:
	DS 9
; This routine converts the value from HL into it's ASCII representation, 
; starting to memory location pointing by DE, in decimal form and with trailing zeroes 
; so it will allways be 5 characters length
; HL : Value to convert to string
; DE : pointer to buffer, at least 8 byte + 0
Num2String:
	PUSH DE
	CALL Num2String_worker
	LD	 A, 0
	LD	 (DE), A	; terminate string
	POP  DE
	PUSH DE
@findfirstzero:
	LD	 A, (DE)
	CP	 '0'
	JR	 NZ, @done
	INC  DE
	JR	 @findfirstzero
@done:
	OR	 A	; end-of-string reached / was the value 0?
	JR	 NZ, @removezeroes
	DEC  DE
@removezeroes:
	POP	 HL	; start of string, DE == start of first number
@copydigit:
	LD	A, (DE)
	LD	(HL), A
	OR  A
	RET	Z
	INC	HL
	INC DE
	JR	@copydigit

Num2String_worker:
	LD	 BC,-10000000
	CALL OneDigit
	LD	 BC,-1000000
	CALL OneDigit
	LD	 BC,-100000
	CALL OneDigit
	LD   BC,-10000
	CALL OneDigit
	LD   BC,-1000
	CALL OneDigit
	LD   BC,-100
	CALL OneDigit
	LD   C,-10
	CALL OneDigit
	LD   C,B
OneDigit:
	LD   A,'0'-1
DivideMe:
	INC  A
	ADD  HL,BC
	JR   C,DivideMe
	SBC  HL,BC
	LD   (DE),A
	INC  DE
	RET
