;
; Title:		MISC - Misc functions
; Author:		Jeroen Venema
; Created:		21/10/2022
; Last Updated:	21/10/2022
; 
; Modinfo:
; 21/10/2022:		Added _puts
;
	XDEF _puts
	segment CODE
	.assume ADL=1
	
_puts:
	push ix			; push ix onto stack and allocate local frame
	ld ix,0
	add ix, sp

	ld hl, (ix+6)
_puts_loop:
	ld a, (hl)
_puts_testloop:
	or a
	jr z, _puts_done
	push hl
	push ix
	rst.lis 16			; output
	pop ix
	pop hl
	inc hl
	jr _puts_loop

_puts_done:
	ld sp,ix
	pop ix
	ret