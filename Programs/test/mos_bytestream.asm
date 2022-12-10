;
; Title:		AGON MOS - MOS hexload assembly code
; Author:		Jeroen Venema
; Created:		23/10/2022
; Last Updated:	26/11/2022
;
; Modinfo:
; 23/10/2022:	Initial assembly code
; 24/10/2022:	0x00 escape code now contained in keymods, resulting in faster upload baudrate
; 26/11/2022:   Adapted as loadable command

	.include "mos_api.inc"
			
	SEGMENT CODE
	.ASSUME	ADL = 1
			
	XDEF	_receive_bytestream
	XDEF	_getTransparentByte
	
; receive a single keycode/keymods packet from the VDU
; keycode can't be 0x00 - so this is escaped with keymods code 0x01
; return transparent byte in a
_getTransparentByte:
	push iy
	push ix

	call getSysvars
	call getTransparentByte
	
	pop ix
	pop iy
	ret

; get pointer to sysvars in ixu
getSysvars:
	ld a, mos_sysvars		; MOS call for mos_sysvars
	rst.lil 08h				; returns pointer to sysvars in ixu
	ret

; call here when ixu has been set previously for performance
getTransparentByte:
	ld a, (ix+sysvar_keycode)
	or a
	jr z, getTransparentByte
	ld c,a
	xor a				; acknowledge receipt of key / debounce key
	ld (ix+sysvar_keycode),a
	ld a,c

	cp a,01h			; check for escape code 0x01
	jr nz, getbyte_done ; no escape code received
	ld c,a				; save received byte
	ld a,(ix+sysvar_keymods)		; is byte escaped with >0 in _keymods?
	or a
	jr z, getbyte_nesc  ; not escaped: normal transmission of 0x01
	ld c,0				; escaped 0x01 => 0x00 meant
getbyte_nesc:
	ld a,c				; restore to a
getbyte_done:
	ret

; argument: 24-bit address pointer
; accepts n>=0 number of blocks from the VDU
; each block is received as follows:
; 0-255 (byte) number of bytes in this block - if this is zero, terminate procedure
; number of packets from VDU, containing both keycode/keymods.

_receive_bytestream:
	push iy
	ld iy,0
	add iy, sp
	
	ld de, (iy+6)			; de == 24-bit address pointer
	ld hl, 0				; de will contain total number of bytes received
	
	push ix					; safeguard main ixu
	
	call getSysvars			; returns pointer to sysvars in ixu
blockloop:
	call getTransparentByte	; ask for number of bytes to receive
	or a
	jr z, rbdone

	ld b,a					; loop counter
$$:
	call getTransparentByte	; receive each byte in a
	ld (de),a				; store byte in memory
	inc de					; next address
	inc hl					; increase total number of bytes read
	djnz $B					; next byte
	jp blockloop
	
rbdone:
	pop ix
	ld sp,iy
	pop iy
	ret				; returns hl with number of bytes read
	
	
	SEGMENT DATA

; Storage for pointer to sysvars

sysvarptr:		DS 3
			
	END
