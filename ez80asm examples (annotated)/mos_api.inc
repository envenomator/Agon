;
; Title:	AGON MOS - API for user projects
; Author:	Dean Belfield
;			Adapted for agon-ez80asm by Jeroen Venema
;			Added MOS error codes for return in HL
; Created:	03/08/2022
; Last Updated:	10/08/2023
;
; Modinfo:
; 05/08/2022:	Added mos_feof
; 09/08/2022:	Added system variables: cursorX, cursorY
; 18/08/2022:	Added system variables: scrchar, scrpixel, audioChannel, audioSuccess, vpd_pflags
; 05/09/2022:	Added mos_ren, vdp_pflag_mode
; 24/09/2022:	Added mos_getError, mos_mkdir
; 13/10/2022:	Added mos_oscli
; 23/02/2023:	Added more sysvars, fixed typo in sysvar_audioSuccess, offsets for sysvar_scrCols, sysvar_scrRows
; 04/03/2023:	Added sysvar_scrpixelIndex
; 08/03/2023:	Renamed sysvar_keycode to sysvar_keyascii, added sysvar_vkeycode
; 15/03/2023:	Added mos_copy, mos_getrtc, mos_setrtc, rtc, vdp_pflag_rtc
; 21/03/2023:	Added mos_setintvector, sysvars for keyboard status, vdu codes for vdp
; 22/03/2023:	The VDP commands are now indexed from 0x80
; 29/03/2023:	Added mos_uopen, mos_uclose, mos_ugetc, mos_uputc
; 13/04/2023:	Added FatFS file structures (FFOBJID, FIL, DIR, FILINFO)
; 15/04/2023:	Added mos_getfil, mos_fread, mos_fwrite and mos_flseek
; 19/05/2023:	Added sysvar_scrMode
; 05/06/2023:	Added sysvar_rtcEnable
; 03/08/2023:	Added mos_setkbvector
; 10/08/2023:	Added mos_getkbmap

; VDP control (VDU 23, 0, n)
;
vdp_gp:				EQU 80h
vdp_keycode:		EQU 81h
vdp_cursor:			EQU	82h
vdp_scrchar:		EQU	83h
vdp_scrpixel:		EQU	84h
vdp_audio:			EQU	85h
vdp_mode:			EQU	86h
vdp_rtc:			EQU	87h
vdp_keystate:		EQU	88h
vdp_logicalcoords:	EQU	C0h
vdp_terminalmode:	EQU	FFh

; MOS high level functions
;
mos_getkey:			EQU	00h
mos_load:			EQU	01h
mos_save:			EQU	02h
mos_cd:				EQU	03h
mos_dir:			EQU	04h
mos_del:			EQU	05h
mos_ren:			EQU	06h
mos_mkdir:			EQU	07h
mos_sysvars:		EQU	08h
mos_editline:		EQU	09h
mos_fopen:			EQU	0Ah
mos_fclose:			EQU	0Bh
mos_fgetc:			EQU	0Ch
mos_fputc:			EQU	0Dh
mos_feof:			EQU	0Eh
mos_getError:		EQU	0Fh
mos_oscli:			EQU	10h
mos_copy:			EQU	11h
mos_getrtc:			EQU	12h
mos_setrtc:			EQU	13h
mos_setintvector:	EQU	14h
mos_uopen:			EQU	15h
mos_uclose:			EQU	16h
mos_ugetc:			EQU	17h
mos_uputc:			EQU	18h
mos_getfil:			EQU	19h
mos_fread:			EQU	1Ah
mos_fwrite:			EQU	1Bh
mos_flseek:			EQU	1Ch
mos_setkbvector:	EQU	1Dh
mos_getkbmap:		EQU	1Eh

; MOS program exit codes
;
EXIT_OK:				EQU  0;	"OK",
EXIT_ERROR_SD_ACCESS:	EQU	 1;	"Error accessing SD card",
EXIT_ERROR_ASSERTION:	EQU  2;	"Assertion failed",
EXIT_SD_CARDFAILURE:	EQU  3;	"SD card failure",
EXIT_FILENOTFOUND:		EQU  4;	"Could not find file",
EXIT_PATHNOTFOUND:		EQU  5;	"Could not find path",
EXIT_INVALIDPATHNAME:	EQU  6;	"Invalid path name",
EXIT_ACCESSDENIED_FULL:	EQU  7;	"Access denied or directory full",
EXIT_ACCESSDENIED:		EQU  8;	"Access denied",
EXIT_INVALIDOBJECT:		EQU  9;	"Invalid file/directory object",
EXIT_SD_WRITEPROTECTED:	EQU 10;	"SD card is write protected",
EXIT_INVALIDDRIVENUMBER:EQU 11;	"Logical drive number is invalid",
EXIT_NOVOLUMEWORKAREA:	EQU 12;	"Volume has no work area",
EXIT_NOVALIDFATVOLUME:	EQU 13;	"No valid FAT volume",
EXIT_ERRORMKFS:			EQU 14;	"Error occurred during mkfs",
EXIT_VOLUMETIMEOUT:		EQU 15;	"Volume timeout",
EXIT_VOLUMELOCKED:		EQU 16;	"Volume locked",
EXIT_LFNALLOCATION:		EQU 17;	"LFN working buffer could not be allocated",
EXIT_MAXOPENFILES:		EQU 18;	"Too many open files",
EXIT_INVALIDPARAMETER:	EQU 19;	"Invalid parameter",
EXIT_INVALIDCOMMAND:	EQU 20;	"Invalid command",
EXIT_INVALIDEXECUTABLE:	EQU 21;	"Invalid executable",
; FatFS file access functions
;
ffs_fopen:			EQU	80h
ffs_fclose:			EQU	81h
ffs_fread:			EQU	82h
ffs_fwrite:			EQU	83h
ffs_flseek:			EQU	84h
ffs_ftruncate:		EQU	85h
ffs_fsync:			EQU	86h
ffs_fforward:		EQU	87h
ffs_fexpand:		EQU	88h
ffs_fgets:			EQU	89h
ffs_fputc:			EQU	8Ah
ffs_fputs:			EQU	8Bh
ffs_fprintf:		EQU	8Ch
ffs_ftell:			EQU	8Dh
ffs_feof:			EQU	8Eh
ffs_fsize:			EQU	8Fh
ffs_ferror:			EQU	90h

; FatFS directory access functions
;
ffs_dopen:			EQU	91h
ffs_dclose:			EQU	92h
ffs_dread:			EQU	93h
ffs_dfindfirst:		EQU	94h
ffs_dfindnext:		EQU	95h

; FatFS file and directory management functions
;
ffs_stat:			EQU	96h
ffs_unlink:			EQU	97h
ffs_rename:			EQU	98h
ffs_chmod:			EQU	99h
ffs_utime:			EQU	9Ah
ffs_mkdir:			EQU	9Bh
ffs_chdir:			EQU	9Ch
ffs_chdrive:		EQU	9Dh
ffs_getcwd:			EQU	9Eh

; FatFS volume management and system configuration functions
;
ffs_mount:			EQU	9Fh
ffs_mkfs:			EQU	A0h
ffs_fdisk:			EQU	A1h
ffs_getfree:		EQU	A2h
ffs_getlabel:		EQU	A3h
ffs_setlabel:		EQU	A4h
ffs_setcp:			EQU	A5h
	
; File access modes
;
fa_read:			EQU	01h
fa_write:			EQU	02h
fa_open_existing:	EQU	00h
fa_create_new:		EQU	04h
fa_create_always:	EQU	08h
fa_open_always:		EQU	10h
fa_open_append:		EQU	30h
	
; System variable indexes for api_sysvars
; Index into _sysvars in globals.asm
;
sysvar_time:			EQU	00h	; 4: Clock timer in centiseconds (incremented by 2 every VBLANK)
sysvar_vpd_pflags:		EQU	04h	; 1: Flags to indicate completion of VDP commands
sysvar_keyascii:		EQU	05h	; 1: ASCII keycode, or 0 if no key is pressed
sysvar_keymods:			EQU	06h	; 1: Keycode modifiers
sysvar_cursorX:			EQU	07h	; 1: Cursor X position
sysvar_cursorY:			EQU	08h	; 1: Cursor Y position
sysvar_scrchar:			EQU	09h	; 1: Character read from screen
sysvar_scrpixel:		EQU	0Ah	; 3: Pixel data read from screen (R,B,G)
sysvar_audioChannel:	EQU	0Dh	; 1: Audio channel 
sysvar_audioSuccess:	EQU	0Eh	; 1: Audio channel note queued (0 = no, 1 = yes)
sysvar_scrWidth:		EQU	0Fh	; 2: Screen width in pixels
sysvar_scrHeight:		EQU	11h	; 2: Screen height in pixels
sysvar_scrCols:			EQU	13h	; 1: Screen columns in characters
sysvar_scrRows:			EQU	14h	; 1: Screen rows in characters
sysvar_scrColours:		EQU	15h	; 1: Number of colours displayed
sysvar_scrpixelIndex:	EQU	16h	; 1: Index of pixel data read from screen
sysvar_vkeycode:		EQU	17h	; 1: Virtual key code from FabGL
sysvar_vkeydown:		EQU	18h	; 1: Virtual key state from FabGL (0=up, 1=down)
sysvar_vkeycount:		EQU	19h	; 1: Incremented every time a key packet is received
sysvar_rtc:				EQU	1Ah	; 6: Real time clock data
sysvar_spare:			EQU	20h	; 2: Spare, previously used by rtc
sysvar_keydelay:		EQU	22h	; 2: Keyboard repeat delay
sysvar_keyrate:			EQU	24h	; 2: Keyboard repeat reat
sysvar_keyled:			EQU	26h	; 1: Keyboard LED status
sysvar_scrMode:			EQU	27h	; 1: Screen mode
sysvar_rtcEnable:		EQU	28h	; 1: RTC enable flag (0: disabled, 1: use ESP32 RTC)
	
; Flags for the VPD protocol
;
vdp_pflag_cursor:		EQU	00000001b
vdp_pflag_scrchar:		EQU	00000010b
vdp_pflag_point:		EQU	00000100b
vdp_pflag_audio:		EQU	00001000b
vdp_pflag_mode:			EQU	00010000b
vdp_pflag_rtc:			EQU	00100000b

;
; FatFS structures
; These mirror the structures contained in src_fatfs/ff.h in the MOS project
;
; Object ID and allocation information (FFOBJID)
;
; Indexes into FFOBJID structure
ffobjid_fs:			EQU	0	; 3: Pointer to the hosting volume of this object
ffobjid_id:			EQU	3	; 2: Hosting volume mount ID
ffobjid_attr:		EQU	5	; 1: Object attribute
ffobjid_stat:		EQU	6	; 1: Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:fragmented in this session, b2:sub-directory stretched)
ffobjid_sclust:		EQU	7	; 4: Object data start cluster (0:no cluster or root directory)
ffobjid_objsize:	EQU	11	; 4: Object size (valid when sclust != 0)
;
; File object structure (FIL)
;
; Indexes into FIL structure
fil_obj:		EQU 0	; 15: Object identifier
fil_flag:		EQU	15 	;  1: File status flags
fil_err:		EQU	16	;  1: Abort flag (error code)
fil_fptr:		EQU	17	;  4: File read/write pointer (Zeroed on file open)
fil_clust:		EQU	21	;  4: Current cluster of fpter (invalid when fptr is 0)
fil_sect:		EQU	25	;  4: Sector number appearing in buf[] (0:invalid)
fil_dir_sect:	EQU	29	;  4: Sector number containing the directory entry
fil_dir_ptr:	EQU	33	;  3: Pointer to the directory entry in the win[]
;
; Directory object structure (DIR)
; Indexes into DIR structure
dir_obj:		EQU  0	; 15: Object identifier
dir_dptr:		EQU	15	;  4: Current read/write offset
dir_clust:		EQU	19	;  4: Current cluster
dir_sect:		EQU	23	;  4: Current sector (0:Read operation has terminated)
dir_dir:		EQU	27	;  3: Pointer to the directory item in the win[]
dir_fn:			EQU	30	; 12: SFN (in/out) {body[8],ext[3],status[1]}
dir_blk_ofs:	EQU	42	;  4: Offset of current entry block being processed (0xFFFFFFFF:Invalid)
;
; File information structure (FILINFO)
;
; Indexes into FILINFO structure
filinfo_fsize:		EQU 0	;   4: File size
filinfo_fdate:		EQU	4	;   2: Modified date
filinfo_ftime:		EQU	6	;   2: Modified time
filinfo_fattrib:	EQU	8	;   1: File attribute
filinfo_altname:	EQU	9	;  13: Alternative file name
filinfo_fname:		EQU	22	; 256: Primary file name
;
; Macro for calling the API
; Parameters:
; - function: One of the function numbers listed above
;
	MACRO	MOSCALL	function
			LD	A, function
			RST.LIL	08h
	ENDMACRO 	
