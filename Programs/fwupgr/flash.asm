;
; Title:		Flash interface
; Author:		Jeroen Venema
; Created:		16/12/2022
; Last Updated:	16/12/2022
; 
; Modinfo:

	XDEF _enableFlashKeyRegister
	XDEF _lockFlashKeyRegister
	XDEF _writeflash
	XDEF _writerow
	XDEF _reset
	
	segment CODE
	.assume ADL=1

_enableFlashKeyRegister:
	push	ix
	ld		ix,0
	add		ix,sp
	
	; actual work here
	ld		a, b6h			; unlock
	out0	(F5h), a
	ld		a, 49h
	out0	(F5h), a
	
	ld		sp,ix
	pop		ix
	ret

_lockFlashKeyRegister:
	push	ix
	ld		ix,0
	add		ix,sp
	
	; actual work here
	ld		a, ffh			; lock
	out0	(F5h), a
	
	ld		sp,ix
	pop		ix
	ret
	
_reset:
	rst 0h
	ret	;will never get here
	
_writerow2:
	push bc
	push de
	push hl
	
	ld hl, 1c000h
	ld b, 128
	ld a, 1
loop:
	ld (hl), a		; 2 cycles
	inc hl			; 4 cycles
	dec b			; 1 cycle
	jr nz, loop		; 2/3 cycles
	

	pop hl
	pop de
	pop bc
	ret
	
_writerow:
	push bc
	push de
	push hl
	
	ld de, 1c000h
	ld hl, 60000h
	ld bc, 256
	ldir

	pop hl
	pop de
	pop bc
	ret

_writeflash:
	push	ix
	ld		ix,0
	add		ix,sp
		
	push bc
	push de
	push hl
	
	ld		de, (ix+6)	; destination address
	ld		hl, (ix+9)	; source
	ld		bc, (ix+12)	; number of bytes to write to flash
	ldir

	pop hl
	pop de
	pop bc

	ld		sp,ix
	pop		ix
	ret

	
end
