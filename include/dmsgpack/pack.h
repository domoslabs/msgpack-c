/*
 * MessagePack for C packing routine
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_PACK_H
#define DMSGPACK_PACK_H

#include "pack_define.h"
#include "object.h"
#include "timestamp.h"
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup dmsgpack_buffer Buffers
 * @ingroup dmsgpack
 * @{
 * @}
 */

/**
 * @defgroup dmsgpack_pack Serializer
 * @ingroup dmsgpack
 * @{
 */

typedef int (*dmsgpack_packer_write)(void* data, const char* buf, size_t len);

typedef struct dmsgpack_packer {
    void* data;
    dmsgpack_packer_write callback;
} dmsgpack_packer;

static void dmsgpack_packer_init(dmsgpack_packer* pk, void* data, dmsgpack_packer_write callback);

static dmsgpack_packer* dmsgpack_packer_new(void* data, dmsgpack_packer_write callback);
static void dmsgpack_packer_free(dmsgpack_packer* pk);

static int dmsgpack_pack_char(dmsgpack_packer* pk, char d);

static int dmsgpack_pack_signed_char(dmsgpack_packer* pk, signed char d);
static int dmsgpack_pack_short(dmsgpack_packer* pk, short d);
static int dmsgpack_pack_int(dmsgpack_packer* pk, int d);
static int dmsgpack_pack_long(dmsgpack_packer* pk, long d);
static int dmsgpack_pack_long_long(dmsgpack_packer* pk, long long d);
static int dmsgpack_pack_unsigned_char(dmsgpack_packer* pk, unsigned char d);
static int dmsgpack_pack_unsigned_short(dmsgpack_packer* pk, unsigned short d);
static int dmsgpack_pack_unsigned_int(dmsgpack_packer* pk, unsigned int d);
static int dmsgpack_pack_unsigned_long(dmsgpack_packer* pk, unsigned long d);
static int dmsgpack_pack_unsigned_long_long(dmsgpack_packer* pk, unsigned long long d);

static int dmsgpack_pack_uint8(dmsgpack_packer* pk, uint8_t d);
static int dmsgpack_pack_uint16(dmsgpack_packer* pk, uint16_t d);
static int dmsgpack_pack_uint32(dmsgpack_packer* pk, uint32_t d);
static int dmsgpack_pack_uint64(dmsgpack_packer* pk, uint64_t d);
static int dmsgpack_pack_int8(dmsgpack_packer* pk, int8_t d);
static int dmsgpack_pack_int16(dmsgpack_packer* pk, int16_t d);
static int dmsgpack_pack_int32(dmsgpack_packer* pk, int32_t d);
static int dmsgpack_pack_int64(dmsgpack_packer* pk, int64_t d);

static int dmsgpack_pack_fix_uint8(dmsgpack_packer* pk, uint8_t d);
static int dmsgpack_pack_fix_uint16(dmsgpack_packer* pk, uint16_t d);
static int dmsgpack_pack_fix_uint32(dmsgpack_packer* pk, uint32_t d);
static int dmsgpack_pack_fix_uint64(dmsgpack_packer* pk, uint64_t d);
static int dmsgpack_pack_fix_int8(dmsgpack_packer* pk, int8_t d);
static int dmsgpack_pack_fix_int16(dmsgpack_packer* pk, int16_t d);
static int dmsgpack_pack_fix_int32(dmsgpack_packer* pk, int32_t d);
static int dmsgpack_pack_fix_int64(dmsgpack_packer* pk, int64_t d);

static int dmsgpack_pack_float(dmsgpack_packer* pk, float d);
static int dmsgpack_pack_double(dmsgpack_packer* pk, double d);

static int dmsgpack_pack_nil(dmsgpack_packer* pk);
static int dmsgpack_pack_true(dmsgpack_packer* pk);
static int dmsgpack_pack_false(dmsgpack_packer* pk);

static int dmsgpack_pack_array(dmsgpack_packer* pk, size_t n);

static int dmsgpack_pack_map(dmsgpack_packer* pk, size_t n);

static int dmsgpack_pack_str(dmsgpack_packer* pk, size_t l);
static int dmsgpack_pack_str_body(dmsgpack_packer* pk, const void* b, size_t l);
static int dmsgpack_pack_str_with_body(dmsgpack_packer* pk, const void* b, size_t l);

static int dmsgpack_pack_v4raw(dmsgpack_packer* pk, size_t l);
static int dmsgpack_pack_v4raw_body(dmsgpack_packer* pk, const void* b, size_t l);

static int dmsgpack_pack_bin(dmsgpack_packer* pk, size_t l);
static int dmsgpack_pack_bin_body(dmsgpack_packer* pk, const void* b, size_t l);
static int dmsgpack_pack_bin_with_body(dmsgpack_packer* pk, const void* b, size_t l);

static int dmsgpack_pack_ext(dmsgpack_packer* pk, size_t l, int8_t type);
static int dmsgpack_pack_ext_body(dmsgpack_packer* pk, const void* b, size_t l);
static int dmsgpack_pack_ext_with_body(dmsgpack_packer* pk, const void* b, size_t l, int8_t type);

static int dmsgpack_pack_timestamp(dmsgpack_packer* pk, const dmsgpack_timestamp* d);

DMSGPACK_DLLEXPORT
int dmsgpack_pack_object(dmsgpack_packer* pk, dmsgpack_object d);


/** @} */


#define dmsgpack_pack_inline_func(name) \
    inline int dmsgpack_pack ## name

#define dmsgpack_pack_inline_func_cint(name) \
    inline int dmsgpack_pack ## name

#define dmsgpack_pack_inline_func_fixint(name) \
    inline int dmsgpack_pack_fix ## name

#define dmsgpack_pack_user dmsgpack_packer*

#define dmsgpack_pack_append_buffer(user, buf, len) \
    return (*(user)->callback)((user)->data, (const char*)buf, len)

#include "pack_template.h"

inline void dmsgpack_packer_init(dmsgpack_packer* pk, void* data, dmsgpack_packer_write callback)
{
    pk->data = data;
    pk->callback = callback;
}

inline dmsgpack_packer* dmsgpack_packer_new(void* data, dmsgpack_packer_write callback)
{
    dmsgpack_packer* pk = (dmsgpack_packer*)calloc(1, sizeof(dmsgpack_packer));
    if(!pk) { return NULL; }
    dmsgpack_packer_init(pk, data, callback);
    return pk;
}

inline void dmsgpack_packer_free(dmsgpack_packer* pk)
{
    free(pk);
}

inline int dmsgpack_pack_str_with_body(dmsgpack_packer* pk, const void* b, size_t l)
 {
     int ret = dmsgpack_pack_str(pk, l);
     if (ret != 0) { return ret; }
     return dmsgpack_pack_str_body(pk, b, l);
 }

 inline int dmsgpack_pack_bin_with_body(dmsgpack_packer* pk, const void* b, size_t l)
 {
     int ret = dmsgpack_pack_bin(pk, l);
     if (ret != 0) { return ret; }
     return dmsgpack_pack_bin_body(pk, b, l);
 }

 inline int dmsgpack_pack_ext_with_body(dmsgpack_packer* pk, const void* b, size_t l, int8_t type)
 {
     int ret = dmsgpack_pack_ext(pk, l, type);
     if (ret != 0) { return ret; }
     return dmsgpack_pack_ext_body(pk, b, l);
 }
 
#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/pack.h */
