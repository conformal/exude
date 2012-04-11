/*
 * Copyright (c) 2011-2012 Conformal Systems LLC <info@conformal.com>
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
#include <sys/tree.h>

#include <clog.h>

#include "exude.h"

#ifdef BUILDSTR
static const char *vertag = "version: " EXUDE_VERSION " " BUILDSTR;
#else
static const char *vertag = "version: " EXUDE_VERSION;
#endif

struct e_mem_debug {
	void		*emd_address;
	size_t		emd_size;
	const char	*emd_file;
	const char	*emd_func;
	int		emd_line;
	RB_ENTRY(e_mem_debug)	emd_entry;	/* r/b on address */
};
RB_HEAD(e_mem_debug_tree, e_mem_debug);

static void e_mem_init();

static int			e_runtime_disable = 1;
static uint64_t			exude_clog_debug_mask;

const char *
exude_verstring(void)
{
	return (vertag);
}

void
exude_version(int *major, int *minor, int *patch)
{
	*major = EXUDE_VERSION_MAJOR;
	*minor = EXUDE_VERSION_MINOR;
	*patch = EXUDE_VERSION_PATCH;
}

void
exude_enable(uint64_t debugmask)
{
	e_runtime_disable = 0;
	exude_clog_debug_mask = debugmask;
	e_mem_init();
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
		CABORTX("invalid malloc size %lu", (unsigned long) sz);

	p = malloc(sz);
	if (p == NULL) {
		CFATAL("malloc failed: sz %lu", (unsigned long) sz);
	}

	return (p);
}

void *
e_calloc_internal(size_t nmemb, size_t sz)
{
	void			*p;

	if (nmemb <= 0 || sz <= 0)
		CABORTX("invalid calloc size %lu %lu", (unsigned long) sz,
		    (unsigned long) nmemb);

	p = calloc(nmemb, sz);
	if (p == NULL) {
		CFATAL("calloc failed: nmemb %lu sz %lu",
		    (unsigned long) nmemb, (unsigned long) sz);
	}

	return (p);
}

void
e_free_internal(void **p)
{
	if (p == NULL)
		CABORTX("%s: bad pointer", __func__);
	free(*p);
	*p = NULL;
}

char *
e_strdup_internal(const char *s)
{
	char			*r;

	if (s == NULL)
		CABORTX("%s: bad pointer", __func__);

	r = strdup(s);
	if (r == NULL)
		CFATAL("strdup failed");

	return (r);
}

int
e_asprintf_internal(char **ret, const char *fmt, ...)
{
	va_list			ap;
	int			sz;

	va_start(ap, fmt);
	if ((sz = vasprintf(ret, fmt, ap)) == -1)
		CFATAL("vasprintf failed");
	va_end(ap);

	return (sz);
}

int
e_vasprintf_internal(char **ret, const char *fmt, va_list ap)
{
	int			sz;

	if ((sz = vasprintf(ret, fmt, ap)) == -1)
		CFATAL("vasprintf failed");

	return (sz);
}

void *
e_realloc_internal(void *p, size_t sz)
{
	void			*np;

	np = realloc(p, sz);
	if (np == NULL)
		CFATAL("realloc failed");

	return (np);
}

int
e_cmp_mem_debug_addr(struct e_mem_debug *c1, struct e_mem_debug *c2)
{
	return (c1->emd_address < c2->emd_address ? -1 :
	    c1->emd_address > c2->emd_address);
}

RB_GENERATE(e_mem_debug_tree, e_mem_debug, emd_entry, e_cmp_mem_debug_addr)

#include <exude_threads.h>


static void *emd_mtx;
static struct e_mem_debug_tree	emd_mem_debug;
static int			emd_init;
static int			emd_paint_free;
static int			emd_paint_alloc;

void
exude_enable_threads(void)
{
	EX_LOCK_ALLOC(&emd_mtx);
}

void
exude_cleanup()
{
	if (emd_mtx)
		EX_LOCK_FREE(&emd_mtx);
}

void
e_mem_init(void)
{
	if (emd_init)
		return;

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

	EX_LOCK(emd_mtx);
	if (RB_INSERT(e_mem_debug_tree, &emd_mem_debug, emd))
		CABORTX("duplicate address");
	EX_UNLOCK(emd_mtx);
}

void *
e_malloc_debug(size_t sz, const char *file, const char *func,
    int line)
{
	void			*p;

	p = e_malloc_internal(sz);

	if (e_runtime_disable)
		return (p);

	CNDBG(exude_clog_debug_mask, "sz %lu file %s func %s line %d "
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

	p = e_calloc_internal(nmemb, sz);

	if (e_runtime_disable)
		return (p);

	CNDBG(exude_clog_debug_mask,
	    "nmemb %lu sz %lu file %s func %s line %d "
	    "p %p", (unsigned long) nmemb, (unsigned long) sz, file, func,
	    line, p);

	e_mem_add_rb(p, nmemb * sz, file, func, line);

	return (p);
}

void
e_free_debug(void **p, const char *file, const char *func, int line)
{
	struct e_mem_debug	e, *emd;

	if (e_runtime_disable) {
		e_free_internal(p);
		return;
	}

	if (p == NULL)
		CABORTX("bad pointer");

	CNDBG(exude_clog_debug_mask, "p %p file %s func %s line %d",
	    *p, file, func, line);

	e.emd_address = *p;
	EX_LOCK(emd_mtx);
	if ((emd = RB_FIND(e_mem_debug_tree, &emd_mem_debug, &e)) != NULL) {
		/* found */
		RB_REMOVE(e_mem_debug_tree, &emd_mem_debug, emd);
		EX_UNLOCK(emd_mtx);
		if (emd_paint_free)
			memset(emd->emd_address, 0xff, emd->emd_size);
		free(emd);

		e_free_internal(p);
	} else {
		EX_UNLOCK(emd_mtx);
		CFATALX("%p not found %s %s %d", *p, file, func, line);
	}

}

char *
e_strdup_debug(const char *s, const char *file, const char *func, int line)
{
	char			*p;

	p = e_strdup_internal(s);

	if (e_runtime_disable)
		return (p);

	CNDBG(exude_clog_debug_mask, "p %p p %s", s, s);

	e_mem_add_rb(p, strlen(p), file, func, line);

	return (p);
}

int
e_asprintf_debug(char **ret, const char *file, const char *func, int line,
    const char *fmt, ...)
{
	va_list			ap;
	int			sz;

	va_start(ap, fmt);
	if ((sz = vasprintf(ret, fmt, ap)) == -1)
		CFATAL("vasprintf failed");
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

	if ((sz = vasprintf(ret, fmt, ap)) == -1)
		CFATAL("vasprintf failed");

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

	if (e_runtime_disable)
		return(e_realloc_internal(p, sz));

	CNDBG(exude_clog_debug_mask, "looking for %p", p);
	if (p == NULL) {
		np = malloc(sz);
		if (np == NULL)
			CFATAL("malloc failed");
		e_mem_add_rb(np, sz, file, func, line);
		return (np);
	}

	e.emd_address = p;
	EX_LOCK(emd_mtx);
	if ((emd = RB_FIND(e_mem_debug_tree, &emd_mem_debug, &e)) != NULL) {
		RB_REMOVE(e_mem_debug_tree, &emd_mem_debug, emd);
		EX_UNLOCK(emd_mtx);
		np = realloc(p, sz);
		if (np == NULL)
			CFATAL("realloc failed");
		CNDBG(exude_clog_debug_mask, "found %p %lu now %p %lu", p,
		    (unsigned long) emd->emd_size, np, (unsigned long) sz);
		free(emd);
		e_mem_add_rb(np, sz, file, func, line);
	} else {
		EX_UNLOCK(emd_mtx);
		CABORTX("%p not found for realloc", p);
	}

	CNDBG(exude_clog_debug_mask, "old %p new %p", p, np);

	return (np);
}

void
e_check_memory(void)
{
	struct e_mem_debug	*emd;

	if (e_runtime_disable)
		return;

	EX_LOCK(emd_mtx);
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
	EX_UNLOCK(emd_mtx);

	CNDBG(exude_clog_debug_mask, "memory clean");
}
