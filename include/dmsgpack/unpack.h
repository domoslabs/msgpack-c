/*
 * MessagePack for C unpacking routine
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_UNPACKER_H
#define DMSGPACK_UNPACKER_H

#include "zone.h"
#include "object.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @defgroup dmsgpack_unpack Deserializer
 * @ingroup dmsgpack
 * @{
 */

typedef struct dmsgpack_unpacked {
    dmsgpack_zone* zone;
    dmsgpack_object data;
} dmsgpack_unpacked;

typedef enum {
    DMSGPACK_UNPACK_SUCCESS              =  2,
    DMSGPACK_UNPACK_EXTRA_BYTES          =  1,
    DMSGPACK_UNPACK_CONTINUE             =  0,
    DMSGPACK_UNPACK_PARSE_ERROR          = -1,
    DMSGPACK_UNPACK_NOMEM_ERROR          = -2
} dmsgpack_unpack_return;


DMSGPACK_DLLEXPORT
dmsgpack_unpack_return
dmsgpack_unpack_next(dmsgpack_unpacked* result,
        const char* data, size_t len, size_t* off);

/** @} */


/**
 * @defgroup dmsgpack_unpacker Streaming deserializer
 * @ingroup dmsgpack
 * @{
 */

typedef struct dmsgpack_unpacker {
    char* buffer;
    size_t used;
    size_t free;
    size_t off;
    size_t parsed;
    dmsgpack_zone* z;
    size_t initial_buffer_size;
    void* ctx;
} dmsgpack_unpacker;


#ifndef DMSGPACK_UNPACKER_INIT_BUFFER_SIZE
#define DMSGPACK_UNPACKER_INIT_BUFFER_SIZE (64*1024)
#endif

/**
 * Initializes a streaming deserializer.
 * The initialized deserializer must be destroyed by dmsgpack_unpacker_destroy(dmsgpack_unpacker*).
 */
DMSGPACK_DLLEXPORT
bool dmsgpack_unpacker_init(dmsgpack_unpacker* mpac, size_t initial_buffer_size);

/**
 * Destroys a streaming deserializer initialized by dmsgpack_unpacker_init(dmsgpack_unpacker*, size_t).
 */
DMSGPACK_DLLEXPORT
void dmsgpack_unpacker_destroy(dmsgpack_unpacker* mpac);


/**
 * Creates a streaming deserializer.
 * The created deserializer must be destroyed by dmsgpack_unpacker_free(dmsgpack_unpacker*).
 */
DMSGPACK_DLLEXPORT
dmsgpack_unpacker* dmsgpack_unpacker_new(size_t initial_buffer_size);

/**
 * Frees a streaming deserializer created by dmsgpack_unpacker_new(size_t).
 */
DMSGPACK_DLLEXPORT
void dmsgpack_unpacker_free(dmsgpack_unpacker* mpac);


#ifndef DMSGPACK_UNPACKER_RESERVE_SIZE
#define DMSGPACK_UNPACKER_RESERVE_SIZE (32*1024)
#endif

/**
 * Reserves free space of the internal buffer.
 * Use this function to fill the internal buffer with
 * dmsgpack_unpacker_buffer(dmsgpack_unpacker*),
 * dmsgpack_unpacker_buffer_capacity(const dmsgpack_unpacker*) and
 * dmsgpack_unpacker_buffer_consumed(dmsgpack_unpacker*).
 */
static inline bool   dmsgpack_unpacker_reserve_buffer(dmsgpack_unpacker* mpac, size_t size);

/**
 * Gets pointer to the free space of the internal buffer.
 * Use this function to fill the internal buffer with
 * dmsgpack_unpacker_reserve_buffer(dmsgpack_unpacker*, size_t),
 * dmsgpack_unpacker_buffer_capacity(const dmsgpack_unpacker*) and
 * dmsgpack_unpacker_buffer_consumed(dmsgpack_unpacker*).
 */
static inline char*  dmsgpack_unpacker_buffer(dmsgpack_unpacker* mpac);

/**
 * Gets size of the free space of the internal buffer.
 * Use this function to fill the internal buffer with
 * dmsgpack_unpacker_reserve_buffer(dmsgpack_unpacker*, size_t),
 * dmsgpack_unpacker_buffer(const dmsgpack_unpacker*) and
 * dmsgpack_unpacker_buffer_consumed(dmsgpack_unpacker*).
 */
static inline size_t dmsgpack_unpacker_buffer_capacity(const dmsgpack_unpacker* mpac);

/**
 * Notifies the deserializer that the internal buffer filled.
 * Use this function to fill the internal buffer with
 * dmsgpack_unpacker_reserve_buffer(dmsgpack_unpacker*, size_t),
 * dmsgpack_unpacker_buffer(dmsgpack_unpacker*) and
 * dmsgpack_unpacker_buffer_capacity(const dmsgpack_unpacker*).
 */
static inline void   dmsgpack_unpacker_buffer_consumed(dmsgpack_unpacker* mpac, size_t size);


/**
 * Deserializes one object.
 * Returns true if it successes. Otherwise false is returned.
 * @param pac  pointer to an initialized dmsgpack_unpacked object.
 */
DMSGPACK_DLLEXPORT
dmsgpack_unpack_return dmsgpack_unpacker_next(dmsgpack_unpacker* mpac, dmsgpack_unpacked* pac);

/**
 * Deserializes one object and set the number of parsed bytes involved.
 * Returns true if it successes. Otherwise false is returned.
 * @param mpac    pointer to an initialized dmsgpack_unpacker object.
 * @param result  pointer to an initialized dmsgpack_unpacked object.
 * @param p_bytes pointer to variable that will be set with the number of parsed bytes.
 */
DMSGPACK_DLLEXPORT
dmsgpack_unpack_return dmsgpack_unpacker_next_with_size(dmsgpack_unpacker* mpac,
                                                      dmsgpack_unpacked* result,
                                                      size_t *p_bytes);

/**
 * Initializes a dmsgpack_unpacked object.
 * The initialized object must be destroyed by dmsgpack_unpacked_destroy(dmsgpack_unpacker*).
 * Use the object with dmsgpack_unpacker_next(dmsgpack_unpacker*, dmsgpack_unpacked*) or
 * dmsgpack_unpack_next(dmsgpack_unpacked*, const char*, size_t, size_t*).
 */
static inline void dmsgpack_unpacked_init(dmsgpack_unpacked* result);

/**
 * Destroys a streaming deserializer initialized by dmsgpack_unpacked().
 */
static inline void dmsgpack_unpacked_destroy(dmsgpack_unpacked* result);

/**
 * Releases the memory zone from dmsgpack_unpacked object.
 * The released zone must be freed by dmsgpack_zone_free(dmsgpack_zone*).
 */
static inline dmsgpack_zone* dmsgpack_unpacked_release_zone(dmsgpack_unpacked* result);


DMSGPACK_DLLEXPORT
int dmsgpack_unpacker_execute(dmsgpack_unpacker* mpac);

DMSGPACK_DLLEXPORT
dmsgpack_object dmsgpack_unpacker_data(dmsgpack_unpacker* mpac);

DMSGPACK_DLLEXPORT
dmsgpack_zone* dmsgpack_unpacker_release_zone(dmsgpack_unpacker* mpac);

DMSGPACK_DLLEXPORT
void dmsgpack_unpacker_reset_zone(dmsgpack_unpacker* mpac);

DMSGPACK_DLLEXPORT
void dmsgpack_unpacker_reset(dmsgpack_unpacker* mpac);

static inline size_t dmsgpack_unpacker_message_size(const dmsgpack_unpacker* mpac);


/** @} */


// obsolete
DMSGPACK_DLLEXPORT
dmsgpack_unpack_return
dmsgpack_unpack(const char* data, size_t len, size_t* off,
        dmsgpack_zone* result_zone, dmsgpack_object* result);




static inline size_t dmsgpack_unpacker_parsed_size(const dmsgpack_unpacker* mpac);

DMSGPACK_DLLEXPORT
bool dmsgpack_unpacker_flush_zone(dmsgpack_unpacker* mpac);

DMSGPACK_DLLEXPORT
bool dmsgpack_unpacker_expand_buffer(dmsgpack_unpacker* mpac, size_t size);

static inline bool dmsgpack_unpacker_reserve_buffer(dmsgpack_unpacker* mpac, size_t size)
{
    if(mpac->free >= size) { return true; }
    return dmsgpack_unpacker_expand_buffer(mpac, size);
}

static inline char* dmsgpack_unpacker_buffer(dmsgpack_unpacker* mpac)
{
    return mpac->buffer + mpac->used;
}

static inline size_t dmsgpack_unpacker_buffer_capacity(const dmsgpack_unpacker* mpac)
{
    return mpac->free;
}

static inline void dmsgpack_unpacker_buffer_consumed(dmsgpack_unpacker* mpac, size_t size)
{
    mpac->used += size;
    mpac->free -= size;
}

static inline size_t dmsgpack_unpacker_message_size(const dmsgpack_unpacker* mpac)
{
    return mpac->parsed - mpac->off + mpac->used;
}

static inline size_t dmsgpack_unpacker_parsed_size(const dmsgpack_unpacker* mpac)
{
    return mpac->parsed;
}


static inline void dmsgpack_unpacked_init(dmsgpack_unpacked* result)
{
    memset(result, 0, sizeof(dmsgpack_unpacked));
}

static inline void dmsgpack_unpacked_destroy(dmsgpack_unpacked* result)
{
    if(result->zone != NULL) {
        dmsgpack_zone_free(result->zone);
        result->zone = NULL;
        memset(&result->data, 0, sizeof(dmsgpack_object));
    }
}

static inline dmsgpack_zone* dmsgpack_unpacked_release_zone(dmsgpack_unpacked* result)
{
    if(result->zone != NULL) {
        dmsgpack_zone* z = result->zone;
        result->zone = NULL;
        return z;
    }
    return NULL;
}


#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/unpack.h */
