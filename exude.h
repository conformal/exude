/* $exude$ */
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

#include <sys/tree.h>

/* versioning */
#define EXUDE_VERSION_MAJOR	0
#define EXUDE_VERSION_MINOR	2
#define EXUDE_VERSION_PATCH	0
#define EXUDE_VERSION		"0.2.0"

void	exude_version(int *major, int *minor, int *patch);

#define E_MEM_DEBUG

#ifdef E_MEM_DEBUG
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

struct e_mem_debug {
	void		*emd_address;
	size_t		emd_size;
	char		*emd_file;
	char		*emd_func;
	int		emd_line;
	RB_ENTRY(e_mem_debug)	emd_entry;	/* r/b on address */
};
RB_HEAD(e_mem_debug_tree, e_mem_debug);

#else /* E_MEM_DEBUG */
#define e_check_memory()
void	*e_malloc_internal(size_t);
void	*e_calloc_internal(size_t, size_t);
void	e_free_internal(void **);
char	*e_strdup_internal(const char *);
int	e_asprintf_internal(char **, const char *, ...);
int	e_vasprintf_internal(char **t, const char *, va_list);
void	*e_realloc_internal(void *, size_t);
#define e_malloc(a)	e_malloc_internal(a)
#define e_malloc(a, b)	e_calloc_internal(a, b)
#define e_free(a)	e_free_internal((void **)a)
#define e_strdup(a)	e_strdup_internal(a)
#define e_asprintf(a,b...)	e_asprintf_internal(a, b)
#define e_vasprintf(a,b, va)	e_vasprintf_internal(a, b, va)
#define e_realloc(a, b)	e_realloc_internal(a, b)
#endif
