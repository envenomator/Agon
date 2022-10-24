;
; Title:		AGON MOS - MOS hexload assembly code
; Author:		Jeroen Venema
; Created:		23/10/2022
; Last Updated:	24/10/2022
;
; Modinfo:
; 23/10/2022:	Initial assembly code
; 24/10/2022:	0x00 escape code now contained in keymods, resulting in faster upload baudrate

			.ASSUME	ADL = 1
			
			DEFINE .STARTUP, SPACE = ROM
			SEGMENT .STARTUP
			
			XDEF	_getTransparentByte
			XDEF	_receive_bytestream
			
			XREF	_keycode
			XREF	_keymods
			
; receive a single keycode/keymods packet from the VDU
; keycode can't be 0x00 - so this is escaped with keymods code 0x01
; return transparent byte in a
_getTransparentByte:
	ld a,(_keycode)
	or a
	jr z, _getTransparentByte
	ld c,a
	xor a				; acknowledge receipt of key / debounce key
	ld (_keycode),a
	ld a,c

	cp a,01h			; check for escape code 0x01
	jr nz, getbyte_done ; no escape code received
	ld c,a				; save received byte
	ld a,(_keymods)		; is byte escaped with >0 in _keymods?
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
	push ix
	ld ix,0
	add ix, sp
	
	ld de, (ix+6)			; de == 24-bit address pointer
	ld hl, 0				; de will contain total number of bytes received
	
blockloop:
	call _getTransparentByte	; ask for number of bytes to receive
	or a
	jr z, rbdone

	ld b,a					; loop counter
$$:
	call _getTransparentByte	; receive each byte in a
	ld (de),a				; store byte in memory
	inc de					; next address
	inc hl					; increase total number of bytes read
	djnz $B					; next byte
	jp blockloop
	
rbdone:	
	ld sp,ix
	pop ix
	ret				; returns hl with number of bytes read