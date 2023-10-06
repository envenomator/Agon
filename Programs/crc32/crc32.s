; 
; Title:        crc32.bin
; Author:       Jeroen Venema
; Last update:  05/10/2023

	.ASSUME ADL=1
	.ORG $0B0000

	.include "mos_api.inc"
	.include "mos_init.s"
	.include "functions.s"

BUFFERSIZE:	EQU 1024

_main:
	; Check for a single parameter
	LD		A, C
	CP		2
	JR		Z, openfile
	LD		HL, usage
	CALL	printString
	LD		HL, EXIT_INVALIDPARAMETER
	RET

openfile:
    ; open file
    LD	HL, (IX+3)
    LD	C, fa_read
    LD	A, mos_fopen
    RST.LIL 08h
    LD	(filehandle), A
    OR	A
    JR	NZ, getfilesize
    LD	HL, EXIT_FILENOTFOUND
    RET

getfilesize:
    ; get pointer to FIL structure in MOS memory
    LD	A, (filehandle)
    LD	C, A
    LD	A, mos_getfil
    RST.LIL 08h

    LD	DE, fil_obj + ffobjid_objsize	; offset to lower 3bytes in FSIZE_t, part of the FFOBJD struct that HL points to
    ADD	HL, DE
    LD	HL, (HL)     ; load actual FSIZE_t value (lower 3 bytes)
    LD	(filesize),HL

    CALL	crc32_initialize

senddata:
    LD	A, (filehandle)
    LD	C, A
    LD	HL, buffer
    LD	DE, BUFFERSIZE
    LD	A, mos_fread
    RST.LIL 08h
    LD	A, D
    OR	E	; 0 bytes read?
    JR	Z, printchecksum

    PUSH	DE
    POP		HL          ; HL contains size for crc32
    LD		DE, buffer   ; DE contains pointer to buffer for crc32
    CALL	crc32

    JR	senddata ; next buffer read from disk

printchecksum:
    CALL	DisplayHexPrefix
    CALL	crc32_finalize

    LD	A, (crc32result+3)
    LD	HL, 0
    LD	L, A
    CALL	printHex8

    LD	A, (crc32result+2)
    LD	HL, 0
    LD	L, A
    CALL	printHex8

    LD	A, (crc32result+1)
    LD	HL, 0
    LD	L, A
    CALL	printHex8

    LD	A, (crc32result)
    LD	HL, 0
    LD	L, A
    CALL	printHex8

	CALL	printNewline

closefile:
    ; close file
    LD	A, (filehandle)
    LD	C, A
    LD	A, mos_fclose
    RST.LIL 08h

endprogram:
    LD HL,0
    RET

crc32_initialize:
    ; Initialise output to 0xFFFFFFFF
    EXX
    LD  A, FFh
    LD  (crc32result+3), A
    LD  (crc32result+2), A
    LD  (crc32result+1), A
    LD  (crc32result), A
    EXX
    RET

; expects DE to point to buffer to check
; expects HL to contain the bytecount
; returns 32-bit value in E:HLU:HL
crc32:        
    LD		BC,1

    EXX
    LD A, (crc32result+3)
    LD D, A
    LD A, (crc32result+2)
    LD E, A
    LD A, (crc32result+1)
    LD B, A
    LD A, (crc32result)
    LD C, A
    EXX

@@:	
    LD	A,(DE)
    INC	DE
    
    ; Calculate offset into lookup table
    EXX
    XOR	C
    LD	HL,crc32_lookup_table >> 2
    LD	L,A
    ADD	HL,HL
    ADD	HL,HL

    LD	A,B
    XOR	(HL)
    INC	HL
    LD	C,A

    LD	A,E
    XOR	(HL)
    INC	HL
    LD	B,A

    LD	A,D
    XOR	(HL)
    INC	HL
    LD	E,A

    LD	D,(HL)
    EXX

    ; Decrement count and loop if not zero
    OR	A,A
    SBC	HL,BC
    JR	NZ,@b

    EXX
    LD  A, D
    LD  (crc32result+3), A
    LD  A, E
    LD  (crc32result+2), A
    LD  A, B
    LD  (crc32result+1), A
    LD  A, C
    LD  (crc32result), A
    EXX
    RET

crc32_finalize:
    LD  A, (crc32result+3)
    CPL
    LD  (crc32result+3), A
    LD  A, (crc32result+2)
    CPL
    LD  (crc32result+2), A
    LD  A, (crc32result+1)
    CPL
    LD  (crc32result+1), A
    LD  A, (crc32result)
    CPL
    LD  (crc32result), A
	RET

; The crc32 routine is optimised in such a way as to require
; the following table to be aligned on a 1024 byte boundary.
	.ALIGN	1024
crc32_lookup_table:
	DW32	$00000000, $77073096, $ee0e612c, $990951ba
	DW32	$076dc419, $706af48f, $e963a535, $9e6495a3
	DW32	$0edb8832, $79dcb8a4, $e0d5e91e, $97d2d988
	DW32	$09b64c2b, $7eb17cbd, $e7b82d07, $90bf1d91
	DW32	$1db71064, $6ab020f2, $f3b97148, $84be41de
	DW32	$1adad47d, $6ddde4eb, $f4d4b551, $83d385c7
	DW32	$136c9856, $646ba8c0, $fd62f97a, $8a65c9ec
	DW32	$14015c4f, $63066cd9, $fa0f3d63, $8d080df5
	DW32	$3b6e20c8, $4c69105e, $d56041e4, $a2677172
	DW32	$3c03e4d1, $4b04d447, $d20d85fd, $a50ab56b
	DW32	$35b5a8fa, $42b2986c, $dbbbc9d6, $acbcf940
	DW32	$32d86ce3, $45df5c75, $dcd60dcf, $abd13d59
	DW32	$26d930ac, $51de003a, $c8d75180, $bfd06116
	DW32	$21b4f4b5, $56b3c423, $cfba9599, $b8bda50f
	DW32	$2802b89e, $5f058808, $c60cd9b2, $b10be924
	DW32	$2f6f7c87, $58684c11, $c1611dab, $b6662d3d
	DW32	$76dc4190, $01db7106, $98d220bc, $efd5102a
	DW32	$71b18589, $06b6b51f, $9fbfe4a5, $e8b8d433
	DW32	$7807c9a2, $0f00f934, $9609a88e, $e10e9818
	DW32	$7f6a0dbb, $086d3d2d, $91646c97, $e6635c01
	DW32	$6b6b51f4, $1c6c6162, $856530d8, $f262004e
	DW32	$6c0695ed, $1b01a57b, $8208f4c1, $f50fc457
	DW32	$65b0d9c6, $12b7e950, $8bbeb8ea, $fcb9887c
	DW32	$62dd1ddf, $15da2d49, $8cd37cf3, $fbd44c65
	DW32	$4db26158, $3ab551ce, $a3bc0074, $d4bb30e2
	DW32	$4adfa541, $3dd895d7, $a4d1c46d, $d3d6f4fb
	DW32	$4369e96a, $346ed9fc, $ad678846, $da60b8d0
	DW32	$44042d73, $33031de5, $aa0a4c5f, $dd0d7cc9
	DW32	$5005713c, $270241aa, $be0b1010, $c90c2086
	DW32	$5768b525, $206f85b3, $b966d409, $ce61e49f
	DW32	$5edef90e, $29d9c998, $b0d09822, $c7d7a8b4
	DW32	$59b33d17, $2eb40d81, $b7bd5c3b, $c0ba6cad
	DW32	$edb88320, $9abfb3b6, $03b6e20c, $74b1d29a
	DW32	$ead54739, $9dd277af, $04db2615, $73dc1683
	DW32	$e3630b12, $94643b84, $0d6d6a3e, $7a6a5aa8
	DW32	$e40ecf0b, $9309ff9d, $0a00ae27, $7d079eb1
	DW32	$f00f9344, $8708a3d2, $1e01f268, $6906c2fe
	DW32	$f762575d, $806567cb, $196c3671, $6e6b06e7
	DW32	$fed41b76, $89d32be0, $10da7a5a, $67dd4acc
	DW32	$f9b9df6f, $8ebeeff9, $17b7be43, $60b08ed5
	DW32	$d6d6a3e8, $a1d1937e, $38d8c2c4, $4fdff252
	DW32	$d1bb67f1, $a6bc5767, $3fb506dd, $48b2364b
	DW32	$d80d2bda, $af0a1b4c, $36034af6, $41047a60
	DW32	$df60efc3, $a867df55, $316e8eef, $4669be79
	DW32	$cb61b38c, $bc66831a, $256fd2a0, $5268e236
	DW32	$cc0c7795, $bb0b4703, $220216b9, $5505262f
	DW32	$c5ba3bbe, $b2bd0b28, $2bb45a92, $5cb36a04
	DW32	$c2d7ffa7, $b5d0cf31, $2cd99e8b, $5bdeae1d
	DW32	$9b64c2b0, $ec63f226, $756aa39c, $026d930a
	DW32	$9c0906a9, $eb0e363f, $72076785, $05005713
	DW32	$95bf4a82, $e2b87a14, $7bb12bae, $0cb61b38
	DW32	$92d28e9b, $e5d5be0d, $7cdcefb7, $0bdbdf21
	DW32	$86d3d2d4, $f1d4e242, $68ddb3f8, $1fda836e
	DW32	$81be16cd, $f6b9265b, $6fb077e1, $18b74777
	DW32	$88085ae6, $ff0f6a70, $66063bca, $11010b5c
	DW32	$8f659eff, $f862ae69, $616bffd3, $166ccf45
	DW32	$a00ae278, $d70dd2ee, $4e048354, $3903b3c2
	DW32	$a7672661, $d06016f7, $4969474d, $3e6e77db
	DW32	$aed16a4a, $d9d65adc, $40df0b66, $37d83bf0
	DW32	$a9bcae53, $debb9ec5, $47b2cf7f, $30b5ffe9
	DW32	$bdbdf21c, $cabac28a, $53b39330, $24b4a3a6
	DW32	$bad03605, $cdd70693, $54de5729, $23d967bf
	DW32	$b3667a2e, $c4614ab8, $5d681b02, $2a6f2b94
	DW32	$b40bbe37, $c30c8ea1, $5a05df1b, $2d02ef8d	

crc32result:
	.DS 4
usage:
	.DB "Please provide a filename\r\n",0
filehandle:
	.DB 0
filesize:
	.DS 3
checksum:
	.DS 1
buffer:
	.DS BUFFERSIZE
