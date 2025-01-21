/*
 * MessagePack for C version information
 *
 * Copyright (C) 2008-2009 FURUHASHI Sadayuki
 *
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef DMSGPACK_VERSION_H
#define DMSGPACK_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

DMSGPACK_DLLEXPORT
const char* dmsgpack_version(void);
DMSGPACK_DLLEXPORT
int dmsgpack_version_major(void);
DMSGPACK_DLLEXPORT
int dmsgpack_version_minor(void);
DMSGPACK_DLLEXPORT
int dmsgpack_version_revision(void);

#include "version_master.h"

#define DMSGPACK_STR(v) #v
#define DMSGPACK_VERSION_I(maj, min, rev) DMSGPACK_STR(maj) "." DMSGPACK_STR(min) "." DMSGPACK_STR(rev)

#define DMSGPACK_VERSION DMSGPACK_VERSION_I(DMSGPACK_VERSION_MAJOR, DMSGPACK_VERSION_MINOR, DMSGPACK_VERSION_REVISION)

#ifdef __cplusplus
}
#endif

#endif /* dmsgpack/version.h */

