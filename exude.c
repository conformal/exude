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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>

#include <clog.h>

#include "exude.h"

static const char *vertag = "version: " EXUDE_VERSION;

int			e_runtime_disable = 1;

void
exude_version(int *major, int *minor, int *patch)
{
	*major = EXUDE_VERSION_MAJOR;
	*minor = EXUDE_VERSION_MINOR;
	*patch = EXUDE_VERSION_PATCH;
	/* Portable way to avoid unused variable compile warnings */
	(void) (vertag);
}

void
exude_enable(void)
{
	e_runtime_disable = 0;
}

void
exude_disable(void)
{
	e_runtime_disable = 1;
}

void *
e_malloc_internal(size_t sz)
{
	void			*p;

	if (sz <= 0)
		CFATALX("invalid malloc size %lu", (unsigned long) sz);

	p = malloc(sz);
	if (p == NULL) {
		CFATAL("sz %lu", (unsigned long) sz);
	}

	return (p);
}

void *
e_calloc_internal(size_t nmemb, size_t sz)
{
	void			*p;

	if (nmemb <= 0 || sz <= 0)
		CFATALX("invalid calloc size %lu", (unsigned long) sz);

	p = calloc(nmemb, sz);
	if (p == NULL) {
		CFATAL("nmemb %lu sz %lu", (unsigned long) nmemb,
		    (unsigned long) sz);
	}

	return (p);
}

void
e_free_internal(void **p)
{
	if (p == NULL)
		CFATALX("bad pointer");
	free(*p);
	*p = NULL;
}

char *
e_strdup_internal(const char *s)
{
	char			*r;

	if (s == NULL)
		CFATALX("bad pointer");

	r = strdup(s);
	if (r == NULL)
		CFATAL("strdup fail");

	return (r);
}

int
e_asprintf_internal(char **ret, const char *fmt, ...)
{
	va_list			ap;
	int			sz;

	va_start(ap, fmt);
	if ((sz = vasprintf(ret, fmt, ap)) == -1)
		CFATAL("vasprintf fail");
	va_end(ap);

	return (sz);
}

int
e_vasprintf_internal(char **ret, const char *fmt, va_list ap)
{
	int			sz;

	if ((sz = vasprintf(ret, fmt, ap)) == -1)
		CFATAL("vasprintf fail");

	return (sz);
}

void *
e_realloc_internal(void *p, size_t sz)
{
	void			*np;

	np = realloc(p, sz);
	if (np == NULL)
		CFATALX("realloc fail");

	return (np);
}

/* poor mans' memory tester */
#ifdef E_MEM_DEBUG
int
e_cmp_mem_debug_addr(struct e_mem_debug *c1, struct e_mem_debug *c2)
{
	return (c1->emd_address < c2->emd_address ? -1 :
	    c1->emd_address > c2->emd_address);
}

RB_GENERATE(e_mem_debug_tree, e_mem_debug, emd_entry, e_cmp_mem_debug_addr)

struct e_mem_debug_tree	emd_mem_debug;
int			emd_init;
int			emd_paint_free;
int			emd_paint_alloc;

void
e_mem_init(void)
{
	RB_INIT(&emd_mem_debug);
	emd_init = 1;
	emd_paint_free = 1;
	emd_paint_alloc = 1;
}

void
e_mem_add_rb(void *p, size_t sz, const char *file, const char *func, int line)
{
	struct e_mem_debug	*emd;

	emd = malloc(sizeof *emd);
	if (emd == NULL)
		CFATAL("emd");
	emd->emd_address = p;
	emd->emd_size = sz;
	emd->emd_file = file;
	emd->emd_func = func;
	emd->emd_line = line;

	if (RB_INSERT(e_mem_debug_tree, &emd_mem_debug, emd))
		CFATALX("duplicate address");
}

void *
e_malloc_debug(size_t sz, const char *file, const char *func,
    int line)
{
	void			*p;

	if (emd_init == 0)
		e_mem_init();
	if (e_runtime_disable)
		return (e_malloc_internal(sz));

	p = e_malloc_internal(sz);
	CDBG("sz %lu file %s func %s line %d "
	    "p %p", (unsigned long) sz, file, func, line, p);

	if (emd_paint_alloc == 1)
		memset(p, 0xfe, sz);

	e_mem_add_rb(p, sz, file, func, line);

	return (p);
}

void *
e_calloc_debug(size_t nmemb, size_t sz, const char *file,
    const char *func, int line)
{
	void			*p;

	if (emd_init == 0)
		e_mem_init();
	if (e_runtime_disable)
		return (e_calloc_internal(nmemb, sz));

	p = e_calloc_internal(nmemb, sz);
	CDBG("nmemb %lu sz %lu file %s func %s line %d "
	    "p %p", (unsigned long) nmemb, (unsigned long) sz, file, func,
	    line, p);

	e_mem_add_rb(p, nmemb * sz, file, func, line);

	return (p);
}

void
e_free_debug(void **p, const char *file, const char *func, int line)
{
	struct e_mem_debug	e, *emd;

	if (emd_init == 0)
		CFATALX("not init yet %s %s %d", file, func, line);
	if (e_runtime_disable) {
		e_free_internal(p);
		return;
	}

	if (p == NULL)
		CFATALX("bad pointer");

	CDBG("p %p file %s func %s line %d",
	    *p, file, func, line);

	e.emd_address = *p;
	if ((emd = RB_FIND(e_mem_debug_tree, &emd_mem_debug, &e)) != NULL) {
		/* found */
		RB_REMOVE(e_mem_debug_tree, &emd_mem_debug, emd);
		if (emd_paint_free)
			memset(emd->emd_address, 0xff, emd->emd_size);
		free(emd);

		e_free_internal(p);
	} else
		CFATALX("%p not found %s %s %d", *p, file, func, line);

}

char *
e_strdup_debug(const char *s, const char *file, const char *func, int line)
{
	char			*p;

	if (emd_init == 0)
		e_mem_init();
	if (e_runtime_disable)
		return (e_strdup_internal(s));

	p = e_strdup_internal(s);

	CDBG("p %p p %s", s, s);

	e_mem_add_rb(p, strlen(p), file, func, line);

	return (p);
}

int
e_asprintf_debug(char **ret, const char *file, const char *func, int line,
    const char *fmt, ...)
{
	va_list			ap;
	int			sz;

	if (emd_init == 0)
		e_mem_init();

	va_start(ap, fmt);
	if ((sz = vasprintf(ret, fmt, ap)) == -1)
		CFATAL("vasprintf fail");
	va_end(ap);

	if (e_runtime_disable)
		return (sz);

	e_mem_add_rb(*ret, strlen(*ret), file, func, line);
	return (sz);
}

int
e_vasprintf_debug(char **ret, const char *file, const char *func, int line,
    const char *fmt, va_list ap)
{
	int			sz;

	if (emd_init == 0)
		e_mem_init();

	if ((sz = vasprintf(ret, fmt, ap)) == -1)
		CFATAL("vasprintf fail");

	if (e_runtime_disable)
		return (sz);

	e_mem_add_rb(*ret, strlen(*ret), file, func, line);
	return (sz);
}

void *
e_realloc_debug(void *p, size_t sz, const char *file, const char *func,
    int line)
{
	struct e_mem_debug	e, *emd;
	void			*np = NULL;

	if (emd_init == 0)
		e_mem_init();
	if (e_runtime_disable)
		return(e_realloc_internal(p, sz));

	CDBG("looking for %p", p);
	if (p == NULL) {
		np = malloc(sz);
		if (np == NULL)
			CFATALX("malloc fail");
		e_mem_add_rb(np, sz, file, func, line);
		return (np);
	}

	e.emd_address = p;
	if ((emd = RB_FIND(e_mem_debug_tree, &emd_mem_debug, &e)) != NULL) {
		np = realloc(p, sz);
		if (np == NULL)
			CFATALX("realloc fail");
		CDBG("found %p %lu now %p %lu", p,
		    (unsigned long) emd->emd_size, np, (unsigned long) sz);
		RB_REMOVE(e_mem_debug_tree, &emd_mem_debug, emd);
		free(emd);
		e_mem_add_rb(np, sz, file, func, line);
	} else
		CFATALX("%p not found for realloc", p);

	CDBG("old %p new %p", p, np);

	return (np);
}

void
e_check_memory(void)
{
	struct e_mem_debug	*emd;

	if (e_runtime_disable)
		return;

	if (!RB_EMPTY(&emd_mem_debug)) {
		CWARNX("not all memory was freed");
		RB_FOREACH(emd, e_mem_debug_tree, &emd_mem_debug) {
			CWARNX("file %s func %s line %d p %p sz %lu",
			    emd->emd_file,
			    emd->emd_func,
			    emd->emd_line,
			    emd->emd_address,
			    (unsigned long) emd->emd_size);
		}
		CABORTX("terminated");
	}

	CDBG("memory clean");
}
#endif /* E_MEM_DEBUG */
