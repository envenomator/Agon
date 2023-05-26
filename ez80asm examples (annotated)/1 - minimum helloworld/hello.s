; 
; Title:        Minimum hello world example code, using ez80asm assembler, with annotations
; Author:       Jeroen Venema
; Last update:  20/05/2023

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
    ld hl, string   ; Load the address of the helloworld string in HL
    ld bc,0         ; Set length of stream in BC to 0, because the string is delimited 
    ld a,0          ; Set the delimiter character for the string to 0, very commonly used
    rst.lil $18     ; Call the MOS API using reset vector $18. The .LIL suffix is needed, 
                    ; because MOS is also called from non-ADL programs and is therefore 
                    ; coded in mixed ADL mode.

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
    .db "Hello, world!\r\n",0 ; The string to output with CR/LF, delimited with a 0
