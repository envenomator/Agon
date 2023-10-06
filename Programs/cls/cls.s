; 
; Title:        cls.bin
; Author:       Jeroen Venema
; Last update:  06/10/2023
;
; Assembler: agon-ez80asm

    .assume adl=1
    .org $0b0000

    .include "mos_api.inc"
    .include "mos_init_minimum.inc"

_main:
    LD  A, 12   ; CLS VDU command
    RST.LIL 10h
    LD  HL, EXIT_OK
    RET