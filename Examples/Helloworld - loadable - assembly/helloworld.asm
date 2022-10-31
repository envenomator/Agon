    .ASSUME ADL=1

    .org 40000h
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
    jr done
hello:
    .db "Hello world!",0
