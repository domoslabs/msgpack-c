/*
 * MessagePack for C TimeStamp
 *
 * Copyright (C) 2018 KONDO Takatoshi
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_TIMESTAMP_H
#define DMSGPACK_TIMESTAMP_H

#include <dmsgpack/object.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct dmsgpack_timestamp {
    int64_t tv_sec;
    uint32_t tv_nsec;
} dmsgpack_timestamp;

static inline bool dmsgpack_object_to_timestamp(const dmsgpack_object* obj, dmsgpack_timestamp* ts) {
    if (obj->type != DMSGPACK_OBJECT_EXT) return false;
    if (obj->via.ext.type != -1) return false;
    switch (obj->via.ext.size) {
    case 4:
        ts->tv_nsec = 0;
        {
            uint32_t v;
            _dmsgpack_load32(uint32_t, obj->via.ext.ptr, &v);
            ts->tv_sec = v;
        }
        return true;
    case 8: {
        uint64_t value;
        _dmsgpack_load64(uint64_t, obj->via.ext.ptr, &value);
        ts->tv_nsec = (uint32_t)(value >> 34);
        ts->tv_sec = value & 0x00000003ffffffffLL;
        return true;
    }
    case 12:
        _dmsgpack_load32(uint32_t, obj->via.ext.ptr, &ts->tv_nsec);
        _dmsgpack_load64(int64_t, obj->via.ext.ptr + 4, &ts->tv_sec);
        return true;
    default:
        return false;
    }
}


#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/timestamp.h */
