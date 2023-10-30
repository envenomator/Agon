; Title:    SNES Joystick button reader
; Author:   Jeroen Venema
; Created:  01/11/2022
; Updated:  04/06/2023 - ez80asm

; Prints out a string of 16 characters, each character representing a button of a SNES controller
; Connections to the SNES controller through level-shifters:
; CLOCK - PC0
; DATA  - PC1
; LATCH - PC2
;
    .assume adl=1
    .org $040000

    jp start        ; We jump over the header, to make sure the header stays at the same location
                    ; This leaves <some> space between the jump and the header to use for data, but not much.
                    ; In that case, be careful that the header doesn't start at the next alignment and the program 
                    ; isn't recognized by MOS as a valid executable. 

; Quark MOS header 
    .align 64       ; Quark MOS expects programs that it LOADs,to have a specific signature
                    ; Starting from decimal 64 onwards
    .db "MOS"       ; MOS header 'magic' characters
    .db 00h         ; MOS header version 0 - the only in existence currently afaik
    .db 01h         ; Flag for run mode (0: Z80, 1: ADL) - We start up in ADL mode here

; Code
start:
    ld hl, welcome
    ld bc, 0
    ld a, 0
    rst.lil 18h         ; print welcome message
    call _initjoystickpins

showbuttons:
    call joyread   ; Joystick pins returned as bits in HL

    ld a, h
    call print8bit
    ld a, l
    call print8bit

    ld a, '\r'
    rst.lil 10h
    
    call wait10ms
    jr showbuttons

; print an 8bit value in A as 0s and 1s to VDP
print8bit:
    push hl
    push bc
    ld b,8    ; loop counter
    ld c,a    ; storage of input
_print8bit_loop:
    sla c     ; left shift one bit, to carry
    ld a, '0'
    adc a, 0  ; if carry, A == '1', else A == '0'
    push hl
    push bc
    rst.lil 10h
    pop bc
    pop hl
    ld a, b
    cp a, 1
    jr z, _print8bit_done
    dec b
    jr _print8bit_loop
_print8bit_done:
    pop bc
    pop hl
    ret

_initjoystickpins:
    ld a,%11111000  ; latch/bit2 and clk/bit0 as output, data temp output, needs to stay high
    out0 (159),a    ; PC_DDR configures input/output
    ld a,0          ; default to ALT1/ALT2
    out0 (160),a
    out0 (161),a

    ld a,%00000011  ; latch/bit2 normally low, data/bit1 and clock/bit0 normally high
    out0 (158),a    ; write to port C

    ld a,%11111010  ; latch/bit2 and clk/bit0 as output, data as input now
    out0 (159),a    ; reconfigure mode - data as input

    ld a,%00000011
    out0 (158),a
    ret

; Joyread function
;
; returns 16bit SNES code in HL
; bit layout:
; bit  0 - B
; bit  1 - Y
; bit  2 - Select
; bit  3 - Start
; bit  4 - Up
; bit  5 - Down
; bit  6 - Left
; bit  7 - Right
; bit  8 - A
; bit  9 - X
; bit 10 - L
; bit 11 - R
; bit 12 - none - always high
; bit 13 - none - always high
; bit 14 - none - always high
; bit 15 - none - always high
;
; Non-pressed buttons return 1
; Pressed buttons return a 0
;
joyread:
    ld a,%00000101
    out0 (158),a    ; latch high, clock stays high
    call wait12us

    ld a,%00000001
    out0 (158),a    ; latch low, clock stays high
    call wait6us

    ld b,16         ; read 16 bit
    ld hl,0         ; zero out result register HL

_joyloop:
    push bc
    srl h           ; shift HL right one bit, first run shifts only zeroes
    rr l
    ld a,%00000000  ; clock & latch low
    out0 (158),a
    call wait6us
    in0 a,(158)     ; read all bits from port C
    and a, %00000010   ; mask out DATA bit
    jr z, checkdone ; 0 received, do nothing
    ld a,h
    or %10000000    ; 1 received, set highest bit
    ld h,a
checkdone:
    ld a,%00000001  ; clock high & latch low
    out0 (158),a
    call wait6us
    pop bc
    djnz _joyloop
    ret

; wait 6us at 18,432Mhz
wait6us:
    ld b,25
wait6usloop:
    djnz wait6usloop
    ret

; wait 12us at 18,432Mhz
wait12us:
    ld b,50
wait12usloop:
    djnz wait12usloop
    ret

; wait 10ms at 18,432Mhz
wait10ms:
    ld de, 25000
wait10msloop:
    dec de
    ld a,d
    or e
    jr nz,wait10msloop
    ret

welcome:
    .db 12, "Button layout:\r\n\r\nbit  0 - B\r\nbit  1 - Y\r\nbit  2 - Select\r\nbit  3 - Start\r\nbit  4 - Up\r\n"
    .db "bit  5 - Down\r\nbit  6 - Left\r\nbit  7 - Right\r\nbit  8 - A\r\n"
    .db "bit  9 - X\r\nbit 10 - L\r\nbit 11 - R\r\nbit 12 - none - always high\r\n"
    .db "bit 13 - none - always high\r\nbit 14 - none - always high\r\nbit 15 - none - always high\r\n"
    .db "\r\nJoystick status bits:\r\n",0