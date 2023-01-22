;
; Title:		AGON MOS - MOS assembly interface
; Author:		Jeroen Venema
; Created:		15/10/2022
; Last Updated:	26/11/2022
; 
; Modinfo:
; 15/10/2022:		Added _putch, _getch
; 21/10/2022:		Added _puts
; 22/10/2022:		Added _waitvblank, _mos_f* file functions
; 26/11/2022:       __putch, changed default routine entries to use IY

	.include "mos_api.inc"

	XDEF _putch
	XDEF __putch
	XDEF _getch
	XDEF __getch
	XDEF _waitvblank
	XDEF _mos_fopen
	XDEF _mos_fclose
	XDEF _mos_fgetc
	XDEF _mos_fputc
	XDEF _mos_feof
	XDEF _getsysvar8bit
	XDEF _getsysvar16bit
	XDEF _getsysvar24bit

	segment CODE
	.assume ADL=1
	
_putch:
__putch:
	push 	iy
	ld 		iy,0
	add 	iy, sp
	ld 		a, (iy+6)
	rst.lil	10h
	ld		hl,0
	ld		l,a
	ld		sp,iy
	pop		iy
	ret

_getch:
__getch:
	push iy
	push ix
	ld a, mos_sysvars			; MOS Call for mos_sysvars
	rst.lil 08h					; returns pointer to sysvars in ixu
_getch0:
	ld a, (ix+sysvar_keycode)	; get current keycode
	or a,a
	jr z, _getch0				; wait for keypress
	
	push af						; debounce key, reload keycode with 0
	xor a
	ld (ix+sysvar_keycode),a
	pop af
	pop ix
	pop iy
	ret

_waitvblank:
	push iy
	ld a, mos_sysvars
	rst.lil 08h
	ld a, (iy + sysvar_time + 0)
$$:	cp a, (iy + sysvar_time + 0)
	jr z, $B
	pop iy
	ret


_getsysvar8bit:
	push iy
	ld a, mos_sysvars
	rst.lil 08h
	ld a, (iy)					; sysvars base address
	ld d,0
	ld e,a
	add iy,de					; iy now points to (mos_sysvars)+parameter
	ld a, (iy)
	pop iy
	ret

_getsysvar16bit:
_getsysvar24bit:
	push iy
	ld a, mos_sysvars
	rst.lil 08h
	ld a, (iy)					; sysvars base address
	ld d,0
	ld e,a
	add iy,de					; ix now points to (mos_sysvars)+parameter
	ld hl, (iy)
	pop iy
	ret

_mos_fopen:
	push iy
	ld iy,0
	add iy, sp
	
	ld hl, (iy+6)	; address to 0-terminated filename in memory
	ld c,  (iy+9)	; mode : fa_read / fa_write etc
	ld a, mos_fopen
	rst.lil 08h		; returns filehandle in A
	
	ld sp,iy
	pop iy
	ret	

_mos_fclose:
	push iy
	ld iy,0
	add iy, sp
	
	ld c, (iy+6)	; filehandle, or 0 to close all files
	ld a, mos_fclose
	rst.lil 08h		; returns number of files still open in A
	
	ld sp,iy
	pop iy
	ret	

_mos_fgetc:
	push iy
	ld iy,0
	add iy, sp
	
	ld c, (iy+6)	; filehandle
	ld a, mos_fgetc
	rst.lil 08h		; returns character in A
	
	ld sp,iy
	pop iy
	ret	

_mos_fputc:
	push iy
	ld iy,0
	add iy, sp
	
	ld c, (iy+6)	; filehandle
	ld b, (iy+9)	; character to write
	ld a, mos_fputc
	rst.lil 08h		; returns nothing
	
	ld sp,iy
	pop iy
	ret	

_mos_feof:
	push iy
	ld iy,0
	add iy, sp
	
	ld c, (iy+6)	; filehandle
	ld a, mos_feof
	rst.lil 08h		; returns A: 1 at End-of-File, 0 otherwise
	
	ld sp,iy
	pop iy
	ret	

end

	
