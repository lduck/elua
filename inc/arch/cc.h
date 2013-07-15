/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _CC_H
#define _CC_H

#include "platform.h"
#include "platform_conf.h"
#include "type.h"

/* Define platform endianness */
#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif /* BYTE_ORDER */

/// Typedefs for the types used by lwip

/// The unsigned data types
//typedef u8      u8_t;
//typedef u16     u16_t;
typedef u32     u32_t;
//
///// The signed counterparts
typedef s8       s8_t;
typedef s16      s16_t;
typedef s32      s32_t;

/// A generic pointer type
typedef u32 mem_ptr_t;

/// Display name of types
#define X8_F            "02x"
#define U16_F           "hu"
#define S16_F           "hd"
#define X16_F           "hx"
#define U32_F           "lu"
#define S32_F           "ld"
#define X32_F           "lx"

/// Compiler hints for packing lwip's structures
#define PACK_STRUCT_FIELD(x)    x __attribute__((packed))
//#define PACK_STRUCT_FIELD(x)    x
#define PACK_STRUCT_STRUCT      __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END


/* prototypes for printf() and abort() */
#include <stdio.h>
#include <stdlib.h>
/* Plaform specific diagnostic output */
#define LWIP_PLATFORM_DIAG(x)   do {printf x;} while(0)

#define LWIP_PLATFORM_ASSERT(x) do {printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); fflush(NULL); abort();} while(0)

#endif  // _CC_H
