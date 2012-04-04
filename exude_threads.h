/*
 * Copyright (c) 2012 Conformal Systems LLC <info@conformal.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIREEX_, INDIREEX_, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * AEX_ION OF CONTRAEX_, NEGLIGENCE OR OTHER TORTIOUS AEX_ION, ARISING OUT OF
 * OR IN CONNEEX_ION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _EX_THREADS_H_
#define _EX_THREADS_H_

#include <pthread.h>

#ifndef EX_LOCK
#define EX_LOCK(var) do {						\
	if (var != NULL)						\
		pthread_mutex_lock(var);				\
} while (0)
#endif
#ifndef EX_UNLOCK
#define EX_UNLOCK(var) do {						\
	if (var != NULL)						\
		pthread_mutex_unlock(var);				\
} while (0)
#endif
#ifndef EX_LOCK_ALLOC
#define EX_LOCK_ALLOC(var)  do {					\
	*var = calloc (1, sizeof (pthread_mutex_t));			\
	pthread_mutex_init(*var, NULL);					\
} while (0)
#endif
#ifndef EX_LOCK_FREE
#define EX_LOCK_FREE(var) do {						\
	pthread_mutex_destroy(*var);					\
	free(*var);							\
	*var = NULL;							\
} while (0);
#endif

#endif	/* !  _EX_THREADS_H_ */
