;
; Title:		AGON MOS - MOS assembly interface
; Author:		Jeroen Venema
; Created:		15/10/2022
; Last Updated:	15/10/2022
; 
; Modinfo:
; 15/10/2022:		Added _putch, _getch
; 21/10/2022:		Added _puts
; 22/10/2022:		Added _waitvblank
;
	.include "mos_api.inc"

	XDEF _putch
	XDEF _getch
	XDEF _puts
	XDEF _waitvblank
	
	segment CODE
	.assume ADL=1
	
_putch:
	push ix
	ld ix,0
	add ix, sp
	
	ld a, (ix+6)
	push ix
	rst.lis 16
	pop ix
	
	ld sp,ix
	pop ix
	ret

_getch:
	ld a, mos_sysvars			; MOS Call for mos_sysvars
	rst.lis 08h					; returns pointer to sysvars in ixu
_getch0:
	ld a, (ix+sysvar_keycode)	; get current keycode
	or a,a
	jr z, _getch0				; wait for keypress
	
	push af						; debounce key, reload keycode with 0
	xor a
	ld (ix+sysvar_keycode),a
	pop af
	
	ret

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

_waitvblank:
	push ix
	ld a, mos_sysvars
	rst.lis 08h
	ld a, (ix + sysvar_time + 0)
$$:	cp a, (ix + sysvar_time + 0)
	jr z, $b
	pop ix
	ret

end

	
