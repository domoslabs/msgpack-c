/*
 * MessagePack for C dynamic typing routine
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_OBJECT_H
#define DMSGPACK_OBJECT_H

#include "zone.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup dmsgpack_object Dynamically typed object
 * @ingroup dmsgpack
 * @{
 */

typedef enum {
    DMSGPACK_OBJECT_NIL                  = 0x00,
    DMSGPACK_OBJECT_BOOLEAN              = 0x01,
    DMSGPACK_OBJECT_POSITIVE_INTEGER     = 0x02,
    DMSGPACK_OBJECT_NEGATIVE_INTEGER     = 0x03,
    DMSGPACK_OBJECT_FLOAT32              = 0x0a,
    DMSGPACK_OBJECT_FLOAT64              = 0x04,
    DMSGPACK_OBJECT_FLOAT                = 0x04,
#if defined(DMSGPACK_USE_LEGACY_NAME_AS_FLOAT)
    DMSGPACK_OBJECT_DOUBLE               = DMSGPACK_OBJECT_FLOAT, /* obsolete */
#endif /* DMSGPACK_USE_LEGACY_NAME_AS_FLOAT */
    DMSGPACK_OBJECT_STR                  = 0x05,
    DMSGPACK_OBJECT_ARRAY                = 0x06,
    DMSGPACK_OBJECT_MAP                  = 0x07,
    DMSGPACK_OBJECT_BIN                  = 0x08,
    DMSGPACK_OBJECT_EXT                  = 0x09
} dmsgpack_object_type;


struct dmsgpack_object;
struct dmsgpack_object_kv;

typedef struct {
    uint32_t size;
    struct dmsgpack_object* ptr;
} dmsgpack_object_array;

typedef struct {
    uint32_t size;
    struct dmsgpack_object_kv* ptr;
} dmsgpack_object_map;

typedef struct {
    uint32_t size;
    const char* ptr;
} dmsgpack_object_str;

typedef struct {
    uint32_t size;
    const char* ptr;
} dmsgpack_object_bin;

typedef struct {
    int8_t type;
    uint32_t size;
    const char* ptr;
} dmsgpack_object_ext;

typedef union {
    bool boolean;
    uint64_t u64;
    int64_t  i64;
#if defined(DMSGPACK_USE_LEGACY_NAME_AS_FLOAT)
    double   dec; /* obsolete*/
#endif /* DMSGPACK_USE_LEGACY_NAME_AS_FLOAT */
    double   f64;
    dmsgpack_object_array array;
    dmsgpack_object_map map;
    dmsgpack_object_str str;
    dmsgpack_object_bin bin;
    dmsgpack_object_ext ext;
} dmsgpack_object_union;

typedef struct dmsgpack_object {
    dmsgpack_object_type type;
    dmsgpack_object_union via;
} dmsgpack_object;

typedef struct dmsgpack_object_kv {
    dmsgpack_object key;
    dmsgpack_object val;
} dmsgpack_object_kv;

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_nil(dmsgpack_object* d);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_boolean(dmsgpack_object* d, bool v);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_unsigned_integer(dmsgpack_object* d, uint64_t v);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_signed_integer(dmsgpack_object* d, int64_t v);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_float32(dmsgpack_object* d, float v);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_float64(dmsgpack_object* d, double v);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_str(dmsgpack_object* d, const char* data, uint32_t size);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_bin(dmsgpack_object* d, const char* data, uint32_t size);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_ext(dmsgpack_object* d, int8_t type, const char* data, uint32_t size);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_array(dmsgpack_object* d, dmsgpack_object* data, uint32_t size);

DMSGPACK_DLLEXPORT
void dmsgpack_object_init_map(dmsgpack_object* d, dmsgpack_object_kv* data, uint32_t size);

#if !defined(_KERNEL_MODE)
DMSGPACK_DLLEXPORT
void dmsgpack_object_print(FILE* out, dmsgpack_object o);
#endif

DMSGPACK_DLLEXPORT
int dmsgpack_object_print_buffer(char *buffer, size_t buffer_size, dmsgpack_object o);

DMSGPACK_DLLEXPORT
bool dmsgpack_object_equal(const dmsgpack_object x, const dmsgpack_object y);

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/object.h */
