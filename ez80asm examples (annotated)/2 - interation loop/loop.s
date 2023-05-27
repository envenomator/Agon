; 
; Title:        Interation loop, counting up, with annotations
;               Program prints a number of lines, ending with the number of the loop
;               Purpose is to show several things:
;               1- How to construct and end a loop
;               2- How to set a label as a number using the .equ directive
;               3- How to use the stack as temporary storage 
; Author:       Jeroen Venema
; Last update:  27/05/2023

ENDLOOP: .equ 9     ; A label that EQUALS a number with the .equ directive

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
; Prepare parameters for an API call to MOS and output a stream of characters
;
    ld d,0          ; We start at iteration 0 and use the 'd' register for counting up
loop:
    ld hl, string   ; Load the address of the 'base' string in HL
    ld bc,0         ; Set length of stream in BC to 0, because the string is delimited 
    ld a,0          ; Set the delimiter character for the string to 0, very commonly used

    push de         ; before we enter the API call, we need to save our counter, together in 'de' on the stack
                    ; or else the API will likely overwrite it during the routine
    rst.lil $18     ; Call the MOS API using reset vector $18. The .LIL suffix is needed, 
                    ; because MOS is also called from non-ADL programs and is therefore 
                    ; coded in mixed ADL mode.
    pop de          ; We temporary need our counter back in 'd'

    push de         ; We save it again to the stack, but the value remains in 'd'
    ld a, d         ; We transfer the value of our iteration counter to 'a' where we can add to it
    add a, 30h      ; We add the ASCII value of the character '0' to it
                    ; This is a hack and will obviously not work beyond the '9', but it is fine for this example
    rst.lil $10     ; And next call the MOS API that outputs this character (in 'a') to the VDP
                    ; Because we did another call to MOS, we needed to save the 'd' register to stack first
    ld hl, crlf     ; We load the address of the 'end of string' string in HL
    ld bc,0         ; We set the length of the stream in BC to 0
    ld a,0          ; Set the delimiter character for the string to 0
    rst.lil $18     ; And call the MOS API to do a NEXT line on screen

    pop de          ; And now we can return our counter from the stack, all API calls done in this loop

    inc d           ; We increase the counter by 1
    ld a, ENDLOOP   ; We load the ENDLOOP label to the A register, where we can do a COMPARE statement to it
                    ; This is a convoluted example because the iteration counts up. Counting down is easier in assembly
                    ; because the DEC command for example immediately sets the zero flag in F.
    cp a, d         ; Have we reached our intended loop end?
    jp nz, loop     ; if not, we will need to do another loop

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
    .db "Iteration ",0 ; The string to output, delimited with a 0
crlf:
    .db "\r\n",0    ; crlf string, delimited with a 0
