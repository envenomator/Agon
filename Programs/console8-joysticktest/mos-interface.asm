;
; Title:		AGON MOS - MOS assembly interface
; Author:		Jeroen Venema
; Created:		15/10/2022
; Last Updated:	30/10/2023
; 
; Modinfo:
; 15/10/2022:		Added _putch, _getch
; 21/10/2022:		Added _puts
; 22/10/2022:		Added _waitvblank, _mos_f* file functions
; 26/11/2022:       __putch, changed default routine entries to use IY
; 10/01/2023:		Added _getsysvar_cursorX/Y and _getsysvar_scrchar
; 23/02/2023:		Added _mos_save and _mos_del, also changed stackframe to use ix exclusively
; 16/04/2023:		Added _mos_fread, _mos_fwrite and _mos_flseek
; 18/04/2023:		_mos_flseek fix
; 19/04/2023:		_mos_getfil added
; 30/10/2023:		removed all sysvar functions -> struct sysvar_t created in mos-interface.h

	.include "mos_api.inc"

	XDEF _putch
	XDEF __putch
	XDEF _getch
	XDEF _waitvblank
	XDEF _mos_puts
	XDEF _mos_load
	XDEF _mos_save
	XDEF _mos_cd
	XDEF _mos_dir
	XDEF _mos_del
	XDEF _mos_ren
	XDEF _mos_copy
	XDEF _mos_mkdir
	XDEF _mos_sysvars
	XDEF _mos_editline
	XDEF _mos_fopen
	XDEF _mos_fclose
	XDEF _mos_fgetc
	XDEF _mos_fputc
	XDEF _mos_feof
	XDEF _mos_getError
	XDEF _mos_oscli
	XDEF _mos_getrtc
	XDEF _mos_setrtc
	XDEF _mos_setintvector
	XDEF _mos_uopen
	XDEF _mos_uclose
	XDEF _mos_ugetc
	XDEF _mos_uputc
	XDEF _mos_fread
	XDEF _mos_fwrite
	XDEF _mos_flseek
	XDEF _mos_getfil

	XDEF _getsysvars
	
	segment CODE
	.assume ADL=1
	
_putch:
__putch:
	push 	ix
	ld 		ix,0
	add 	ix, sp
	ld 		a, (ix+6)
	rst.lil	10h
	ld		hl,0
	ld		l,a
	ld		sp,ix
	pop		ix
	ret

_mos_puts:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl,	(ix+6)	; Address of buffer
	ld		bc, (ix+9)	; Size to write from buffer - or 0 if using delimiter
	ld		a,	(ix+12) ; delimiter - only if size is 0
	rst.lil	18h			; Write a block of bytes out to the ESP32
	ld		sp,ix
	pop		ix
	ret

_getch:
	push	ix
	ld		a, mos_getkey	; Read a keypress from the VDP
	rst.lil	08h
	pop		ix
	ret

_waitvblank:
	push	ix
	ld		a, mos_sysvars
	rst.lil	08h
	ld.lil	a, (ix + sysvar_time + 0)
$$:	cp.lil	a, (ix + sysvar_time + 0)
	jr		z, $B
	pop		ix
	ret

_getsysvars:
	push	ix
	ld		a, mos_sysvars
	rst.lil	08h
	push	ix
	pop		hl
	pop		ix
	ret

_mos_load:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; address of filename (zero terminated)
	ld		de, (ix+9)	; address at which to load
	ld		bc, (ix+12)	; maximum allowed size
	ld a,	mos_load
	rst.lil	08h			; Load a file from SD card
	ld		sp,ix
	pop		ix
	ret

_mos_save:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; address of filename (zero terminated)
	ld		de, (ix+9)	; address to save from
	ld		bc, (ix+12)	; number of bytes to save
	ld a,	mos_save
	rst.lil	08h			; save file to SD card
	ld		sp,ix
	pop		ix
	ret

_mos_cd:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; address of pathname (zero terminated)
	ld a,	mos_cd
	rst.lil	08h			; Change current directory on the SD card
	ld		sp,ix
	pop		ix
	ret

_mos_dir:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; address of pathname (zero terminated)
	ld a,	mos_dir
	rst.lil	08h			; List SD card folder contents
	ld		sp,ix
	pop		ix
	ret

_mos_del:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Address of path (zero terminated)
	ld a,	mos_del
	rst.lil	08h			; Delete a file or folder from the SD card
	ld		sp,ix
	pop		ix
	ret

_mos_ren:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Address of filename1 (zero terminated)
	ld		de, (ix+9)	; Address of filename2 (zero terminated)
	ld a,	mos_ren
	rst.lil	08h			; Rename a file on the SD card
	ld		sp,ix
	pop		ix
	ret

_mos_copy:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Address of filename1 (zero terminated)
	ld		de, (ix+9)	; Address of filename2 (zero terminated)
	ld a,	mos_copy
	rst.lil	08h			; Copy a file on the SD card
	ld		sp,ix
	pop		ix
	ret

_mos_mkdir:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Address of path (zero terminated)
	ld a,	mos_mkdir
	rst.lil	08h			; Make a folder on the SD card
	ld		sp,ix
	pop		ix
	ret

_mos_sysvars:
	push	ix
	ld a,	mos_sysvars
	rst.lil	08h			; Fetch a pointer to the system variables
	ld 		hl, ix
	pop		ix
	ret

_mos_editline:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Address of the buffer
	ld		bc, (ix+9)	; Buffer length
	ld      e,  (ix+12) ; 0 to not clear buffer, 1 to clear
	ld a,	mos_editline
	rst.lil	08h			; Invoke the line editor
	ld		sp,ix
	pop		ix
	ret

_mos_fopen:
	push	ix
	ld		ix,0
	add		ix, sp
	ld		hl, (ix+6)	; address to 0-terminated filename in memory
	ld		c,  (ix+9)	; mode : fa_read / fa_write etc
	ld		a, mos_fopen
	rst.lil	08h		; returns filehandle in A
	ld		sp,ix
	pop		ix
	ret	

_mos_fclose:
	push	ix
	ld		ix,0
	add		ix, sp
	ld		c, (ix+6)	; filehandle, or 0 to close all files
	ld		a, mos_fclose
	rst.lil	08h		; returns number of files still open in A
	ld		sp,ix
	pop		ix
	ret	

_mos_fgetc:
	push	ix
	ld		ix,0
	add		ix, sp
	ld		c, (ix+6)	; filehandle
	ld		a, mos_fgetc
	rst.lil	08h		; returns character in A
	ld		sp,ix
	pop		ix
	ret	

_mos_fputc:
	push	ix
	ld		ix,0
	add		ix, sp
	ld		c, (ix+6)	; filehandle
	ld		b, (ix+9)	; character to write
	ld		a, mos_fputc
	rst.lil	08h		; returns nothing
	ld		sp,ix
	pop		ix
	ret	

_mos_feof:
	push	ix
	ld		ix,0
	add		ix, sp
	ld		c, (ix+6)	; filehandle
	ld		a, mos_feof
	rst.lil	08h		; returns A: 1 at End-of-File, 0 otherwise
	ld		sp,ix
	pop		ix
	ret	

_mos_getError:
	push	ix
	ld		ix,0
	add		ix, sp
	ld		e, (ix+6)	; The error code
	ld		hl,(ix+7)	; Address of buffer to copy message into
	ld		bc,(ix+10)  ; Size of buffer
	ld		a, mos_getError
	rst.lil	08h			; Copy an error string to a buffer
	ld		sp,ix
	pop		ix
	ret	

_mos_oscli:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Pointer the the MOS command string
	ld		bc, (ix+9)	; Pointer to additional command structure
	ld      de, (ix+12) ; Number of additional commands
	ld a,	mos_oscli
	rst.lil	08h			; Execute a MOS command
	ld		sp,ix
	pop		ix
	ret

_mos_getrtc:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Pointer to a buffer to copy the string to (at least 32 bytes)
	ld a,	mos_getrtc
	rst.lil	08h			; Get a time string from the RTC (Requires MOS 1.03 or above)
	ld		sp,ix
	pop		ix
	ret

_mos_setrtc:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Pointer to a 6-byte buffer with the time data
	ld a,	mos_setrtc
	rst.lil	08h			; Set the RTC (Requires MOS 1.03 or above)
	ld		sp,ix
	pop		ix
	ret

_mos_setintvector:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		de,	(ix+6)	; Interrupt vector number to set
	ld		hl, (ix+9)	; Address of new interrupt vector
	ld a,	mos_setintvector
	rst.lil	08h			; Set an interrupt vector (Requires MOS 1.03 or above)
	ld		sp,ix
	pop		ix
	ret

_mos_uopen:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		hl, (ix+6)	; Address of uart1 settings structure
	push	ix
	ld		ix, hl
	ld		a, mos_uopen
	rst.lil 08h
	pop		ix
	ld		sp,ix
	pop		ix
	ret

_mos_uclose:
	push	ix
	ld		a, mos_uclose
	rst.lil	08h
	pop		ix
	ret

_mos_ugetc:
	push	ix
	ld		hl, 0
	ld		a, mos_ugetc	; Read a byte from UART1
	rst.lil	08h
	ld		l, a
	jr		nc, $F
	ld		h, 1h			; error, return >255 in hl	
$$:
	pop		ix
	ret

_mos_uputc:
	push	ix
	ld		c, a
	ld		a, mos_uputc
	rst.lil	08h
	ld		a, 1h
	jr		nc, $F
	xor		a,a				; error condition, return 0
$$:
	pop		ix
	ret

_mos_fread:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		bc, (ix+6)	; file handle
	ld		hl, (ix+9)	; buffer address
	ld		de, (ix+12)	; number of bytes to read
	ld a,	mos_fread
	rst.lil	08h
	ld		hl, de		; number of bytes read
	ld		sp,ix
	pop		ix
	ret

_mos_fwrite:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		bc, (ix+6)	; file handle
	ld		hl, (ix+9)	; buffer address
	ld		de, (ix+12)	; number of bytes to write
	ld a,	mos_fwrite
	rst.lil	08h
	ld		hl, de		; number of bytes written
	ld		sp,ix
	pop		ix
	ret

_mos_flseek:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		bc, (ix+6)	; file handle
	ld		de, 0
	ld		hl, (ix+9)  ; 24 least significant bits
	ld		e,  (ix+12)	; 8 most most significant bits
	ld a,	mos_flseek
	rst.lil	08h
	ld		sp,ix
	pop		ix
	ret

_mos_getfil:
	push	ix
	ld 		ix,0
	add 	ix, sp
	ld 		bc, (ix+6)	; File identifier
	ld a,	mos_getfil
	rst.lil	08h			; Get a pointer to the relevant FIL struct
	ld		sp,ix
	pop		ix
	ret

end
