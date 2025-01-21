/*
 * MessagePack for C memory pool implementation
 *
 * Copyright (C) 2008-2010 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_ZONE_H
#define DMSGPACK_ZONE_H

#include "sysdep.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup dmsgpack_zone Memory zone
 * @ingroup dmsgpack
 * @{
 */

typedef struct dmsgpack_zone_finalizer {
    void (*func)(void* data);
    void* data;
} dmsgpack_zone_finalizer;

typedef struct dmsgpack_zone_finalizer_array {
    dmsgpack_zone_finalizer* tail;
    dmsgpack_zone_finalizer* end;
    dmsgpack_zone_finalizer* array;
} dmsgpack_zone_finalizer_array;

struct dmsgpack_zone_chunk;
typedef struct dmsgpack_zone_chunk dmsgpack_zone_chunk;

typedef struct dmsgpack_zone_chunk_list {
    size_t free;
    char* ptr;
    dmsgpack_zone_chunk* head;
} dmsgpack_zone_chunk_list;

typedef struct dmsgpack_zone {
    dmsgpack_zone_chunk_list chunk_list;
    dmsgpack_zone_finalizer_array finalizer_array;
    size_t chunk_size;
} dmsgpack_zone;

#ifndef DMSGPACK_ZONE_CHUNK_SIZE
#define DMSGPACK_ZONE_CHUNK_SIZE 8192
#endif

DMSGPACK_DLLEXPORT
bool dmsgpack_zone_init(dmsgpack_zone* zone, size_t chunk_size);
DMSGPACK_DLLEXPORT
void dmsgpack_zone_destroy(dmsgpack_zone* zone);

DMSGPACK_DLLEXPORT
dmsgpack_zone* dmsgpack_zone_new(size_t chunk_size);
DMSGPACK_DLLEXPORT
void dmsgpack_zone_free(dmsgpack_zone* zone);

static inline void* dmsgpack_zone_malloc(dmsgpack_zone* zone, size_t size);
static inline void* dmsgpack_zone_malloc_no_align(dmsgpack_zone* zone, size_t size);

static inline bool dmsgpack_zone_push_finalizer(dmsgpack_zone* zone,
        void (*func)(void* data), void* data);

static inline void dmsgpack_zone_swap(dmsgpack_zone* a, dmsgpack_zone* b);

DMSGPACK_DLLEXPORT
bool dmsgpack_zone_is_empty(dmsgpack_zone* zone);

DMSGPACK_DLLEXPORT
void dmsgpack_zone_clear(dmsgpack_zone* zone);

/** @} */


#ifndef DMSGPACK_ZONE_ALIGN
#define DMSGPACK_ZONE_ALIGN sizeof(void*)
#endif

DMSGPACK_DLLEXPORT
void* dmsgpack_zone_malloc_expand(dmsgpack_zone* zone, size_t size);

static inline void* dmsgpack_zone_malloc_no_align(dmsgpack_zone* zone, size_t size)
{
    char* ptr;
    dmsgpack_zone_chunk_list* cl = &zone->chunk_list;

    if(zone->chunk_list.free < size) {
        return dmsgpack_zone_malloc_expand(zone, size);
    }

    ptr = cl->ptr;
    cl->free -= size;
    cl->ptr  += size;

    return ptr;
}

static inline void* dmsgpack_zone_malloc(dmsgpack_zone* zone, size_t size)
{
    char* aligned =
        (char*)(
            (uintptr_t)(
                zone->chunk_list.ptr + (DMSGPACK_ZONE_ALIGN - 1)
            ) & ~(uintptr_t)(DMSGPACK_ZONE_ALIGN - 1)
        );
    size_t adjusted_size = size + (size_t)(aligned - zone->chunk_list.ptr);
    if(zone->chunk_list.free >= adjusted_size) {
        zone->chunk_list.free -= adjusted_size;
        zone->chunk_list.ptr  += adjusted_size;
        return aligned;
    }
    {
        void* ptr = dmsgpack_zone_malloc_expand(zone, size + (DMSGPACK_ZONE_ALIGN - 1));
        if (ptr) {
            return (char*)((uintptr_t)(ptr) & ~(uintptr_t)(DMSGPACK_ZONE_ALIGN - 1));
        }
    }
    return NULL;
}


bool dmsgpack_zone_push_finalizer_expand(dmsgpack_zone* zone,
        void (*func)(void* data), void* data);

static inline bool dmsgpack_zone_push_finalizer(dmsgpack_zone* zone,
        void (*func)(void* data), void* data)
{
    dmsgpack_zone_finalizer_array* const fa = &zone->finalizer_array;
    dmsgpack_zone_finalizer* fin = fa->tail;

    if(fin == fa->end) {
        return dmsgpack_zone_push_finalizer_expand(zone, func, data);
    }

    fin->func = func;
    fin->data = data;

    ++fa->tail;

    return true;
}

static inline void dmsgpack_zone_swap(dmsgpack_zone* a, dmsgpack_zone* b)
{
    dmsgpack_zone tmp = *a;
    *a = *b;
    *b = tmp;
}


#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/zone.h */
