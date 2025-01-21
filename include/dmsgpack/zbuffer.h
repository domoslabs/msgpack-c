/*
 * MessagePack for C deflate buffer implementation
 *
 * Copyright (C) 2010 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_ZBUFFER_H
#define DMSGPACK_ZBUFFER_H

#include "sysdep.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup dmsgpack_zbuffer Compressed buffer
 * @ingroup dmsgpack_buffer
 * @{
 */

typedef struct dmsgpack_zbuffer {
    z_stream stream;
    char* data;
    size_t init_size;
} dmsgpack_zbuffer;

#ifndef DMSGPACK_ZBUFFER_INIT_SIZE
#define DMSGPACK_ZBUFFER_INIT_SIZE 8192
#endif

static inline bool dmsgpack_zbuffer_init(
    dmsgpack_zbuffer* zbuf, int level, size_t init_size);
static inline void dmsgpack_zbuffer_destroy(dmsgpack_zbuffer* zbuf);

static inline dmsgpack_zbuffer* dmsgpack_zbuffer_new(int level, size_t init_size);
static inline void dmsgpack_zbuffer_free(dmsgpack_zbuffer* zbuf);

static inline char* dmsgpack_zbuffer_flush(dmsgpack_zbuffer* zbuf);

static inline const char* dmsgpack_zbuffer_data(const dmsgpack_zbuffer* zbuf);
static inline size_t dmsgpack_zbuffer_size(const dmsgpack_zbuffer* zbuf);

static inline bool dmsgpack_zbuffer_reset(dmsgpack_zbuffer* zbuf);
static inline void dmsgpack_zbuffer_reset_buffer(dmsgpack_zbuffer* zbuf);
static inline char* dmsgpack_zbuffer_release_buffer(dmsgpack_zbuffer* zbuf);


#ifndef DMSGPACK_ZBUFFER_RESERVE_SIZE
#define DMSGPACK_ZBUFFER_RESERVE_SIZE 512
#endif

static inline int dmsgpack_zbuffer_write(void* data, const char* buf, size_t len);

static inline bool dmsgpack_zbuffer_expand(dmsgpack_zbuffer* zbuf);


static inline bool dmsgpack_zbuffer_init(dmsgpack_zbuffer* zbuf,
        int level, size_t init_size)
{
    memset(zbuf, 0, sizeof(dmsgpack_zbuffer));
    zbuf->init_size = init_size;
    if(deflateInit(&zbuf->stream, level) != Z_OK) {
        free(zbuf->data);
        return false;
    }
    return true;
}

static inline void dmsgpack_zbuffer_destroy(dmsgpack_zbuffer* zbuf)
{
    deflateEnd(&zbuf->stream);
    free(zbuf->data);
}

static inline dmsgpack_zbuffer* dmsgpack_zbuffer_new(int level, size_t init_size)
{
    dmsgpack_zbuffer* zbuf = (dmsgpack_zbuffer*)malloc(sizeof(dmsgpack_zbuffer));
    if (zbuf == NULL) return NULL;
    if(!dmsgpack_zbuffer_init(zbuf, level, init_size)) {
        free(zbuf);
        return NULL;
    }
    return zbuf;
}

static inline void dmsgpack_zbuffer_free(dmsgpack_zbuffer* zbuf)
{
    if(zbuf == NULL) { return; }
    dmsgpack_zbuffer_destroy(zbuf);
    free(zbuf);
}

static inline bool dmsgpack_zbuffer_expand(dmsgpack_zbuffer* zbuf)
{
    size_t used = (size_t)((char *)(zbuf->stream.next_out) - zbuf->data);
    size_t csize = used + zbuf->stream.avail_out;

    size_t nsize = (csize == 0) ? zbuf->init_size : csize * 2;

    char* tmp = (char*)realloc(zbuf->data, nsize);
    if(tmp == NULL) {
        return false;
    }

    zbuf->data = tmp;
    zbuf->stream.next_out  = (Bytef*)(tmp + used);
    zbuf->stream.avail_out = (uInt)(nsize - used);

    return true;
}

static inline int dmsgpack_zbuffer_write(void* data, const char* buf, size_t len)
{
    dmsgpack_zbuffer* zbuf = (dmsgpack_zbuffer*)data;

    assert(buf || len == 0);
    if(!buf) return 0;

    zbuf->stream.next_in = (Bytef*)buf;
    zbuf->stream.avail_in = (uInt)len;

    while(zbuf->stream.avail_in > 0) {
        if(zbuf->stream.avail_out < DMSGPACK_ZBUFFER_RESERVE_SIZE) {
            if(!dmsgpack_zbuffer_expand(zbuf)) {
                return -1;
            }
        }

        if(deflate(&zbuf->stream, Z_NO_FLUSH) != Z_OK) {
            return -1;
        }
    }

    return 0;
}

static inline char* dmsgpack_zbuffer_flush(dmsgpack_zbuffer* zbuf)
{
    while(true) {
        switch(deflate(&zbuf->stream, Z_FINISH)) {
        case Z_STREAM_END:
            return zbuf->data;
        case Z_OK:
        case Z_BUF_ERROR:
            if(!dmsgpack_zbuffer_expand(zbuf)) {
                return NULL;
            }
            break;
        default:
            return NULL;
        }
    }
}

static inline const char* dmsgpack_zbuffer_data(const dmsgpack_zbuffer* zbuf)
{
    return zbuf->data;
}

static inline size_t dmsgpack_zbuffer_size(const dmsgpack_zbuffer* zbuf)
{
    return (size_t)((char *)(zbuf->stream.next_out) - zbuf->data);
}

static inline void dmsgpack_zbuffer_reset_buffer(dmsgpack_zbuffer* zbuf)
{
    zbuf->stream.avail_out += (uInt)((char*)zbuf->stream.next_out - zbuf->data);
    zbuf->stream.next_out = (Bytef*)zbuf->data;
}

static inline bool dmsgpack_zbuffer_reset(dmsgpack_zbuffer* zbuf)
{
    if(deflateReset(&zbuf->stream) != Z_OK) {
        return false;
    }
    dmsgpack_zbuffer_reset_buffer(zbuf);
    return true;
}

static inline char* dmsgpack_zbuffer_release_buffer(dmsgpack_zbuffer* zbuf)
{
    char* tmp = zbuf->data;
    zbuf->data = NULL;
    zbuf->stream.next_out = NULL;
    zbuf->stream.avail_out = 0;
    return tmp;
}

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/zbuffer.h */
