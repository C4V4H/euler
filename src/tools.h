#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/* ===================== TOOLS ====================== */

static inline void*
xmalloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "ERROR: malloc failed. exiting the program...");
        exit(errno);
    }
    return ptr;
}

static inline void*
xrealloc(void* oldptr, size_t size) {
    void* ptr = realloc(oldptr, size);
    if (!ptr) {
        fprintf(stderr, "ERROR: malloc failed. exiting the program...");
        exit(errno);
    }
    return ptr;
}

static inline void*
xcalloc(size_t count, size_t size) {
    void* ptr = calloc(count, size);
    if (!ptr) {
        fprintf(stderr, "ERROR: malloc failed. exiting the program...");
        exit(errno);
    }
    return ptr;
}
#endif
