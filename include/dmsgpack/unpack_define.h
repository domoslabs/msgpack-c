/*
 * MessagePack unpacking routine template
 *
 * Copyright (C) 2008-2010 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_UNPACK_DEFINE_H
#define DMSGPACK_UNPACK_DEFINE_H

#include "dmsgpack/sysdep.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef DMSGPACK_EMBED_STACK_SIZE
#define DMSGPACK_EMBED_STACK_SIZE 32
#endif


typedef enum {
    DMSGPACK_CS_HEADER            = 0x00,  // nil

    //DMSGPACK_CS_                = 0x01,
    //DMSGPACK_CS_                = 0x02,  // false
    //DMSGPACK_CS_                = 0x03,  // true

    DMSGPACK_CS_BIN_8             = 0x04,
    DMSGPACK_CS_BIN_16            = 0x05,
    DMSGPACK_CS_BIN_32            = 0x06,

    DMSGPACK_CS_EXT_8             = 0x07,
    DMSGPACK_CS_EXT_16            = 0x08,
    DMSGPACK_CS_EXT_32            = 0x09,

    DMSGPACK_CS_FLOAT             = 0x0a,
    DMSGPACK_CS_DOUBLE            = 0x0b,
    DMSGPACK_CS_UINT_8            = 0x0c,
    DMSGPACK_CS_UINT_16           = 0x0d,
    DMSGPACK_CS_UINT_32           = 0x0e,
    DMSGPACK_CS_UINT_64           = 0x0f,
    DMSGPACK_CS_INT_8             = 0x10,
    DMSGPACK_CS_INT_16            = 0x11,
    DMSGPACK_CS_INT_32            = 0x12,
    DMSGPACK_CS_INT_64            = 0x13,

    DMSGPACK_CS_FIXEXT_1          = 0x14,
    DMSGPACK_CS_FIXEXT_2          = 0x15,
    DMSGPACK_CS_FIXEXT_4          = 0x16,
    DMSGPACK_CS_FIXEXT_8          = 0x17,
    DMSGPACK_CS_FIXEXT_16         = 0x18,

    DMSGPACK_CS_STR_8             = 0x19, // str8
    DMSGPACK_CS_STR_16            = 0x1a, // str16
    DMSGPACK_CS_STR_32            = 0x1b, // str32
    DMSGPACK_CS_ARRAY_16          = 0x1c,
    DMSGPACK_CS_ARRAY_32          = 0x1d,
    DMSGPACK_CS_MAP_16            = 0x1e,
    DMSGPACK_CS_MAP_32            = 0x1f,

    //DMSGPACK_ACS_BIG_INT_VALUE,
    //DMSGPACK_ACS_BIG_FLOAT_VALUE,
    DMSGPACK_ACS_STR_VALUE,
    DMSGPACK_ACS_BIN_VALUE,
    DMSGPACK_ACS_EXT_VALUE
} dmsgpack_unpack_state;


typedef enum {
    DMSGPACK_CT_ARRAY_ITEM,
    DMSGPACK_CT_MAP_KEY,
    DMSGPACK_CT_MAP_VALUE
} dmsgpack_container_type;


#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/unpack_define.h */

