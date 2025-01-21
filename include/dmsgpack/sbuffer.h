/*
 * MessagePack for C simple buffer implementation
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_SBUFFER_H
#define DMSGPACK_SBUFFER_H

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup dmsgpack_sbuffer Simple buffer
 * @ingroup dmsgpack_buffer
 * @{
 */

typedef struct dmsgpack_sbuffer {
    size_t size;
    char* data;
    size_t alloc;
} dmsgpack_sbuffer;

static inline void dmsgpack_sbuffer_init(dmsgpack_sbuffer* sbuf)
{
    memset(sbuf, 0, sizeof(dmsgpack_sbuffer));
}

static inline void dmsgpack_sbuffer_destroy(dmsgpack_sbuffer* sbuf)
{
    free(sbuf->data);
}

static inline dmsgpack_sbuffer* dmsgpack_sbuffer_new(void)
{
    return (dmsgpack_sbuffer*)calloc(1, sizeof(dmsgpack_sbuffer));
}

static inline void dmsgpack_sbuffer_free(dmsgpack_sbuffer* sbuf)
{
    if(sbuf == NULL) { return; }
    dmsgpack_sbuffer_destroy(sbuf);
    free(sbuf);
}

#ifndef DMSGPACK_SBUFFER_INIT_SIZE
#define DMSGPACK_SBUFFER_INIT_SIZE 8192
#endif

static inline int dmsgpack_sbuffer_write(void* data, const char* buf, size_t len)
{
    dmsgpack_sbuffer* sbuf = (dmsgpack_sbuffer*)data;

    assert(buf || len == 0);
    if(!buf) return 0;

    if(sbuf->alloc - sbuf->size < len) {
        void* tmp;
        size_t nsize = (sbuf->alloc) ?
                sbuf->alloc * 2 : DMSGPACK_SBUFFER_INIT_SIZE;

        while(nsize < sbuf->size + len) {
            size_t tmp_nsize = nsize * 2;
            if (tmp_nsize <= nsize) {
                nsize = sbuf->size + len;
                break;
            }
            nsize = tmp_nsize;
        }

        tmp = realloc(sbuf->data, nsize);
        if(!tmp) { return -1; }

        sbuf->data = (char*)tmp;
        sbuf->alloc = nsize;
    }

    memcpy(sbuf->data + sbuf->size, buf, len);
    sbuf->size += len;

    return 0;
}

static inline char* dmsgpack_sbuffer_release(dmsgpack_sbuffer* sbuf)
{
    char* tmp = sbuf->data;
    sbuf->size = 0;
    sbuf->data = NULL;
    sbuf->alloc = 0;
    return tmp;
}

static inline void dmsgpack_sbuffer_clear(dmsgpack_sbuffer* sbuf)
{
    sbuf->size = 0;
}

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/sbuffer.h */
