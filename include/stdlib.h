// SPDX-License-Identifier: BSD-3-Clause

#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

void *aligned_alloc(size_t alignment, size_t size);

unsigned long strtoul(const char *nptr, char **endptr, int base);

#include <stdnoreturn.h> // For noreturn
noreturn void exit(int status);

#endif
