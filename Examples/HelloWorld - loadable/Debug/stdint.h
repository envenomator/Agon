/*!\file stdint.h
 * \brief Definition file for data type and other definitions used
 *        across eZ80 ZiLOG Standard Library.
 *
 *  This file contains data type and other definitions that are
 *  used across ZiLOG Standard Library implementations for eZ80 and
 *  eZ80 Acclaim! series of microprocessors and microcontrollers.
 *
 *  This source file uses Doxygen-style comments to generate documentation
 *  automatically.
 *
 *  Copyright (C) 1999-2004 by  ZiLOG, Inc.
 *  All Rights Reserved.
 *
 *  Modified to match stdint.h
 */

#ifndef _STDINT_H_
#define _STDINT_H_



#if 0 //removed
#ifndef NULL
#define NULL	(void *)0	//!< The NULL definition.
#endif
#ifndef TRUE
#define TRUE	1		//!< The TRUE definition.
#endif
#ifndef FALSE
#define FALSE	0		//!< The FALSE definition.
#endif
#endif


/*! The type name for void data type char */
typedef void VOID ;

/*! The type name for standard C data type char */
typedef char CHAR ;

/*! The type name for standard C data type unsigned char */
typedef unsigned char UCHAR;

/*! The type name for standard C data type unsigned char as byte */
typedef unsigned char byte;

/*! The type name for standard C data type unsigned char as bool */
typedef	unsigned char bool;

/*! The type name for standard C data type char as integer */
typedef char int8_t;

/*! The type name for standard C data type unsigned char as unsigned integer */
typedef unsigned char uint8_t;

/*! The type name for standard C data type short */
typedef short int16_t;

/*! The type name for standard C data type unsigned short */
typedef unsigned short uint16_t;

/*! The type name for standard C data type unsigned short as word*/
typedef unsigned short word;

/*! The type name for standard C data type int */
typedef int int24_t;

/*! The type name for standard C data type int */
typedef int INT;

/*! The type name for standard C data type unsigned int */
typedef unsigned int uint_t;

/*! The type name for standard C data type unsigned int */
typedef unsigned int uint24_t;

/*! The type name for standard C data type long */
typedef long int32_t;

/*! The type name for standard C data type unsigned long */
typedef unsigned long uint32_t;

/*! The type name for standard C data type unsigned long as dword */
typedef unsigned long dword;

/*! The type name for standard C data type float */
typedef float float32;


#endif /*! _STDINT_H_ */

