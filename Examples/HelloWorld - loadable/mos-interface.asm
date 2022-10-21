;
; Title:		AGON MOS - MOS assembly interface
; Author:		Jeroen Venema
; Created:		15/10/2022
; Last Updated:	15/10/2022
; 
; Modinfo:
; 15/10/2022:		Added _putch, _getch
;
	.include "mos_api.inc"

	XDEF _putch
	XDEF _getch
	
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
end

	
