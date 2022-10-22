;
; Title:		AGON MOS - MOS assembly interface
; Author:		Jeroen Venema
; Created:		15/10/2022
; Last Updated:	15/10/2022
; 
; Modinfo:
; 15/10/2022:		Added _putch, _getch
; 21/10/2022:		Added _puts
; 22/10/2022:		Added _waitvblank, _mos_f* file functions
;
	.include "mos_api.inc"

	XDEF _putch
	XDEF _getch
	XDEF _puts
	XDEF _waitvblank
	XDEF _mos_fopen
	XDEF _mos_fclose
	XDEF _mos_fgetc
	XDEF _mos_fputc
	XDEF _mos_feof
	
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
	jr z, $B
	pop ix
	ret

_mos_fopen:
	push ix
	ld ix,0
	add ix, sp
	
	ld hl, (ix+6)	; address to 0-terminated filename in memory
	ld c,  (ix+9)	; mode : fa_read / fa_write etc
	ld a, mos_fopen
	rst.lis 08h		; returns filehandle in A
	
	ld sp,ix
	pop ix
	ret	

_mos_fclose:
	push ix
	ld ix,0
	add ix, sp
	
	ld c, (ix+6)	; filehandle, or 0 to close all files
	ld a, mos_fclose
	rst.lis 08h		; returns number of files still open in A
	
	ld sp,ix
	pop ix
	ret	

_mos_fgetc:
	push ix
	ld ix,0
	add ix, sp
	
	ld c, (ix+6)	; filehandle
	ld a, mos_fgetc
	rst.lis 08h		; returns character in A
	
	ld sp,ix
	pop ix
	ret	

_mos_fputc:
	push ix
	ld ix,0
	add ix, sp
	
	ld c, (ix+6)	; filehandle
	ld b, (ix+9)	; character to write
	ld a, mos_fputc
	rst.lis 08h		; returns nothing
	
	ld sp,ix
	pop ix
	ret	

_mos_feof:
	push ix
	ld ix,0
	add ix, sp
	
	ld c, (ix+6)	; filehandle
	ld a, mos_feof
	rst.lis 08h		; returns A: 1 at End-of-File, 0 otherwise
	
	ld sp,ix
	pop ix
	ret	

end

	
