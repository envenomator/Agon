    .org 40000h
    .include "header.inc"

start:
    ld b, 10

loop:
    dec b
    jp nz, loop

end:
    ld hl,0
    ret
