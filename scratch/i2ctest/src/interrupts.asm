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
			
;* I2C Buffer
I2C_BUFFERLENGTH	.equ	32
;* I2C status codes from I2C_SR
I2C_BUSERROR		.equ 	00h
I2C_START			.equ	08h
I2C_REPEATED_START	.equ	10h
I2C_AW_ACKED		.equ	18h
I2C_AW_NACKED		.equ	20h
I2C_DB_M_ACKED		.equ	28h
I2C_DB_M_NACKED		.equ	30h
I2C_M_ARBLOST		.equ	38h

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

			; Generic
			;CP		I2C_BUSERROR
			;JR		Z, i2c_case_buserror
			; All master
			CP		I2C_START
			JR		Z, i2c_casestart
			CP		I2C_REPEATED_START
			JR		Z, i2c_caserepstart
			; Master transmitter
			CP		I2C_AW_ACKED
			JR		Z, i2c_case_aw_acked
			CP		I2C_DB_M_ACKED
			JR		Z, i2c_case_db_acked
			CP		I2C_AW_NACKED
			JR		i2c_case_aw_nacked
			CP		I2C_DB_M_NACKED
			JR		Z, i2c_case_db_nacked
			CP		I2C_M_ARBLOST
			JR		Z, i2c_releasebus
			
			
			; case default - do nothing
			JR		i2c_end

i2c_case_buserror:
			; perform software reset of the bus
			XOR		A
			OUT0	(I2C_SRR),A
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
			JR		i2c_end
			
i2c_casestart:
i2c_caserepstart:
			LD		A, (_i2c_slarw)		; load slave address and r/w bit
			OUT0	(I2C_DR), A		; store to I2C Data Register
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_AAK
			OUT0	(I2C_CTL),A		; set to Control register
			JR		i2c_end

i2c_case_aw_acked:
i2c_case_db_acked:
			LD		A, (_i2c_mbindex)	; master buffer index
			LD		L,A
			LD		A, (_i2c_mbufferlength)
			CP		L
			JR		Z, i2c_checkstop

			LD		HL, _i2c_mbuffer
			LD		A, (_i2c_mbindex)

			; loop
			LD		B,A ; loop counter
loopx:
			XOR		A,A
			OR		B
			JR		Z, loopxend
			INC		HL
			DEC		B
			JR		loopx
loopxend:	
			; Load indexed byte from buffer
			LD		A, (HL)

			OUT0	(I2C_DR), A		; store to I2C Data Register
			LD		A, (_i2c_mbindex)
			INC		A
			LD		(_i2c_mbindex),A	; _mbindex++
			
			
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_AAK
			OUT0	(I2C_CTL),A		; set to Control register
			JR		i2c_end

i2c_case_aw_nacked:
i2c_case_db_nacked:
			LD		A, I2C_ERROR
			LD		(_i2c_state),A
			JR 		i2c_sendstop

i2c_checkstop:
			LD		A,(_i2c_sendstop)
			CP		01h
			JR		Z, i2c_sendstop
			; we are going to send start
			LD		A, 1
			LD		(_i2c_inrepstart), A
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
			JR		i2c_end
			
i2c_sendstop:
			; send stop
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB | I2C_CTL_STP
			OUT0	(I2C_CTL),A		; set to Control register
			; maybe we should wait for the bus to stop here
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
			JR		i2c_end
i2c_releasebus:
			; relinquish the bus
			LD		A, I2C_CTL_IEN | I2C_CTL_ENAB
			OUT0	(I2C_CTL),A		; set to Control register
			; maybe we should wait for the bus to stop here
			LD		HL, _i2c_state
			LD		A, I2C_READY	; READY state
			LD		(HL),A
			JR		i2c_end

i2c_end:
			POP		BC
			POP		HL
			POP		AF
			EI
			RETI.L


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
			
