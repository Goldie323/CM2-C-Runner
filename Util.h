#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define app_abort(func_name, reasson, ...) \
{fprintf(stderr, "[ABORT]: %s:" func_name ":%d: " reasson "\n", __FILE_NAME__, __LINE__, ##__VA_ARGS__); \
exit(1);}

#define quick_abort(reasson, ...) \
{fprintf(stderr, "[ABORT]: %s:%d: " reasson "\n", __FILE_NAME__, __LINE__, ##__VA_ARGS__); \
exit(1);}

// Strong malloc
void *smalloc(size_t size);
// Strong realloc
void *srealloc(void *ptr, size_t size);

#endif