;
; Title:	AGON MOS - Interrupt handlers
; Author:	Dean Belfield
; Created:	03/08/2022
; Last Updated:	03/08/2022
;
; Modinfo:

			INCLUDE	"macros.inc"
			INCLUDE	"equs.inc"
			INCLUDE "ez80f92.inc"
			
			.ASSUME	ADL = 1

			DEFINE .STARTUP, SPACE = ROM
			SEGMENT .STARTUP
			
			XDEF	_vblank_handler
			XDEF	_timer2_handler
			XDEF	_uart0_handler
			XDEF	_i2c_handler
			
			XREF	_timer2
			XREF	_clock
			XREF	_vdp_protocol_data
			
			XREF	serial_RX
			XREF	serial_TX
			XREF	mos_api
			XREF	vdp_protocol			

			XREF	_i2c_slarw
			XREF	_i2c_state
			XREF	_i2c_mbindex
			XREF	_i2c_mbufferlength
			XREF	_i2c_sendstop
			XREF	_i2c_inrepstart
			XREF	_i2c_mbuffer
			XREF	_i2c_debug
			XREF	_i2c_debugptr
			XREF	_i2c_debugcnt
			
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
I2C_M_ARBLOST			.equ	38h
I2C_MR_AR_ACK			.equ	40h
I2C_MR_AR_NACK			.equ	48h
I2C_MR_DBR_ACK			.equ	50h
I2C_MR_DBR_NACK			.equ	58h

;* I2C STATES
I2C_READY			.equ	00h
I2C_MTX				.equ	01h
I2C_MRX				.equ	02h
I2C_SRX				.equ	04h
I2C_STX				.equ	08h
I2C_ERROR			.equ	80h

;* I2C_CTL bits
I2C_CTL_IEN			.equ	10000000b
I2C_CTL_ENAB		.equ	01000000b
I2C_CTL_STA			.equ	00100000b
I2C_CTL_STP			.equ	00010000b
I2C_CTL_IFLG		.equ	00001000b
I2C_CTL_AAK			.equ	00000100b

; AGON Test I2C Interrupt handler
_i2c_handler:
			DI
			PUSH	AF
			PUSH	HL
			PUSH	BC
			
			IN0		A, (I2C_SR)		; input I2C status register - switch case to this value

			PUSH	AF
			; DEBUG
			LD		HL, (_i2c_debugptr)
			LD		(HL),A
			LD		BC,HL
			INC		BC
			LD		HL, _i2c_debugptr
			LD		(HL),BC
			
			LD		A, (_i2c_debugcnt)
			INC		A
			LD		(_i2c_debugcnt),A
			
			POP		AF

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
			CP		I2C_M_ARBLOST
			JP		Z, i2c_releasebus
			; Master receiver
			CP		I2C_MR_DBR_ACK
			JP		Z, i2c_case_mr_dbr_ack
			CP		I2C_MR_AR_ACK
			JP		Z, i2c_case_mr_ar_ack
			CP		I2C_MR_DBR_NACK
			JP		Z, i2c_case_mr_dbr_nack
			CP		I2C_MR_AR_NACK
			JP		Z, i2c_case_mr_ar_nack
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
loop2:
			XOR		A,A
			OR		B
			JR		Z, loop2end
			INC		HL
			DEC		B
			JR		loop2
loop2end:	
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
loop1:
			XOR		A,A
			OR		B
			JR		Z, loop1end
			INC		HL
			DEC		B
			JR		loop1
loop1end:	
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
			
i2c_case_buserror:
			;JR		i2c_sendstop
			; perform software reset of the bus
			XOR		A
			OUT0	(I2C_SRR),A
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
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
loopx:
			XOR		A,A
			OR		B
			JR		Z, loopxend
			INC		HL
			DEC		B
			JP		loopx
loopxend:	
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
i2c_case_db_nacked:
			LD		A, I2C_ERROR
			LD		(_i2c_state),A
			JP 		i2c_sendstop

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
sendstoploop:
			DEC		L
			JR		Z, i2c_handletimeout	; timeout
			IN0		A,(I2C_CTL)
			AND		I2C_CTL_STP			; STP bit still in place?
			JR		NZ, sendstoploop	; then we need to wait some more
			
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
			LD		A,(_i2c_debugcnt)
			CP		30
			JR		NZ, contend
			LD		A, I2C_CTL_ENAB | I2C_CTL_STP			
			OUT0	(I2C_CTL),A		; set to Control register
			
contend:			
			
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
			
; AGON Vertical Blank Interrupt handler
;
_vblank_handler:	DI
			PUSH		AF
			SET_GPIO 	PB_DR, 2		; Need to set this to 2 for the interrupt to work correctly
			PUSH		BC
			PUSH		DE
			PUSH		HL	
			LD 		HL, (_clock)		; Increment the 32-bit clock counter
			LD		BC, 2			; By 2, effectively timing in centiseconds
			ADD		HL, BC
			LD		(_clock), HL
			LD		A, (_clock + 3)
			ADC		A, 0
			LD		(_clock + 3), A			
			POP		HL
			POP		DE
			POP		BC
			POP		AF
			EI	
			RETI.L
			
; AGON Timer 2 Interrupt Handler
;
_timer2_handler:	DI
			PUSH		AF
			IN0  		A,(TMR2_CTL)		; Clear the timer interrupt
			PUSH		BC
			LD		BC, (_timer2)		; Increment the delay timer
			INC		BC
			LD		(_timer2), BC
			POP		BC
			POP		AF
			EI
			RETI.L
			
; AGON UART0 Interrupt Handler
;
_uart0_handler:		DI
			PUSH		AF
			PUSH		BC
			PUSH		DE
			PUSH		HL
			CALL		serial_RX
			LD		C, A		
			LD		HL, _vdp_protocol_data
			CALL		vdp_protocol
			POP		HL
			POP		DE
			POP		BC
			POP		AF
			EI
			RETI.L	
			
