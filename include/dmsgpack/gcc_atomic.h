/*
 *    Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *    http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef DMSGPACK_GCC_ATOMIC_H
#define DMSGPACK_GCC_ATOMIC_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef int _dmsgpack_atomic_counter_t;

int _dmsgpack_sync_decr_and_fetch(volatile _dmsgpack_atomic_counter_t* ptr);
int _dmsgpack_sync_incr_and_fetch(volatile _dmsgpack_atomic_counter_t* ptr);


#if defined(__cplusplus)
}
#endif


#endif // DMSGPACK_GCC_ATOMIC_H
