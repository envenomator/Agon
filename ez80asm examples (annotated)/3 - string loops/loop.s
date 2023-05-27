; 
; Title:        Interation loop, counting up, with annotations
;               Program prints the same string three times, but with different loop types
; Author:       Jeroen Venema
; Last update:  27/05/2023

; Assembler directives
    .assume adl=1   ; We start up in full 24bit mode, allowing full memory access and 24-bit wide registers
    .org $040000    ; This program assembles to the first mapped RAM location, start for user programs

; Skip the header
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
start:              ; We jump here with from 'jp start' earlier to do some actual work, after the header
    push af         ; Push all registers to the stack
    push bc
    push de
    push ix
    push iy

; Actual user code
;
; loop type 1 is printing out the next character in the string, and terminate after n characters
    ld b, 13h       ; we will print 13h / 19 decimal number of characters from the string. Cumbersome, but this is an example
    ld hl, string   ; Load the starting address of the string in HL
loop1:
    push bc         ; save our counter to stack
    push hl         ; save our start address to stack
    ld a, (hl)      ; load the A register with the content that HL points to
    rst.lil $10     ; Call the MOS API and output just one character
    pop hl          ; retrieve HL from the stack - observe the FIFO order here
    pop bc          ; retrieve BC from the stack - observe the FIFO order here
    inc hl          ; increase the address in HL, to point to the next character in the string
    dec b           ; decrease our number of characters to print, this will set the zero flag to 1/0
    jp nz, loop1    ; if b reached 0 previously, zero flag is set, and we drop out of this statement
                    ; if b didn't reach 0, zero flag isn't set, the 'NZ' condition is true and we jump back to the loop

; loop type 2 is the same as loop 1, but the ending uses a single command, that combines dec b / jp nz
    ld b, 13h       ; we will print 13h / 19 decimal number of characters from the string. Cumbersome, but this is an example
    ld hl, string   ; Load the starting address of the string in HL
loop2:
    push bc         ; save our counter to stack
    push hl         ; save our start address to stack
    ld a, (hl)      ; load the A register with the content that HL points to
    rst.lil $10     ; Call the MOS API and output just one character
    pop hl          ; retrieve HL from the stack - observe the FIFO order here
    pop bc          ; retrieve BC from the stack - observe the FIFO order here
    inc hl          ; increase the address in HL, to point to the next character in the string
    djnz loop2      ; voila. This works only with the B register

; loop type 2 actively checks for the presence of a '0' byte to terminate the string output
    ld hl, string   ; Load the starting address of the string in HL
loop3:
    ld a, (hl)      ; load the A register with the content that HL points to
    cp a, 0         ; Compare the A register with the terminator byte
    jp z, loop3_done; If a == 0, there is zero difference, the zero flag is set and the 'Z' condition drops us out
    push hl         ; save our start address to stack
    rst.lil $10     ; Call the MOS API and output just one (non-zero) character
    pop hl          ; retrieve our address from the stack
    inc hl          ; go to the next character address
    jr loop3        ; and go back to our loop
loop3_done:         ; if we end up here, our loop is done because we found the '0' character at the string end

; Cleanup stack, prepare for return to MOS
    pop iy          ; Pop all registers back from the stack
    pop ix
    pop de
    pop bc
    pop af
    ld hl,0         ; Load the MOS API return code (0) for no errors.
                    ; MOS expects this to be set to something meaningful - see API docs.
    ret             ; Return to the caller (MOS)

string:
    .db "Hello 01234567890\r\n",0 ; The string to output, delimited with a 0
        ;123456789abcdef01 2 3  4   byte count in this string