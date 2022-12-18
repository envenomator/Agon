	.include "mos_api.inc"
			
	SEGMENT CODE
	.ASSUME	ADL = 1
			
	XDEF	_getfullkeycode
	
_getfullkeycode:
	push ix
	ld a, mos_sysvars		; MOS call for mos_sysvars
	rst.lil 08h				; returns pointer to sysvars in ixu

waitkey:
	ld a, (ix+sysvar_keycode)
	ld h, (ix+sysvar_keymods)
	or a
	jr z, waitkey
	ld l,a
	xor a				; acknowledge receipt of key / debounce key
	ld (ix+sysvar_keycode),a
	
	pop ix
	ret				; return hl as h:keymods, l:keycode
end