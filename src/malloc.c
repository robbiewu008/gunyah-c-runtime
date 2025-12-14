// Minimal bump allocator backed by the RootVM heap (init_heap/sys_brk).
//
// SPDX-License-Identifier: BSD-3-Clause

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <arch_def.h>
#include <sys/types.h>
#include <syscall_defs.h>

// Align to 16 bytes to satisfy typical ABI requirements.
#define ALIGN_UP_16(x) (((x) + 15U) & ~(size_t)15U)

typedef struct header {
	size_t size;
} header_t;

static uintptr_t heap_cursor;
static uintptr_t heap_limit;
static bool	heap_initialised;

static bool
heap_grow(uintptr_t min_required)
{
	// Grow brk to the next page boundary.
	uintptr_t new_brk_req = (min_required + (PAGE_SIZE - 1U)) &
			       ~(uintptr_t)(PAGE_SIZE - 1U);

	long ret = sys_brk((unsigned long)new_brk_req);
	if ((ret <= 0) || ((uintptr_t)ret < new_brk_req)) {
		return false;
	}

	heap_limit = (uintptr_t)ret;
	return true;
}

static bool
heap_reserve(size_t bytes, uintptr_t *out_start)
{
	if (!heap_initialised) {
		long cur = sys_brk(0UL);
		if (cur <= 0) {
			return false;
		}
		heap_cursor	 = (uintptr_t)cur;
		heap_limit	 = (uintptr_t)cur;
		heap_initialised = true;
	}

	uintptr_t start = heap_cursor;
	if ((start + bytes) < start) {
		return false;
	}

	uintptr_t end = start + bytes;
	if (end > heap_limit) {
		if (!heap_grow(end)) {
			return false;
		}
	}

	heap_cursor = end;
	*out_start  = start;
	return true;
}

void *
malloc(size_t size)
{
	if (size == 0U) {
		return NULL;
	}

	size_t alloc_size = ALIGN_UP_16(sizeof(header_t) + size);

	uintptr_t start;
	if (!heap_reserve(alloc_size, &start)) {
		return NULL;
	}

	header_t *h = (header_t *)(void *)start;
	h->size	    = size;

	return (void *)(h + 1);
}

void
free(void *ptr)
{
	(void)ptr;
	// No-op (bump allocator).
}

void *
calloc(size_t nmemb, size_t size)
{
	if ((nmemb != 0U) && (size > (SIZE_MAX / nmemb))) {
		return NULL;
	}

	size_t total = nmemb * size;
	void  *ptr   = malloc(total);
	if (ptr != NULL) {
		(void)memset(ptr, 0, total);
	}
	return ptr;
}

void *
realloc(void *ptr, size_t size)
{
	if (ptr == NULL) {
		return malloc(size);
	}
	if (size == 0U) {
		free(ptr);
		return NULL;
	}

	header_t *h	 = ((header_t *)ptr) - 1;
	size_t	  old_size = h->size;

	if (size <= old_size) {
		h->size = size;
		return ptr;
	}

	void *new_ptr = malloc(size);
	if (new_ptr != NULL) {
		(void)memcpy(new_ptr, ptr, old_size);
		free(ptr);
	}
	return new_ptr;
}

void *
aligned_alloc(size_t alignment, size_t size)
{
	if ((size == 0U) || (alignment == 0U)) {
		return NULL;
	}

	// alignment must be power of 2.
	if ((alignment & (alignment - 1U)) != 0U) {
		return NULL;
	}

	// Reserve enough space for worst-case alignment padding plus header.
	size_t	  reserve = size + (alignment - 1U) + sizeof(header_t);
	reserve		  = ALIGN_UP_16(reserve);

	uintptr_t start;
	if (!heap_reserve(reserve, &start)) {
		return NULL;
	}

	uintptr_t payload_base = start + sizeof(header_t);
	uintptr_t payload_aligned =
		(payload_base + (alignment - 1U)) &
		~(uintptr_t)(alignment - 1U);

	header_t *h = (header_t *)(void *)(payload_aligned - sizeof(header_t));
	h->size	    = size;

	return (void *)(h + 1);
}
