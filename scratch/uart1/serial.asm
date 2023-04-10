PORT			EQU	%D0			; UART1
				
REG_RBR:		EQU	PORT+0		; Receive buffer
REG_THR:		EQU	PORT+0		; Transmitter holding
REG_DLL:		EQU	PORT+0		; Divisor latch low
REG_IER:		EQU	PORT+1		; Interrupt enable
REG_DLH:		EQU	PORT+1		; Divisor latch high
REG_IIR:		EQU	PORT+2		; Interrupt identification
REG_FCT			EQU	PORT+2;		; Flow control
REG_LCR:		EQU	PORT+3		; Line control
REG_MCR:		EQU	PORT+4		; Modem control
REG_LSR:		EQU	PORT+5		; Line status
REG_MSR:		EQU	PORT+6		; Modem status

REG_SCR:		EQU PORT+7		; Scratch

TX_WAIT			EQU	16384 		; Count before a TX times out

UART_LSR_ERR	EQU %80			; Error
UART_LSR_ETX	EQU %40			; Transmit empty
UART_LSR_ETH	EQU	%20			; Transmit holding register empty
UART_LSR_RDY	EQU	%01			; Data ready


			.ASSUME	ADL = 1
			SEGMENT CODE
			
			XDEF	_uart1_handler
			XDEF	_uart1_getch
			XDEF	_uart1_putch
			
_uart1_handler:		
			DI
			PUSH	AF
			IN0		A,(REG_LSR)			; Get the line status register
			AND		UART_LSR_RDY		; Check for characters in buffer
			JR		Z, $F				; Nothing received
			LD		A,1
			LD		(uart1_received),A
			IN0		A,(REG_RBR)			; Read the character from the UART receive buffer
			LD		(uart1_buffer),A
			POP		AF
			EI
			RETI.L
$$:			; No character received
			XOR 	A,A
			LD		(uart1_received),A			
			POP		AF
			EI
			RETI.L	
				
; Write a byte to UART1
; A: Data byte to write
; Returns:
; A = 0 if timed out
; A = 1 if written
_uart1_putch:
			PUSH	BC			; Stack BC
			PUSH	AF 			; Stack AF
			LD		BC,TX_WAIT	; Set CB to the transmit timeout
$$:			IN0		A,(REG_LSR)	; Get the line status register
			AND 	UART_LSR_ETX; Check for TX empty
			JR		NZ, $F		; If set, then TX is empty, goto transmit
			DEC		BC
			LD		A, B
			OR		C
			JR		NZ, $B
			POP		AF			; We've timed out at this point so
			POP		BC			; Restore the stack
			XOR		A,A			; Return timeout
			RET	
$$:			POP		AF			; Good to send at this point, so
			OUT0	(REG_THR),A	; Write the character to the UART transmit buffer
			POP		BC			; Restore BC
			LD		A,1			; Return succes, also works when A was 0 originally
			RET 

; Blocking read a byte from UART1
; Returns:
; A = byte read
_uart1_getch:
			LD		A,(uart1_received)
			OR		A
			JR		Z, _uart1_getch
			DI		; atomic acknowledge byte read
			XOR		A,A
			LD		(uart1_received),A
			LD		A,(uart1_buffer)
			EI
			RET
	
			SEGMENT DATA
			
uart1_buffer		DEFB	1	; 64 byte receive buffer
uart1_received		DEFB	1	; boolean