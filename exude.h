/*
 * Copyright (c) 2011 Conformal Systems LLC <info@conformal.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef _EXUDE_H_
#define _EXUDE_H_

#include <stdlib.h>
#include <stdarg.h>

/* versioning */
#define EXUDE_STRINGIFY(x)	#x
#define EXUDE_STR(x)		EXUDE_STRINGIFY(x)
#define EXUDE_VERSION_MAJOR	0
#define EXUDE_VERSION_MINOR	6
#define EXUDE_VERSION_PATCH	0
#define EXUDE_VERSION		EXUDE_STR(EXUDE_VERSION_MAJOR) "." \
				EXUDE_STR(EXUDE_VERSION_MINOR) "." \
				EXUDE_STR(EXUDE_VERSION_PATCH)

const char	*exude_verstring(void);
void		 exude_version(int *major, int *minor, int *patch);
#define	EXUDE_DBG_ALWAYS	(~(0Ull))
void		 exude_enable(uint64_t);
void		 exude_disable(void);

void	e_check_memory(void);
void	*e_malloc_debug(size_t, const char *, const char *, int);
void	*e_calloc_debug(size_t, size_t, const char *, const char *, int);
void	e_free_debug(void **, const char *, const char *, int);
char	*e_strdup_debug(const char *, const char *, const char *, int);
int	e_asprintf_debug(char **ret, const char *file, const char *func,
	    int line, const char *fmt, ...);
int	e_vasprintf_debug(char **ret, const char *file, const char *func,
	    int line, const char *fmt, va_list);
void	*e_realloc_debug(void *, size_t, const char *, const char *, int);

#define e_malloc(a)	e_malloc_debug(a, __FILE__, __FUNCTION__, __LINE__)
#define e_calloc(a,b)	e_calloc_debug(a, b, __FILE__, __FUNCTION__, __LINE__)
#define e_free(a)	e_free_debug((void **)a, __FILE__, __FUNCTION__,\
			    __LINE__)
#define e_strdup(a)	e_strdup_debug(a, __FILE__, __FUNCTION__, __LINE__)
#define e_asprintf(a,b...)	e_asprintf_debug(a, __FILE__, __FUNCTION__,\
				    __LINE__, b)
#define e_vasprintf(a,b, c)	e_vasprintf_debug(a, __FILE__, __FUNCTION__,\
				    __LINE__, b, c)
#define e_realloc(a,b)	e_realloc_debug(a, b, __FILE__, __FUNCTION__, __LINE__)

#endif /* ! _EXUDE_H_ */
