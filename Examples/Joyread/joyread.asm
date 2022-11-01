; Title:    SNES Joystick button reader
; Author:   Jeroen Venema
; Created:  01/11/2022
;
; Prints out a string of 16 characters, each character representing a button of a SNES controller
; Connections to the SNES controller through level-shifters:
; CLOCK - PC0
; DATA  - PC1
; LATCH - PC2
;
; Assembled using spasm-ng with 'spasm -E -T joyread.asm'

    .ASSUME ADL=1
    .org 40000h

start:
    call _initjoystickpins
displayread:
    call _joyread
    
    ld b, 16        ; 16 characters to screen, 12 button bits and 4 non-used bits
loop:
    add hl,hl       ; shift hl right one bit, resulting bit in carry
    ld a, 0
    adc a, 30h      ; 30+NC = 30 ('0'), 30+C = 31 ('1')
    rst.lis 10h     ; output to MOS
    ld a,b
    cp 1            ; last button to print?
    jr z, creturn
    dec b
    jr loop
creturn:
    ld a, '\r'
    rst.lis 10h     ; return to beginning of the line

    call wait10ms   ; wait before next poll to the controller, fast enough for demo

    jp displayread

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
_joyread:
    ld a,%00000101
    out0 (158),a    ; latch high, clock stays high
    call wait12us

    ld a,%00000001
    out0 (158),a    ; latch low, clock stays high
    call wait6us

    ld b,16         ; read 16 bit
    ld hl,0         ; zero out result register HL

joyloop:
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
    djnz joyloop
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
