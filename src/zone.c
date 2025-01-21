/*
 * MessagePack for C memory pool implementation
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#include "dmsgpack/zone.h"
#include <stdlib.h>
#include <string.h>

struct dmsgpack_zone_chunk {
    struct dmsgpack_zone_chunk* next;
    /* data ... */
};

static inline bool init_chunk_list(dmsgpack_zone_chunk_list* cl, size_t chunk_size)
{
    dmsgpack_zone_chunk* chunk = (dmsgpack_zone_chunk*)malloc(
            sizeof(dmsgpack_zone_chunk) + chunk_size);
    if(chunk == NULL) {
        return false;
    }

    cl->head = chunk;
    cl->free = chunk_size;
    cl->ptr  = ((char*)chunk) + sizeof(dmsgpack_zone_chunk);
    chunk->next = NULL;

    return true;
}

static inline void destroy_chunk_list(dmsgpack_zone_chunk_list* cl)
{
    dmsgpack_zone_chunk* c = cl->head;
    while(true) {
        dmsgpack_zone_chunk* n = c->next;
        free(c);
        if(n != NULL) {
            c = n;
        } else {
            break;
        }
    }
}

static inline void clear_chunk_list(dmsgpack_zone_chunk_list* cl, size_t chunk_size)
{
    dmsgpack_zone_chunk* c = cl->head;
    while(true) {
        dmsgpack_zone_chunk* n = c->next;
        if(n != NULL) {
            free(c);
            c = n;
        } else {
            cl->head = c;
            break;
        }
    }
    cl->head->next = NULL;
    cl->free = chunk_size;
    cl->ptr  = ((char*)cl->head) + sizeof(dmsgpack_zone_chunk);
}

void* dmsgpack_zone_malloc_expand(dmsgpack_zone* zone, size_t size)
{
    dmsgpack_zone_chunk_list* const cl = &zone->chunk_list;
    dmsgpack_zone_chunk* chunk;

    size_t sz = zone->chunk_size;

    while(sz < size) {
        size_t tmp_sz = sz * 2;
        if (tmp_sz <= sz) {
            sz = size;
            break;
        }
        sz = tmp_sz;
    }

    chunk = (dmsgpack_zone_chunk*)malloc(
            sizeof(dmsgpack_zone_chunk) + sz);
    if (chunk == NULL) {
        return NULL;
    }
    else {
        char* ptr = ((char*)chunk) + sizeof(dmsgpack_zone_chunk);
        chunk->next = cl->head;
        cl->head = chunk;
        cl->free = sz - size;
        cl->ptr  = ptr + size;

        return ptr;
    }
}


static inline void init_finalizer_array(dmsgpack_zone_finalizer_array* fa)
{
    fa->tail  = NULL;
    fa->end   = NULL;
    fa->array = NULL;
}

static inline void call_finalizer_array(dmsgpack_zone_finalizer_array* fa)
{
    dmsgpack_zone_finalizer* fin = fa->tail;
    for(; fin != fa->array; --fin) {
        (*(fin-1)->func)((fin-1)->data);
    }
}

static inline void destroy_finalizer_array(dmsgpack_zone_finalizer_array* fa)
{
    call_finalizer_array(fa);
    free(fa->array);
}

static inline void clear_finalizer_array(dmsgpack_zone_finalizer_array* fa)
{
    call_finalizer_array(fa);
    fa->tail = fa->array;
}

bool dmsgpack_zone_push_finalizer_expand(dmsgpack_zone* zone,
        void (*func)(void* data), void* data)
{
    dmsgpack_zone_finalizer_array* const fa = &zone->finalizer_array;
    dmsgpack_zone_finalizer* tmp;

    const size_t nused = (size_t)(fa->end - fa->array);

    size_t nnext;
    if(nused == 0) {
        nnext = (sizeof(dmsgpack_zone_finalizer) < 72/2) ?
                72 / sizeof(dmsgpack_zone_finalizer) : 8;

    } else {
        nnext = nused * 2;
    }

    tmp = (dmsgpack_zone_finalizer*)realloc(fa->array,
                sizeof(dmsgpack_zone_finalizer) * nnext);
    if(tmp == NULL) {
        return false;
    }

    fa->array  = tmp;
    fa->end    = tmp + nnext;
    fa->tail   = tmp + nused;

    fa->tail->func = func;
    fa->tail->data = data;

    ++fa->tail;

    return true;
}


bool dmsgpack_zone_is_empty(dmsgpack_zone* zone)
{
    dmsgpack_zone_chunk_list* const cl = &zone->chunk_list;
    dmsgpack_zone_finalizer_array* const fa = &zone->finalizer_array;
    return cl->free == zone->chunk_size && cl->head->next == NULL &&
        fa->tail == fa->array;
}


void dmsgpack_zone_destroy(dmsgpack_zone* zone)
{
    destroy_finalizer_array(&zone->finalizer_array);
    destroy_chunk_list(&zone->chunk_list);
}

void dmsgpack_zone_clear(dmsgpack_zone* zone)
{
    clear_finalizer_array(&zone->finalizer_array);
    clear_chunk_list(&zone->chunk_list, zone->chunk_size);
}

bool dmsgpack_zone_init(dmsgpack_zone* zone, size_t chunk_size)
{
    zone->chunk_size = chunk_size;

    if(!init_chunk_list(&zone->chunk_list, chunk_size)) {
        return false;
    }

    init_finalizer_array(&zone->finalizer_array);

    return true;
}

dmsgpack_zone* dmsgpack_zone_new(size_t chunk_size)
{
    dmsgpack_zone* zone = (dmsgpack_zone*)malloc(
            sizeof(dmsgpack_zone));
    if(zone == NULL) {
        return NULL;
    }

    zone->chunk_size = chunk_size;

    if(!init_chunk_list(&zone->chunk_list, chunk_size)) {
        free(zone);
        return NULL;
    }

    init_finalizer_array(&zone->finalizer_array);

    return zone;
}

void dmsgpack_zone_free(dmsgpack_zone* zone)
{
    if(zone == NULL) { return; }
    dmsgpack_zone_destroy(zone);
    free(zone);
}
