	XREF _main
	XDEF _putch
	XDEF __putch

    SEGMENT CODE
	.ASSUME ADL=1

	JP _start
	
	ALIGN 64
	DB	"MOS"
	DB	00h
	DB	01h

_start:			PUSH	AF		; Preserve registers
			PUSH	BC
			PUSH	DE
			PUSH	IX
			PUSH	IY		; Preserve this
;
			PUSH	HL		; argv[0] = HL (pointer to the parameters)
			LD	HL, 1
			PUSH	HL		; argc
			CALL	_main		; int main(int argc, char *argv[])
			POP	DE		; Balance the stack
			POP	DE

			POP	IY		; Restore registers
			POP	IX
			POP	DE
			POP 	BC
			POP	AF
			RET

__putch:
_putch:
	push iy
	ld iy,0
	add iy,sp
	push af
	ld a,(iy+6)
	rst.lil 10h
	ld hl,0
	ld l,a
	pop af
	ld sp,iy
	pop iy
	ret

