;
; Title:	AGON I2C - Interrupt vector and BSS variables
; Author:	Jeroen Venema
; Created:	27/12/2022
; Last Updated:	27/12/2022
;
; Modinfo:
; 27/12/2022:	Initial version

			INCLUDE	"../src/equs.inc"
			INCLUDE "ez80f92.inc"

;* I2C Buffer
I2C_BUFFERLENGTH	.equ	32
;* I2C status codes from I2C_SR
I2C_BUSERROR			.equ 	00h
I2C_START				.equ	08h
I2C_REPEATED_START		.equ	10h
I2C_AW_ACKED			.equ	18h
I2C_AW_NACKED			.equ	20h
I2C_DB_M_ACKED			.equ	28h
I2C_DB_M_NACKED			.equ	30h
I2C_MR_AR_ACK			.equ	40h
I2C_MR_AR_NACK			.equ	48h
I2C_MR_DBR_ACK			.equ	50h
I2C_MR_DBR_NACK			.equ	58h
I2C_ARBLOST				.equ	38h
I2C_ARBLOST_SLAW_ACK	.equ	68h	; slave code - not checked currently
I2C_ARBLOST_GCAR_ACK	.equ	78h ; slave code - not checked currently
I2C_ARBLOST_SLAR_ACK	.equ	b0h ; slave code - not checked currently

;* Return status codes to read/write caller
RET_OK					.equ	00h	; ok
RET_SLA_NACK			.equ	01h	; address sent, nack received
RET_DATA_NACK			.equ	02h ; data sent, nack received
RET_ARB_LOST			.equ	04h ; arbitration lost
RET_BUS_ERROR			.equ	08h ; Bus error

;* I2C STATES
I2C_READY				.equ	00h
I2C_MTX					.equ	01h
I2C_MRX					.equ	02h
I2C_SRX					.equ	04h
I2C_STX					.equ	08h

;* I2C_CTL bits
I2C_CTL_IEN				.equ	10000000b
I2C_CTL_ENAB			.equ	01000000b
I2C_CTL_STA				.equ	00100000b
I2C_CTL_STP				.equ	00010000b
I2C_CTL_IFLG			.equ	00001000b
I2C_CTL_AAK				.equ	00000100b
						
			SEGMENT BSS		; This section is reset to 0 in cstartup.asm
			
			XDEF	_i2c_slarw
			XDEF	_i2c_error
			XDEF	_i2c_state
			XDEF	_i2c_mbindex
			XDEF	_i2c_sendstop
			XDEF	_i2c_inrepstart
			XDEF	_i2c_mbuffer
			XDEF	_i2c_mbufferlength
			XDEF	_i2c_debug
			XDEF	_i2c_debugptr
			XDEF	_i2c_debugcnt

; I2C protocol variables
_i2c_slarw:			DS	1		; 7bit slave address + R/W bit
_i2c_error:			DS	1		; Error report to caller application
_i2c_state:			DS	1		; I2C current state
_i2c_mbindex:		DS	1		; master buffer index
_i2c_mbufferlength:	DS	1		; master buffer length
_i2c_sendstop:		DS	1		; boolean - send stop?
_i2c_inrepstart:	DS	1		; boolean - in repetitive start?
_i2c_mbuffer:		DS	32		; master buffer
_i2c_debug			DS 256	
_i2c_debugptr		DS	3
_i2c_debugcnt		DS	1
			
			.ASSUME	ADL = 1

			DEFINE .STARTUP, SPACE = ROM
			SEGMENT .STARTUP

			XDEF	_i2c_handler

; AGON I2C Interrupt handler
_i2c_handler:
			DI
			PUSH	AF
			PUSH	HL
			PUSH	BC
			
			IN0		A, (I2C_SR)		; input I2C status register - switch case to this value

			;PUSH	AF
			; DEBUG
			;LD		HL, (_i2c_debugptr)
			;LD		(HL),A
			;LD		BC,HL
			;INC		BC
			;LD		HL, _i2c_debugptr
			;LD		(HL),BC
			
			;LD		A, (_i2c_debugcnt)
			;INC		A
			;LD		(_i2c_debugcnt),A
			
			;POP		AF

			; Generic
			CP		I2C_BUSERROR
			JP		Z, i2c_case_buserror
			; All master
			CP		I2C_START
			JP		Z, i2c_case_master_start
			CP		I2C_REPEATED_START
			JP		Z, i2c_case_master_repstart
			; Master transmitter
			CP		I2C_AW_ACKED
			JP		Z, i2c_case_aw_acked
			CP		I2C_DB_M_ACKED
			JP		Z, i2c_case_db_acked
			CP		I2C_AW_NACKED
			JP		Z, i2c_case_aw_nacked
			CP		I2C_DB_M_NACKED
			JP		Z, i2c_case_db_nacked
			; Master receiver
			CP		I2C_MR_DBR_ACK
			JP		Z, i2c_case_mr_dbr_ack
			CP		I2C_MR_AR_ACK
			JP		Z, i2c_case_mr_ar_ack
			CP		I2C_MR_DBR_NACK
			JP		Z, i2c_case_mr_dbr_nack
			CP		I2C_MR_AR_NACK
			JP		Z, i2c_case_mr_ar_nack
			; Arbitration lost
			CP		I2C_ARBLOST
			JP		Z, i2c_case_arblost

			; case default - do nothing
			JP		i2c_end

i2c_case_mr_ar_nack: ; 48h
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_STA
			JP		i2c_end
			
i2c_case_mr_dbr_nack: ; 58h
			; calculate offset address into i2c_mbuffer
			LD		HL, _i2c_mbuffer
			LD		A, (_i2c_mbindex)
			LD		B,A ; loop counter
$$:
			XOR		A,A
			OR		B
			JR		Z, $F
			INC		HL
			DEC		B
			JR		$B
$$:	
			IN0		A,(I2C_DR)			; load byte from I2C Data Register
			LD		(HL),A				; store in buffer at calculated index
			LD		A, (_i2c_mbindex)
			INC		A
			LD		(_i2c_mbindex),A	; _mbindex++

			;JP		i2c_checkstop
			JP		i2c_sendstop
			
i2c_case_mr_dbr_ack: ; 50h
			; calculate offset address into i2c_mbuffer
			LD		HL, _i2c_mbuffer
			LD		A, (_i2c_mbindex)
			LD		B,A ; loop counter
$$:
			XOR		A,A
			OR		B
			JR		Z, $F
			INC		HL
			DEC		B
			JR		$B
$$:	
			IN0		A,(I2C_DR)			; load byte from I2C Data Register
			LD		(HL),A				; store in buffer at calculated index
			LD		A, (_i2c_mbindex)
			INC		A
			LD		(_i2c_mbindex),A	; _mbindex++

test:
i2c_case_mr_ar_ack: ; 40h
			;receive single byte
			; Just send a NACK, to see how this fails
			;LD		A, (_i2c_slarw)		; load slave address and r/w bit
			;OUT0	(I2C_DR), A		; store to I2C Data Register
			;LD		A, I2C_CTL_IEN | I2C_CTL_ENAB
			;OUT0	(I2C_CTL),A		; set to Control register
			
			LD		A, (_i2c_mbindex)	; master buffer index
			LD		L,A
			LD		A, (_i2c_mbufferlength)
			CP		L
			JR		Z, i2c_replywithoutack
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_AAK
			OUT0	(I2C_CTL),A		; set to Control register
			JP		i2c_end

i2c_replywithoutack:
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB
			OUT0	(I2C_CTL),A		; set to Control register
			JP		i2c_end
						
i2c_case_master_start:
i2c_case_master_repstart:
			LD		A, (_i2c_slarw)		; load slave address and r/w bit
			OUT0	(I2C_DR), A		; store to I2C Data Register
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_AAK
			OUT0	(I2C_CTL),A		; set to Control register
			JP		i2c_end

i2c_case_aw_acked:
i2c_case_db_acked:
			LD		A, (_i2c_mbindex)	; master buffer index
			LD		L,A
			LD		A, (_i2c_mbufferlength)
			CP		L
			JP		Z, i2c_checkstop

			; calculate offset address into i2c_mbuffer
			LD		HL, _i2c_mbuffer
			LD		A, (_i2c_mbindex)
			LD		B,A ; loop counter
$$:
			XOR		A,A
			OR		B
			JR		Z, $F
			INC		HL
			DEC		B
			JP		$B
$$:	
			; Load indexed byte from buffer
			LD		A, (HL)

			OUT0	(I2C_DR), A		; store to I2C Data Register
			LD		A, (_i2c_mbindex)
			INC		A
			LD		(_i2c_mbindex),A	; _mbindex++
			
			
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_AAK
			OUT0	(I2C_CTL),A		; set to Control register
			JP		i2c_end

i2c_case_aw_nacked:
			LD		A, RET_SLA_NACK
			LD		(_i2c_error),A
			JP		i2c_sendstop
			
i2c_case_db_nacked:
			LD		A, RET_DATA_NACK
			LD		(_i2c_state),A
			JP 		i2c_sendstop

i2c_case_arblost:
			LD		A, RET_ARB_LOST
			LD		(_i2c_state),A
			JP 		i2c_releasebus

i2c_case_buserror:
			LD		A, RET_BUS_ERROR
			LD		(_i2c_state),A
			
			; perform software reset of the bus
			XOR		A
			OUT0	(I2C_SRR),A
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
			JP		i2c_end

i2c_checkstop:
			LD		A,(_i2c_sendstop)
			CP		01h
			JP		Z, i2c_sendstop
			; we are going to send start
			LD		A, 1
			LD		(_i2c_inrepstart), A
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
			JP		i2c_end
			
i2c_sendstop:
			; send stop
			;LD		A, I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_STP
			LD		A, I2C_CTL_ENAB | I2C_CTL_STP			
			OUT0	(I2C_CTL),A		; set to Control register
			
			LD		L, 16			; rudimentary timeout, can't use timer2 during ISR
$$:
			DEC		L
			JR		Z, i2c_handletimeout	; timeout
			IN0		A,(I2C_CTL)
			AND		I2C_CTL_STP			; STP bit still in place?
			JR		NZ, $B	; then we need to wait some more
			
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
			JP		i2c_end

i2c_handletimeout:
			CALL	_i2c_init
			JP		i2c_releasebus
			
i2c_releasebus:
			; relinquish the bus
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB
			OUT0	(I2C_CTL),A		; set to Control register
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
			JP		i2c_end

i2c_end:
			; DEBUG
			;LD		A,(_i2c_debugcnt)
			;CP		30
			;JR		NZ, $F
			;LD		A, I2C_CTL_ENAB | I2C_CTL_STP			
			;OUT0	(I2C_CTL),A		; set to Control register
			
$$:			
			
			POP		BC
			POP		HL
			POP		AF
			EI
			RETI.L


_i2c_init:
			XOR		A,A
			OUT0	(I2C_CTL),A		; reset
			LD		A, 04h			; CLK_PPD_I2C_OFF, bit 2
			OUT0	(CLK_PPD1),A	; power down clock to the I2C block
			LD		A,I2C_CTL_ENAB	; enable the bus, before enabling the clock to the I2C block
			OUT0	(I2C_CTL),A
			XOR		A,A
			OUT0	(CLK_PPD1),A	; enable clock to the I2C block
			RET


			END