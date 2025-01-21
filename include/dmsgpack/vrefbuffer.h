/*
 * MessagePack for C zero-copy buffer implementation
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_VREFBUFFER_H
#define DMSGPACK_VREFBUFFER_H

#include "zone.h"
#include <stdlib.h>
#include <assert.h>

#if defined(unix) || defined(__unix) || defined(__linux__) || defined(__APPLE__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__QNX__) || defined(__QNXTO__) || defined(__HAIKU__)
#include <sys/uio.h>
typedef struct iovec dmsgpack_iovec;
#else
struct dmsgpack_iovec {
    void  *iov_base;
    size_t iov_len;
};
typedef struct dmsgpack_iovec dmsgpack_iovec;
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup dmsgpack_vrefbuffer Vectored Referencing buffer
 * @ingroup dmsgpack_buffer
 * @{
 */

struct dmsgpack_vrefbuffer_chunk;
typedef struct dmsgpack_vrefbuffer_chunk dmsgpack_vrefbuffer_chunk;

typedef struct dmsgpack_vrefbuffer_inner_buffer {
    size_t free;
    char*  ptr;
    dmsgpack_vrefbuffer_chunk* head;
} dmsgpack_vrefbuffer_inner_buffer;

typedef struct dmsgpack_vrefbuffer {
    dmsgpack_iovec* tail;
    dmsgpack_iovec* end;
    dmsgpack_iovec* array;

    size_t chunk_size;
    size_t ref_size;

    dmsgpack_vrefbuffer_inner_buffer inner_buffer;
} dmsgpack_vrefbuffer;


#ifndef DMSGPACK_VREFBUFFER_REF_SIZE
#define DMSGPACK_VREFBUFFER_REF_SIZE 32
#endif

#ifndef DMSGPACK_VREFBUFFER_CHUNK_SIZE
#define DMSGPACK_VREFBUFFER_CHUNK_SIZE 8192
#endif

DMSGPACK_DLLEXPORT
bool dmsgpack_vrefbuffer_init(dmsgpack_vrefbuffer* vbuf,
        size_t ref_size, size_t chunk_size);
DMSGPACK_DLLEXPORT
void dmsgpack_vrefbuffer_destroy(dmsgpack_vrefbuffer* vbuf);

static inline dmsgpack_vrefbuffer* dmsgpack_vrefbuffer_new(size_t ref_size, size_t chunk_size);
static inline void dmsgpack_vrefbuffer_free(dmsgpack_vrefbuffer* vbuf);

static inline int dmsgpack_vrefbuffer_write(void* data, const char* buf, size_t len);

static inline const dmsgpack_iovec* dmsgpack_vrefbuffer_vec(const dmsgpack_vrefbuffer* vref);
static inline size_t dmsgpack_vrefbuffer_veclen(const dmsgpack_vrefbuffer* vref);

DMSGPACK_DLLEXPORT
int dmsgpack_vrefbuffer_append_copy(dmsgpack_vrefbuffer* vbuf,
        const char* buf, size_t len);

DMSGPACK_DLLEXPORT
int dmsgpack_vrefbuffer_append_ref(dmsgpack_vrefbuffer* vbuf,
        const char* buf, size_t len);

DMSGPACK_DLLEXPORT
int dmsgpack_vrefbuffer_migrate(dmsgpack_vrefbuffer* vbuf, dmsgpack_vrefbuffer* to);

DMSGPACK_DLLEXPORT
void dmsgpack_vrefbuffer_clear(dmsgpack_vrefbuffer* vref);

/** @} */


static inline dmsgpack_vrefbuffer* dmsgpack_vrefbuffer_new(size_t ref_size, size_t chunk_size)
{
    dmsgpack_vrefbuffer* vbuf = (dmsgpack_vrefbuffer*)malloc(sizeof(dmsgpack_vrefbuffer));
    if (vbuf == NULL) return NULL;
    if(!dmsgpack_vrefbuffer_init(vbuf, ref_size, chunk_size)) {
        free(vbuf);
        return NULL;
    }
    return vbuf;
}

static inline void dmsgpack_vrefbuffer_free(dmsgpack_vrefbuffer* vbuf)
{
    if(vbuf == NULL) { return; }
    dmsgpack_vrefbuffer_destroy(vbuf);
    free(vbuf);
}

static inline int dmsgpack_vrefbuffer_write(void* data, const char* buf, size_t len)
{
    dmsgpack_vrefbuffer* vbuf = (dmsgpack_vrefbuffer*)data;
    assert(buf || len == 0);

    if(!buf) return 0;

    if(len < vbuf->ref_size) {
        return dmsgpack_vrefbuffer_append_copy(vbuf, buf, len);
    } else {
        return dmsgpack_vrefbuffer_append_ref(vbuf, buf, len);
    }
}

static inline const dmsgpack_iovec* dmsgpack_vrefbuffer_vec(const dmsgpack_vrefbuffer* vref)
{
    return vref->array;
}

static inline size_t dmsgpack_vrefbuffer_veclen(const dmsgpack_vrefbuffer* vref)
{
    return (size_t)(vref->tail - vref->array);
}


#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/vrefbuffer.h */
