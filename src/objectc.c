/*
 * MessagePack for C dynamic typing routine
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#if defined(_KERNEL_MODE)
#  undef  _NO_CRT_STDIO_INLINE
#  define _NO_CRT_STDIO_INLINE
#endif

#include "dmsgpack/object.h"
#include "dmsgpack/pack.h"
#include <ctype.h>

#include <stdio.h>
#include <string.h>

#if defined(_MSC_VER)
#if _MSC_VER >= 1800
#include <inttypes.h>
#else
#define PRIu64 "I64u"
#define PRIi64 "I64i"
#define PRIi8 "i"
#endif
#else
#include <inttypes.h>
#endif

#if defined(_KERNEL_MODE)
#  undef  snprintf
#  define snprintf _snprintf
#endif

int dmsgpack_pack_object(dmsgpack_packer* pk, dmsgpack_object d)
{
    switch(d.type) {
    case DMSGPACK_OBJECT_NIL:
        return dmsgpack_pack_nil(pk);

    case DMSGPACK_OBJECT_BOOLEAN:
        if(d.via.boolean) {
            return dmsgpack_pack_true(pk);
        } else {
            return dmsgpack_pack_false(pk);
        }

    case DMSGPACK_OBJECT_POSITIVE_INTEGER:
        return dmsgpack_pack_uint64(pk, d.via.u64);

    case DMSGPACK_OBJECT_NEGATIVE_INTEGER:
        return dmsgpack_pack_int64(pk, d.via.i64);

    case DMSGPACK_OBJECT_FLOAT32:
        return dmsgpack_pack_float(pk, (float)d.via.f64);

    case DMSGPACK_OBJECT_FLOAT64:
        return dmsgpack_pack_double(pk, d.via.f64);

    case DMSGPACK_OBJECT_STR:
        {
            int ret = dmsgpack_pack_str(pk, d.via.str.size);
            if(ret < 0) { return ret; }
            return dmsgpack_pack_str_body(pk, d.via.str.ptr, d.via.str.size);
        }

    case DMSGPACK_OBJECT_BIN:
        {
            int ret = dmsgpack_pack_bin(pk, d.via.bin.size);
            if(ret < 0) { return ret; }
            return dmsgpack_pack_bin_body(pk, d.via.bin.ptr, d.via.bin.size);
        }

    case DMSGPACK_OBJECT_EXT:
        {
            int ret = dmsgpack_pack_ext(pk, d.via.ext.size, d.via.ext.type);
            if(ret < 0) { return ret; }
            return dmsgpack_pack_ext_body(pk, d.via.ext.ptr, d.via.ext.size);
        }

    case DMSGPACK_OBJECT_ARRAY:
        {
            int ret = dmsgpack_pack_array(pk, d.via.array.size);
            if(ret < 0) {
                return ret;
            }
            else {
                dmsgpack_object* o = d.via.array.ptr;
                dmsgpack_object* const oend = d.via.array.ptr + d.via.array.size;
                for(; o != oend; ++o) {
                    ret = dmsgpack_pack_object(pk, *o);
                    if(ret < 0) { return ret; }
                }

                return 0;
            }
        }

    case DMSGPACK_OBJECT_MAP:
        {
            int ret = dmsgpack_pack_map(pk, d.via.map.size);
            if(ret < 0) {
                return ret;
            }
            else {
                dmsgpack_object_kv* kv = d.via.map.ptr;
                dmsgpack_object_kv* const kvend = d.via.map.ptr + d.via.map.size;
                for(; kv != kvend; ++kv) {
                    ret = dmsgpack_pack_object(pk, kv->key);
                    if(ret < 0) { return ret; }
                    ret = dmsgpack_pack_object(pk, kv->val);
                    if(ret < 0) { return ret; }
                }

                return 0;
            }
        }

    default:
        return -1;
    }
}

void dmsgpack_object_init_nil(dmsgpack_object* d) {
    d->type = DMSGPACK_OBJECT_NIL;
}

void dmsgpack_object_init_boolean(dmsgpack_object* d, bool v) {
    d->type = DMSGPACK_OBJECT_BOOLEAN;
    d->via.boolean = v;
}

void dmsgpack_object_init_unsigned_integer(dmsgpack_object* d, uint64_t v) {
    d->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
    d->via.u64 = v;
}

void dmsgpack_object_init_signed_integer(dmsgpack_object* d, int64_t v) {
    if (v < 0) {
        d->type = DMSGPACK_OBJECT_NEGATIVE_INTEGER;
        d->via.i64 = v;
    }
    else {
        d->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
        d->via.u64 = v;
    }
}

void dmsgpack_object_init_float32(dmsgpack_object* d, float v) {
    d->type = DMSGPACK_OBJECT_FLOAT32;
    d->via.f64 = v;
}

void dmsgpack_object_init_float64(dmsgpack_object* d, double v) {
    d->type = DMSGPACK_OBJECT_FLOAT64;
    d->via.f64 = v;
}

void dmsgpack_object_init_str(dmsgpack_object* d, const char* data, uint32_t size) {
    d->type = DMSGPACK_OBJECT_STR;
    d->via.str.ptr = data;
    d->via.str.size = size;
}

void dmsgpack_object_init_bin(dmsgpack_object* d, const char* data, uint32_t size) {
    d->type = DMSGPACK_OBJECT_BIN;
    d->via.bin.ptr = data;
    d->via.bin.size = size;
}

void dmsgpack_object_init_ext(dmsgpack_object* d, int8_t type, const char* data, uint32_t size) {
    d->type = DMSGPACK_OBJECT_EXT;
    d->via.ext.type = type;
    d->via.ext.ptr = data;
    d->via.ext.size = size;
}

void dmsgpack_object_init_array(dmsgpack_object* d, dmsgpack_object* data, uint32_t size) {
    d->type = DMSGPACK_OBJECT_ARRAY;
    d->via.array.ptr = data;
    d->via.array.size = size;
}

void dmsgpack_object_init_map(dmsgpack_object* d, dmsgpack_object_kv* data, uint32_t size) {
    d->type = DMSGPACK_OBJECT_MAP;
    d->via.map.ptr = data;
    d->via.map.size = size;
}

#if !defined(_KERNEL_MODE)

static void dmsgpack_object_bin_print(FILE* out, const char *ptr, size_t size)
{
    size_t i;
    for (i = 0; i < size; ++i) {
        if (ptr[i] == '"') {
            fputs("\\\"", out);
        } else if (isprint((unsigned char)ptr[i])) {
            fputc(ptr[i], out);
        } else {
            fprintf(out, "\\x%02x", (unsigned char)ptr[i]);
        }
    }
}

void dmsgpack_object_print(FILE* out, dmsgpack_object o)
{
    switch(o.type) {
    case DMSGPACK_OBJECT_NIL:
        fprintf(out, "nil");
        break;

    case DMSGPACK_OBJECT_BOOLEAN:
        fprintf(out, (o.via.boolean ? "true" : "false"));
        break;

    case DMSGPACK_OBJECT_POSITIVE_INTEGER:
#if defined(PRIu64)
        fprintf(out, "%" PRIu64, o.via.u64);
#else
        if (o.via.u64 > ULONG_MAX)
            fprintf(out, "over 4294967295");
        else
            fprintf(out, "%lu", (unsigned long)o.via.u64);
#endif
        break;

    case DMSGPACK_OBJECT_NEGATIVE_INTEGER:
#if defined(PRIi64)
        fprintf(out, "%" PRIi64, o.via.i64);
#else
        if (o.via.i64 > LONG_MAX)
            fprintf(out, "over +2147483647");
        else if (o.via.i64 < LONG_MIN)
            fprintf(out, "under -2147483648");
        else
            fprintf(out, "%ld", (signed long)o.via.i64);
#endif
        break;

    case DMSGPACK_OBJECT_FLOAT32:
    case DMSGPACK_OBJECT_FLOAT64:
        fprintf(out, "%f", o.via.f64);
        break;

    case DMSGPACK_OBJECT_STR:
        fprintf(out, "\"");
        fwrite(o.via.str.ptr, o.via.str.size, 1, out);
        fprintf(out, "\"");
        break;

    case DMSGPACK_OBJECT_BIN:
        fprintf(out, "\"");
        dmsgpack_object_bin_print(out, o.via.bin.ptr, o.via.bin.size);
        fprintf(out, "\"");
        break;

    case DMSGPACK_OBJECT_EXT:
#if defined(PRIi8)
        fprintf(out, "(ext: %" PRIi8 ")", o.via.ext.type);
#else
        fprintf(out, "(ext: %d)", (int)o.via.ext.type);
#endif
        fprintf(out, "\"");
        dmsgpack_object_bin_print(out, o.via.ext.ptr, o.via.ext.size);
        fprintf(out, "\"");
        break;

    case DMSGPACK_OBJECT_ARRAY:
        fprintf(out, "[");
        if(o.via.array.size != 0) {
            dmsgpack_object* p = o.via.array.ptr;
            dmsgpack_object* const pend = o.via.array.ptr + o.via.array.size;
            dmsgpack_object_print(out, *p);
            ++p;
            for(; p < pend; ++p) {
                fprintf(out, ", ");
                dmsgpack_object_print(out, *p);
            }
        }
        fprintf(out, "]");
        break;

    case DMSGPACK_OBJECT_MAP:
        fprintf(out, "{");
        if(o.via.map.size != 0) {
            dmsgpack_object_kv* p = o.via.map.ptr;
            dmsgpack_object_kv* const pend = o.via.map.ptr + o.via.map.size;
            dmsgpack_object_print(out, p->key);
            fprintf(out, "=>");
            dmsgpack_object_print(out, p->val);
            ++p;
            for(; p < pend; ++p) {
                fprintf(out, ", ");
                dmsgpack_object_print(out, p->key);
                fprintf(out, "=>");
                dmsgpack_object_print(out, p->val);
            }
        }
        fprintf(out, "}");
        break;

    default:
        // FIXME
#if defined(PRIu64)
        fprintf(out, "#<UNKNOWN %i %" PRIu64 ">", o.type, o.via.u64);
#else
        if (o.via.u64 > ULONG_MAX)
            fprintf(out, "#<UNKNOWN %i over 4294967295>", o.type);
        else
            fprintf(out, "#<UNKNOWN %i %lu>", o.type, (unsigned long)o.via.u64);
#endif

    }
}

#endif

#define DMSGPACK_CHECKED_CALL(ret, func, aux_buffer, aux_buffer_size, ...) \
    ret = func(aux_buffer, aux_buffer_size, __VA_ARGS__);                 \
    if (ret <= 0 || ret >= (int)aux_buffer_size) return 0;                \
    aux_buffer = aux_buffer + ret;                                        \
    aux_buffer_size = aux_buffer_size - ret                               \

static int dmsgpack_object_bin_print_buffer(char *buffer, size_t buffer_size, const char *ptr, size_t size)
{
    size_t i;
    char *aux_buffer = buffer;
    size_t aux_buffer_size = buffer_size;
    int ret;

    for (i = 0; i < size; ++i) {
        if (ptr[i] == '"') {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "\\\"");
        } else if (isprint((unsigned char)ptr[i])) {
            if (aux_buffer_size > 0) {
                memcpy(aux_buffer, ptr + i, 1);
                aux_buffer = aux_buffer + 1;
                aux_buffer_size = aux_buffer_size - 1;
            }
        } else {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "\\x%02x", (unsigned char)ptr[i]);
        }
    }

    return (int)(buffer_size - aux_buffer_size);
}

int dmsgpack_object_print_buffer(char *buffer, size_t buffer_size, dmsgpack_object o)
{
    char *aux_buffer = buffer;
    size_t aux_buffer_size = buffer_size;
    int ret;
    switch(o.type) {
    case DMSGPACK_OBJECT_NIL:
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "nil");
        break;

    case DMSGPACK_OBJECT_BOOLEAN:
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, (o.via.boolean ? "true" : "false"));
        break;

    case DMSGPACK_OBJECT_POSITIVE_INTEGER:
#if defined(PRIu64)
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "%" PRIu64, o.via.u64);
#else
        if (o.via.u64 > ULONG_MAX) {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "over 4294967295");
        } else {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "%lu", (unsigned long)o.via.u64);
        }
#endif
        break;

    case DMSGPACK_OBJECT_NEGATIVE_INTEGER:
#if defined(PRIi64)
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "%" PRIi64, o.via.i64);
#else
        if (o.via.i64 > LONG_MAX) {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "over +2147483647");
        } else if (o.via.i64 < LONG_MIN) {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "under -2147483648");
        } else {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "%ld", (signed long)o.via.i64);
        }
#endif
        break;

    case DMSGPACK_OBJECT_FLOAT32:
    case DMSGPACK_OBJECT_FLOAT64:
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "%f", o.via.f64);
        break;

    case DMSGPACK_OBJECT_STR:
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "\"");
        if (o.via.str.size > 0) {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "%.*s", (int)o.via.str.size, o.via.str.ptr);
        }
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "\"");
        break;

    case DMSGPACK_OBJECT_BIN:
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "\"");
        DMSGPACK_CHECKED_CALL(ret, dmsgpack_object_bin_print_buffer, aux_buffer, aux_buffer_size, o.via.bin.ptr, o.via.bin.size);
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "\"");
        break;

    case DMSGPACK_OBJECT_EXT:
#if defined(PRIi8)
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "(ext: %" PRIi8 ")", o.via.ext.type);
#else
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "(ext: %d)", (int)o.via.ext.type);
#endif
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "\"");
        DMSGPACK_CHECKED_CALL(ret, dmsgpack_object_bin_print_buffer, aux_buffer, aux_buffer_size, o.via.ext.ptr, o.via.ext.size);
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "\"");
        break;

    case DMSGPACK_OBJECT_ARRAY:
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "[");
        if(o.via.array.size != 0) {
            dmsgpack_object* p = o.via.array.ptr;
            dmsgpack_object* const pend = o.via.array.ptr + o.via.array.size;
            DMSGPACK_CHECKED_CALL(ret, dmsgpack_object_print_buffer, aux_buffer, aux_buffer_size, *p);
            ++p;
            for(; p < pend; ++p) {
                DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, ", ");
                DMSGPACK_CHECKED_CALL(ret, dmsgpack_object_print_buffer, aux_buffer, aux_buffer_size, *p);
            }
        }
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "]");
        break;

    case DMSGPACK_OBJECT_MAP:
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "{");
        if(o.via.map.size != 0) {
            dmsgpack_object_kv* p = o.via.map.ptr;
            dmsgpack_object_kv* const pend = o.via.map.ptr + o.via.map.size;
            DMSGPACK_CHECKED_CALL(ret, dmsgpack_object_print_buffer, aux_buffer, aux_buffer_size, p->key);
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "=>");
            DMSGPACK_CHECKED_CALL(ret, dmsgpack_object_print_buffer, aux_buffer, aux_buffer_size, p->val);
            ++p;
            for(; p < pend; ++p) {
                DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, ", ");
                DMSGPACK_CHECKED_CALL(ret, dmsgpack_object_print_buffer, aux_buffer, aux_buffer_size, p->key);
                DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "=>");
                DMSGPACK_CHECKED_CALL(ret, dmsgpack_object_print_buffer, aux_buffer, aux_buffer_size, p->val);
            }
        }
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "}");
        break;

    default:
    // FIXME
#if defined(PRIu64)
        DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "#<UNKNOWN %i %" PRIu64 ">", o.type, o.via.u64);
#else
        if (o.via.u64 > ULONG_MAX) {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "#<UNKNOWN %i over 4294967295>", o.type);
        } else {
            DMSGPACK_CHECKED_CALL(ret, snprintf, aux_buffer, aux_buffer_size, "#<UNKNOWN %i %lu>", o.type, (unsigned long)o.via.u64);
        }
#endif
    }

    return (int)(buffer_size - aux_buffer_size);
}

#undef DMSGPACK_CHECKED_CALL

bool dmsgpack_object_equal(const dmsgpack_object x, const dmsgpack_object y)
{
    if(x.type != y.type) { return false; }

    switch(x.type) {
    case DMSGPACK_OBJECT_NIL:
        return true;

    case DMSGPACK_OBJECT_BOOLEAN:
        return x.via.boolean == y.via.boolean;

    case DMSGPACK_OBJECT_POSITIVE_INTEGER:
        return x.via.u64 == y.via.u64;

    case DMSGPACK_OBJECT_NEGATIVE_INTEGER:
        return x.via.i64 == y.via.i64;

    case DMSGPACK_OBJECT_FLOAT32:
    case DMSGPACK_OBJECT_FLOAT64:
        return x.via.f64 == y.via.f64;

    case DMSGPACK_OBJECT_STR:
        return x.via.str.size == y.via.str.size &&
            memcmp(x.via.str.ptr, y.via.str.ptr, x.via.str.size) == 0;

    case DMSGPACK_OBJECT_BIN:
        return x.via.bin.size == y.via.bin.size &&
            memcmp(x.via.bin.ptr, y.via.bin.ptr, x.via.bin.size) == 0;

    case DMSGPACK_OBJECT_EXT:
        return x.via.ext.size == y.via.ext.size &&
            x.via.ext.type == y.via.ext.type &&
            memcmp(x.via.ext.ptr, y.via.ext.ptr, x.via.ext.size) == 0;

    case DMSGPACK_OBJECT_ARRAY:
        if(x.via.array.size != y.via.array.size) {
            return false;
        } else if(x.via.array.size == 0) {
            return true;
        } else {
            dmsgpack_object* px = x.via.array.ptr;
            dmsgpack_object* const pxend = x.via.array.ptr + x.via.array.size;
            dmsgpack_object* py = y.via.array.ptr;
            do {
                if(!dmsgpack_object_equal(*px, *py)) {
                    return false;
                }
                ++px;
                ++py;
            } while(px < pxend);
            return true;
        }

    case DMSGPACK_OBJECT_MAP:
        if(x.via.map.size != y.via.map.size) {
            return false;
        } else if(x.via.map.size == 0) {
            return true;
        } else {
            dmsgpack_object_kv* px = x.via.map.ptr;
            dmsgpack_object_kv* const pxend = x.via.map.ptr + x.via.map.size;
            dmsgpack_object_kv* py = y.via.map.ptr;
            do {
                if(!dmsgpack_object_equal(px->key, py->key) || !dmsgpack_object_equal(px->val, py->val)) {
                    return false;
                }
                ++px;
                ++py;
            } while(px < pxend);
            return true;
        }

    default:
        return false;
    }
}
