// © 2021 Qualcomm Innovation Center, Inc. All rights reserved.
//
// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>
#include <stdlib.h>

int
strcmp(const char *s1, const char *s2)
{
	const char *l = s1, *r = s2;

	while ((*l != '\0') && (*l == *r)) {
		l++;
		r++;
	}

	return *l - *r;
}

size_t
strnlen(const char *s, size_t maxlen)
{
	size_t len = 0;
	for (; len < maxlen && s[len]; len++);
	return len;
}

// Implementation for strdup
char *
strdup(const char *s)
{
	size_t len = strlen(s);
	char *new_s = malloc(len + 1);
	if (new_s == NULL) {
		return NULL;
	}
	return memcpy(new_s, s, len + 1);
}

// Implementation for strndup
char *
strndup(const char *s, size_t n)
{
	size_t len = strnlen(s, n);
	char *new_s = malloc(len + 1);
	if (new_s == NULL) {
		return NULL;
	}
	new_s[len] = '\0';
	return memcpy(new_s, s, len);
}

// Implementation for strchr
char *
strchr(const char *s, int c)
{
	while (*s != (char)c) {
		if (!*s) {
			return NULL;
		}
		s++;
	}
	return (char *)s;
}

// Implementation for strrchr
char *
strrchr(const char *s, int c)
{
	char *ret = NULL;
	do {
		if (*s == (char)c) {
			ret = (char *)s;
		}
	} while (*s++);
	return ret;
}

// Implementation for strspn
size_t
strspn(const char *s, const char *accept)
{
	const char *p = s;
	const char *a;
	while (*p) {
		for (a = accept; *a; a++) {
			if (*p == *a) {
				break;
			}
		}
		if (!*a) {
			return (size_t)(p - s);
		}
		p++;
	}
	return (size_t)(p - s);
}

// Implementation for strpbrk
char *
strpbrk(const char *s, const char *accept)
{
	const char *a;
	while (*s) {
		for (a = accept; *a; a++) {
			if (*s == *a) {
				return (char *)s;
			}
		}
		s++;
	}
	return NULL;
}

// Implementation for strtok_r (reentrant strtok)
char *
strtok_r(char *s, const char *delim, char **saveptr)
{
	char *token;
	if (s == NULL) {
		s = *saveptr;
	}
	s += strspn(s, delim);
	if (*s == '\0') {
		return NULL;
	}
	token = s;
	s = strpbrk(token, delim);
	if (s == NULL) {
		*saveptr = strchr(token, '\0');
	} else {
		*s = '\0';
		*saveptr = s + 1;
	}
	return token;
}

// Implementation for memcmp
int
memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *c1 = s1, *c2 = s2;
	for (; n--; c1++, c2++) {
		if (*c1 != *c2) {
			return *c1 - *c2;
		}
	}
	return 0;
}

// Helpers for strtoul
static inline int isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}
static inline int isdigit(int c) {
    return c >= '0' && c <= '9';
}
static inline int isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
static inline int isupper(int c) {
    return c >= 'A' && c <= 'Z';
}

#include <limits.h>

unsigned long strtoul(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;

    if (base < 0 || base == 1 || base > 36) {
        if (endptr) *endptr = (char *)nptr;
        return 0;
    }

    do {
        c = *s++;
    } while (isspace(c));

    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    cutoff = ULONG_MAX / (unsigned long)base;
    cutlim = (int)(ULONG_MAX % (unsigned long)base);
    for (acc = 0, any = 0;; c = *s++) {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc *= (unsigned long)base;
            acc += (unsigned long)c;
        }
    }
    if (any < 0) {
        acc = ULONG_MAX;
    } else if (neg)
        acc = -acc;
    if (endptr != 0)
        *endptr = (char *)(any ? s - 1 : nptr);
    return acc;
}
