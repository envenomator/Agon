    .org 40000h
    .include "header.inc"

start:
    ld b, 10   ; load value 10 decimal to register 'b'

loop:
    dec b      ; decrement register 'b' by 1
    jp nz, loop ; jump to 'loop' if decrement reduced 'b' to 0

end:
    ld hl, 0   ; load return value 0
    ret        ; return to Quark MOS
