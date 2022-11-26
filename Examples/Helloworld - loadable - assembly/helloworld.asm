    .ASSUME ADL=1

    .org 40000h
    jp start

    .seek 40040h
    .DB		"MOS"		; Flag for MOS - to confirm this is a valid MOS command
    .DB		00h		; MOS header version 0
    .DB		01h		; Flag for run mode (0: Z80, 1: ADL)

start:
    ld hl,hello
loop:
    ld a,(hl)
    or a
    jr z, done
    rst.lis 10h
    inc hl
    jr loop
done:
    ld hl,0
    ret
hello:
    .db "Hello world!\r\n",0
