	.include "mos_api.inc"

	XDEF _setsysvar_keyascii
	
	segment CODE
	.assume ADL=1
	

_setsysvar_keyascii:
	push 	ix
	ld 		ix,0
	add 	ix, sp
	ld 		a, (ix+6)
	push	af			; store value on stack
	ld		a, mos_sysvars
	rst.lil	08h
	pop		af
	ld		(ix+sysvar_keyascii), a

	pop		ix
	ret

end
