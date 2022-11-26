;*****************************************************************************
; cstartup.asm
;
; ZDS II C Runtime Startup for the eZ80 and eZ80Acclaim! C Compiler
;*****************************************************************************
; Copyright (C) 2005 by ZiLOG, Inc.  All Rights Reserved.
;*****************************************************************************
; Mofified to create loadable code with bare minimum C startup code


        XDEF _errno

		XREF _main
        XREF __low_bss         ; Low address of bss segment
        XREF __len_bss         ; Length of bss segment
        XREF __low_data        ; Address of initialized data section
        XREF __low_romdata     ; Addr of initialized data section in ROM
        XREF __len_data        ; Length of initialized data section


;*****************************************************************************
; Reset code
        DEFINE .RESET, SPACE = ROM
        SEGMENT .RESET
		.ASSUME ADL=1
_reset:
_rst0:
	push af
	push bc
	push de
	push hl
	
    jp.lil __c_startup

		ALIGN		64
		
		DB		"MOS"		; Flag for MOS - to confirm this is a valid MOS command
		DB		00h		; MOS header version 0
		DB		00h		; Flag for run mode (0: Z80, 1: ADL)

;*****************************************************************************
; This segment must be aligned on a 512 byte boundary anywhere in RAM
; Each entry will be a 3-byte address in a 4-byte space 
        DEFINE .IVECTS, SPACE = ROM, ALIGN = 200h
        SEGMENT .IVECTS
		.ASSUME ADL=1

;*****************************************************************************
; Startup code
        DEFINE .STARTUP, SPACE = ROM
        SEGMENT .STARTUP
        .ASSUME ADL=1

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Initializes the C environment
__c_startup:	
    ;--------------------------------------------------
    ; Clear the uninitialized data section
    ld bc, __len_bss           ; Check for non-zero length
    ld a, __len_bss >> 16
    or a, c
    or a, b
    jr z, _c_bss_done          ; BSS is zero-length ...
    xor a, a
    ld (__low_bss), a
    sbc hl, hl                 ; hl = 0
    dec bc                     ; 1st byte's taken care of
    sbc hl, bc
    jr z, _c_bss_done          ; Just 1 byte ...
    ld hl, __low_bss           ; reset hl
    ld de, __low_bss + 1       ; [de] = bss + 1
    ldir                       ; Clear this section
_c_bss_done:
    ;--------------------------------------------------
    ; Copy the initialized data section
    ld bc, __len_data          ; [bc] = data length
    ld a, __len_data >> 16     ; Check for non-zero length
    or a, c
    or a, b
    jr z, _c_data_done         ; __len_data is zero-length ...
    ld hl, __low_romdata       ; [hl] = data_copy
    ld de, __low_data          ; [de] = data
    ldir                       ; Copy the data section
_c_data_done:
    ; C environment created, continue with the initialization process

    ;---------------------------------------------
    ; prepare to go to the main system rountine
    ld hl, 0                   ; hl = NULL
    push hl                    ; argv[0] = NULL
    ld ix, 0
    add ix, sp                 ; ix = &argv[0]
    push ix                    ; &argv[0]
    pop hl
    ld de, 0                   ; argc = 0
    call _main                 ; int main(int argc, char *argv[]))
    pop de                     ; clean the stack

__exit:
_exit:
_abort:
	
	pop hl
	pop de
	pop bc
	pop af
	ld hl, 0
	ret
;	rst.lis	00h			; Reset MOS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;*****************************************************************************
; Define global system var _errno. Used by floating point libraries
        SEGMENT DATA
_errno:
        DS 3                   ; extern int _errno
		
        END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
_SysClkFreq:
        DL _SYS_CLK_FREQ


        XREF _SYS_CLK_FREQ
        XDEF _SysClkFreq

        END
