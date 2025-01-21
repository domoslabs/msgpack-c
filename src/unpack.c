/*
 * MessagePack for C unpacking routine
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#include "dmsgpack/unpack.h"
#include "dmsgpack/unpack_define.h"
#include "dmsgpack/util.h"
#include <stdlib.h>

#ifdef _dmsgpack_atomic_counter_header
#include _dmsgpack_atomic_counter_header
#endif


typedef struct {
    dmsgpack_zone** z;
    bool referenced;
} unpack_user;


#define dmsgpack_unpack_struct(name) \
    struct template ## name

#define dmsgpack_unpack_func(ret, name) \
    ret template ## name

#define dmsgpack_unpack_callback(name) \
    template_callback ## name

#define dmsgpack_unpack_object dmsgpack_object

#define dmsgpack_unpack_user unpack_user


struct template_context;
typedef struct template_context template_context;

static void template_init(template_context* ctx);

static dmsgpack_object template_data(template_context* ctx);

static int template_execute(
    template_context* ctx, const char* data, size_t len, size_t* off);


static inline dmsgpack_object template_callback_root(unpack_user* u)
{
    dmsgpack_object o;
    DMSGPACK_UNUSED(u);
    o.type = DMSGPACK_OBJECT_NIL;
    return o;
}

static inline int template_callback_uint8(unpack_user* u, uint8_t d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
    o->via.u64 = d;
    return 0;
}

static inline int template_callback_uint16(unpack_user* u, uint16_t d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
    o->via.u64 = d;
    return 0;
}

static inline int template_callback_uint32(unpack_user* u, uint32_t d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
    o->via.u64 = d;
    return 0;
}

static inline int template_callback_uint64(unpack_user* u, uint64_t d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
    o->via.u64 = d;
    return 0;
}

static inline int template_callback_int8(unpack_user* u, int8_t d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    if(d >= 0) {
        o->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
        o->via.u64 = (uint64_t)d;
        return 0;
    }
    else {
        o->type = DMSGPACK_OBJECT_NEGATIVE_INTEGER;
        o->via.i64 = d;
        return 0;
    }
}

static inline int template_callback_int16(unpack_user* u, int16_t d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    if(d >= 0) {
        o->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
        o->via.u64 = (uint64_t)d;
        return 0;
    }
    else {
        o->type = DMSGPACK_OBJECT_NEGATIVE_INTEGER;
        o->via.i64 = d;
        return 0;
    }
}

static inline int template_callback_int32(unpack_user* u, int32_t d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    if(d >= 0) {
        o->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
        o->via.u64 = (uint64_t)d;
        return 0;
    }
    else {
        o->type = DMSGPACK_OBJECT_NEGATIVE_INTEGER;
        o->via.i64 = d;
        return 0;
    }
}

static inline int template_callback_int64(unpack_user* u, int64_t d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    if(d >= 0) {
        o->type = DMSGPACK_OBJECT_POSITIVE_INTEGER;
        o->via.u64 = (uint64_t)d;
        return 0;
    }
    else {
        o->type = DMSGPACK_OBJECT_NEGATIVE_INTEGER;
        o->via.i64 = d;
        return 0;
    }
}

static inline int template_callback_float(unpack_user* u, float d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_FLOAT32;
    o->via.f64 = d;
    return 0;
}

static inline int template_callback_double(unpack_user* u, double d, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_FLOAT64;
    o->via.f64 = d;
    return 0;
}

static inline int template_callback_nil(unpack_user* u, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_NIL;
    return 0;
}

static inline int template_callback_true(unpack_user* u, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_BOOLEAN;
    o->via.boolean = true;
    return 0;
}

static inline int template_callback_false(unpack_user* u, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(u);
    o->type = DMSGPACK_OBJECT_BOOLEAN;
    o->via.boolean = false;
    return 0;
}

static inline int template_callback_array(unpack_user* u, unsigned int n, dmsgpack_object* o)
{
    size_t size;
    // Let's leverage the fact that sizeof(dmsgpack_object) is a compile time constant
    // to check for int overflows.
    // Note - while n is constrained to 32-bit, the product of n * sizeof(dmsgpack_object)
    // might not be constrained to 4GB on 64-bit systems
#if SIZE_MAX == UINT_MAX
    if (n > SIZE_MAX/sizeof(dmsgpack_object))
        return DMSGPACK_UNPACK_NOMEM_ERROR;
#endif

    o->type = DMSGPACK_OBJECT_ARRAY;
    o->via.array.size = 0;

    size = n * sizeof(dmsgpack_object);

    if (*u->z == NULL) {
        *u->z = dmsgpack_zone_new(DMSGPACK_ZONE_CHUNK_SIZE);
        if(*u->z == NULL) {
            return DMSGPACK_UNPACK_NOMEM_ERROR;
        }
    }

    // Unsure whether size = 0 should be an error, and if so, what to return
    o->via.array.ptr = (dmsgpack_object*)dmsgpack_zone_malloc(*u->z, size);
    if(o->via.array.ptr == NULL) { return DMSGPACK_UNPACK_NOMEM_ERROR; }
    return 0;
}

static inline int template_callback_array_item(unpack_user* u, dmsgpack_object* c, dmsgpack_object o)
{
    DMSGPACK_UNUSED(u);
#if defined(__GNUC__) && !defined(__clang__)
    memcpy(&c->via.array.ptr[c->via.array.size], &o, sizeof(dmsgpack_object));
#else  /* __GNUC__ && !__clang__ */
    c->via.array.ptr[c->via.array.size] = o;
#endif /* __GNUC__ && !__clang__ */
    ++c->via.array.size;
    return 0;
}

static inline int template_callback_map(unpack_user* u, unsigned int n, dmsgpack_object* o)
{
    size_t size;
    // Let's leverage the fact that sizeof(dmsgpack_object_kv) is a compile time constant
    // to check for int overflows
    // Note - while n is constrained to 32-bit, the product of n * sizeof(dmsgpack_object)
    // might not be constrained to 4GB on 64-bit systems

    // Note - this will always be false on 64-bit systems
#if SIZE_MAX == UINT_MAX
    if (n > SIZE_MAX/sizeof(dmsgpack_object_kv))
        return DMSGPACK_UNPACK_NOMEM_ERROR;
#endif

    o->type = DMSGPACK_OBJECT_MAP;
    o->via.map.size = 0;

    size = n * sizeof(dmsgpack_object_kv);

    if (*u->z == NULL) {
        *u->z = dmsgpack_zone_new(DMSGPACK_ZONE_CHUNK_SIZE);
        if(*u->z == NULL) {
            return DMSGPACK_UNPACK_NOMEM_ERROR;
        }
    }

    // Should size = 0 be an error? If so, what error to return?
    o->via.map.ptr = (dmsgpack_object_kv*)dmsgpack_zone_malloc(*u->z, size);
    if(o->via.map.ptr == NULL) { return DMSGPACK_UNPACK_NOMEM_ERROR; }
    return 0;
}

static inline int template_callback_map_item(unpack_user* u, dmsgpack_object* c, dmsgpack_object k, dmsgpack_object v)
{
    DMSGPACK_UNUSED(u);
#if defined(__GNUC__) && !defined(__clang__)
    memcpy(&c->via.map.ptr[c->via.map.size].key, &k, sizeof(dmsgpack_object));
    memcpy(&c->via.map.ptr[c->via.map.size].val, &v, sizeof(dmsgpack_object));
#else  /* __GNUC__ && !__clang__ */
    c->via.map.ptr[c->via.map.size].key = k;
    c->via.map.ptr[c->via.map.size].val = v;
#endif /* __GNUC__ && !__clang__ */
    ++c->via.map.size;
    return 0;
}

static inline int template_callback_str(unpack_user* u, const char* b, const char* p, unsigned int l, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(b);
    if (*u->z == NULL) {
        *u->z = dmsgpack_zone_new(DMSGPACK_ZONE_CHUNK_SIZE);
        if(*u->z == NULL) {
            return DMSGPACK_UNPACK_NOMEM_ERROR;
        }
    }
    o->type = DMSGPACK_OBJECT_STR;
    o->via.str.ptr = p;
    o->via.str.size = l;
    u->referenced = true;
    return 0;
}

static inline int template_callback_bin(unpack_user* u, const char* b, const char* p, unsigned int l, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(b);
    if (*u->z == NULL) {
        *u->z = dmsgpack_zone_new(DMSGPACK_ZONE_CHUNK_SIZE);
        if(*u->z == NULL) {
            return DMSGPACK_UNPACK_NOMEM_ERROR;
        }
    }
    o->type = DMSGPACK_OBJECT_BIN;
    o->via.bin.ptr = p;
    o->via.bin.size = l;
    u->referenced = true;
    return 0;
}

static inline int template_callback_ext(unpack_user* u, const char* b, const char* p, unsigned int l, dmsgpack_object* o)
{
    DMSGPACK_UNUSED(b);
    if (l == 0) {
        return DMSGPACK_UNPACK_PARSE_ERROR;
    }
    if (*u->z == NULL) {
        *u->z = dmsgpack_zone_new(DMSGPACK_ZONE_CHUNK_SIZE);
        if(*u->z == NULL) {
            return DMSGPACK_UNPACK_NOMEM_ERROR;
        }
    }
    o->type = DMSGPACK_OBJECT_EXT;
    o->via.ext.type = *p;
    o->via.ext.ptr = p + 1;
    o->via.ext.size = l - 1;
    u->referenced = true;
    return 0;
}

#include "dmsgpack/unpack_template.h"


#define CTX_CAST(m) ((template_context*)(m))
#define CTX_REFERENCED(mpac) CTX_CAST((mpac)->ctx)->user.referenced

#define COUNTER_SIZE (sizeof(_dmsgpack_atomic_counter_t))


static inline void init_count(void* buffer)
{
    *(volatile _dmsgpack_atomic_counter_t*)buffer = 1;
}

static inline void decr_count(void* buffer)
{
    // atomic if(--*(_dmsgpack_atomic_counter_t*)buffer == 0) { free(buffer); }
    if(_dmsgpack_sync_decr_and_fetch((volatile _dmsgpack_atomic_counter_t*)buffer) == 0) {
        free(buffer);
    }
}

static inline void incr_count(void* buffer)
{
    // atomic ++*(_dmsgpack_atomic_counter_t*)buffer;
    _dmsgpack_sync_incr_and_fetch((volatile _dmsgpack_atomic_counter_t*)buffer);
}

static inline _dmsgpack_atomic_counter_t get_count(void* buffer)
{
    return *(volatile _dmsgpack_atomic_counter_t*)buffer;
}

bool dmsgpack_unpacker_init(dmsgpack_unpacker* mpac, size_t initial_buffer_size)
{
    char* buffer;
    void* ctx;

    if(initial_buffer_size < COUNTER_SIZE) {
        initial_buffer_size = COUNTER_SIZE;
    }

    buffer = (char*)malloc(initial_buffer_size);
    if(buffer == NULL) {
        return false;
    }

    ctx = malloc(sizeof(template_context));
    if(ctx == NULL) {
        free(buffer);
        return false;
    }

    mpac->buffer = buffer;
    mpac->used = COUNTER_SIZE;
    mpac->free = initial_buffer_size - mpac->used;
    mpac->off = COUNTER_SIZE;
    mpac->parsed = 0;
    mpac->initial_buffer_size = initial_buffer_size;
    mpac->z = NULL;
    mpac->ctx = ctx;

    init_count(mpac->buffer);

    template_init(CTX_CAST(mpac->ctx));
    CTX_CAST(mpac->ctx)->user.z = &mpac->z;
    CTX_CAST(mpac->ctx)->user.referenced = false;

    return true;
}

void dmsgpack_unpacker_destroy(dmsgpack_unpacker* mpac)
{
    dmsgpack_zone_free(mpac->z);
    free(mpac->ctx);
    decr_count(mpac->buffer);
}

dmsgpack_unpacker* dmsgpack_unpacker_new(size_t initial_buffer_size)
{
    dmsgpack_unpacker* mpac = (dmsgpack_unpacker*)malloc(sizeof(dmsgpack_unpacker));
    if(mpac == NULL) {
        return NULL;
    }

    if(!dmsgpack_unpacker_init(mpac, initial_buffer_size)) {
        free(mpac);
        return NULL;
    }

    return mpac;
}

void dmsgpack_unpacker_free(dmsgpack_unpacker* mpac)
{
    dmsgpack_unpacker_destroy(mpac);
    free(mpac);
}

bool dmsgpack_unpacker_expand_buffer(dmsgpack_unpacker* mpac, size_t size)
{
    if(mpac->used == mpac->off && get_count(mpac->buffer) == 1
            && !CTX_REFERENCED(mpac)) {
        // rewind buffer
        mpac->free += mpac->used - COUNTER_SIZE;
        mpac->used = COUNTER_SIZE;
        mpac->off = COUNTER_SIZE;

        if(mpac->free >= size) {
            return true;
        }
    }

    if(mpac->off == COUNTER_SIZE) {
        char* tmp;
        size_t next_size = (mpac->used + mpac->free) * 2;  // include COUNTER_SIZE
        while(next_size < size + mpac->used) {
            size_t tmp_next_size = next_size * 2;
            if (tmp_next_size <= next_size) {
                next_size = size + mpac->used;
                break;
            }
            next_size = tmp_next_size;
        }

        tmp = (char*)realloc(mpac->buffer, next_size);
        if(tmp == NULL) {
            return false;
        }

        mpac->buffer = tmp;
        mpac->free = next_size - mpac->used;

    } else {
        char* tmp;
        size_t next_size = mpac->initial_buffer_size;  // include COUNTER_SIZE
        size_t not_parsed = mpac->used - mpac->off;
        while(next_size < size + not_parsed + COUNTER_SIZE) {
            size_t tmp_next_size = next_size * 2;
            if (tmp_next_size <= next_size) {
                next_size = size + not_parsed + COUNTER_SIZE;
                break;
            }
            next_size = tmp_next_size;
        }

        tmp = (char*)malloc(next_size);
        if(tmp == NULL) {
            return false;
        }

        init_count(tmp);

        memcpy(tmp+COUNTER_SIZE, mpac->buffer+mpac->off, not_parsed);

        if(CTX_REFERENCED(mpac)) {
            if(!dmsgpack_zone_push_finalizer(mpac->z, decr_count, mpac->buffer)) {
                free(tmp);
                return false;
            }
            CTX_REFERENCED(mpac) = false;
        } else {
            decr_count(mpac->buffer);
        }

        mpac->buffer = tmp;
        mpac->used = not_parsed + COUNTER_SIZE;
        mpac->free = next_size - mpac->used;
        mpac->off = COUNTER_SIZE;
    }

    return true;
}

int dmsgpack_unpacker_execute(dmsgpack_unpacker* mpac)
{
    size_t off = mpac->off;
    int ret = template_execute(CTX_CAST(mpac->ctx),
            mpac->buffer, mpac->used, &mpac->off);
    if(mpac->off > off) {
        mpac->parsed += mpac->off - off;
    }
    return ret;
}

dmsgpack_object dmsgpack_unpacker_data(dmsgpack_unpacker* mpac)
{
    return template_data(CTX_CAST(mpac->ctx));
}

dmsgpack_zone* dmsgpack_unpacker_release_zone(dmsgpack_unpacker* mpac)
{
    dmsgpack_zone* old = mpac->z;

    if (old == NULL) return NULL;
    if(!dmsgpack_unpacker_flush_zone(mpac)) {
        return NULL;
    }

    mpac->z = NULL;
    CTX_CAST(mpac->ctx)->user.z = &mpac->z;

    return old;
}

void dmsgpack_unpacker_reset_zone(dmsgpack_unpacker* mpac)
{
    dmsgpack_zone_clear(mpac->z);
}

bool dmsgpack_unpacker_flush_zone(dmsgpack_unpacker* mpac)
{
    if(CTX_REFERENCED(mpac)) {
        if(!dmsgpack_zone_push_finalizer(mpac->z, decr_count, mpac->buffer)) {
            return false;
        }
        CTX_REFERENCED(mpac) = false;

        incr_count(mpac->buffer);
    }

    return true;
}

void dmsgpack_unpacker_reset(dmsgpack_unpacker* mpac)
{
    template_init(CTX_CAST(mpac->ctx));
    // don't reset referenced flag
    mpac->parsed = 0;
}

static inline dmsgpack_unpack_return unpacker_next(dmsgpack_unpacker* mpac,
                                                  dmsgpack_unpacked* result)
{
    int ret;

    dmsgpack_unpacked_destroy(result);

    ret = dmsgpack_unpacker_execute(mpac);

    if(ret < 0) {
        result->zone = NULL;
        memset(&result->data, 0, sizeof(dmsgpack_object));
        return (dmsgpack_unpack_return)ret;
    }

    if(ret == 0) {
        return DMSGPACK_UNPACK_CONTINUE;
    }
    result->zone = dmsgpack_unpacker_release_zone(mpac);
    result->data = dmsgpack_unpacker_data(mpac);

    return DMSGPACK_UNPACK_SUCCESS;
}

dmsgpack_unpack_return dmsgpack_unpacker_next(dmsgpack_unpacker* mpac,
                                            dmsgpack_unpacked* result)
{
    dmsgpack_unpack_return ret;

    ret = unpacker_next(mpac, result);
    if (ret == DMSGPACK_UNPACK_SUCCESS) {
        dmsgpack_unpacker_reset(mpac);
    }

    return ret;
}

dmsgpack_unpack_return
dmsgpack_unpacker_next_with_size(dmsgpack_unpacker* mpac,
                                dmsgpack_unpacked* result, size_t *p_bytes)
{
    dmsgpack_unpack_return ret;

    ret = unpacker_next(mpac, result);
    if (ret == DMSGPACK_UNPACK_SUCCESS || ret == DMSGPACK_UNPACK_CONTINUE) {
        *p_bytes = mpac->parsed;
    }

    if (ret == DMSGPACK_UNPACK_SUCCESS) {
        dmsgpack_unpacker_reset(mpac);
    }

    return ret;
}

dmsgpack_unpack_return
dmsgpack_unpack(const char* data, size_t len, size_t* off,
        dmsgpack_zone* result_zone, dmsgpack_object* result)
{
    size_t noff = 0;
    if(off != NULL) { noff = *off; }

    if(len <= noff) {
        // FIXME
        return DMSGPACK_UNPACK_CONTINUE;
    }
    else {
        int e;
        template_context ctx;
        template_init(&ctx);

        ctx.user.z = &result_zone;
        ctx.user.referenced = false;

        e = template_execute(&ctx, data, len, &noff);
        if(e < 0) {
            return (dmsgpack_unpack_return)e;
        }

        if(off != NULL) { *off = noff; }

        if(e == 0) {
            return DMSGPACK_UNPACK_CONTINUE;
        }

        *result = template_data(&ctx);

        if(noff < len) {
            return DMSGPACK_UNPACK_EXTRA_BYTES;
        }

        return DMSGPACK_UNPACK_SUCCESS;
    }
}

dmsgpack_unpack_return
dmsgpack_unpack_next(dmsgpack_unpacked* result,
        const char* data, size_t len, size_t* off)
{
    size_t noff = 0;
    dmsgpack_unpacked_destroy(result);

    if(off != NULL) { noff = *off; }

    if(len <= noff) {
        return DMSGPACK_UNPACK_CONTINUE;
    }

    {
        int e;
        template_context ctx;
        template_init(&ctx);

        ctx.user.z = &result->zone;
        ctx.user.referenced = false;

        e = template_execute(&ctx, data, len, &noff);

        if(off != NULL) { *off = noff; }

        if(e < 0) {
            dmsgpack_zone_free(result->zone);
            result->zone = NULL;
            return (dmsgpack_unpack_return)e;
        }

        if(e == 0) {
            return DMSGPACK_UNPACK_CONTINUE;
        }

        result->data = template_data(&ctx);

        return DMSGPACK_UNPACK_SUCCESS;
    }
}

#if defined(DMSGPACK_OLD_COMPILER_BUS_ERROR_WORKAROUND)
// FIXME: Dirty hack to avoid a bus error caused by OS X's old gcc.
static void dummy_function_to_avoid_bus_error()
{
}
#endif
