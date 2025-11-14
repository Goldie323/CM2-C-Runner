#include <stdlib.h>
#include "Util.h"

// Strong malloc
void *smalloc(size_t size) {
   void *alloc = malloc(size);
   if (alloc) return alloc;
   else app_abort("smalloc()", "Error: Failed to allocate memory for buffer\n")
}

// Strong realloc
void *srealloc(void *ptr, size_t size) {
   void *alloc = realloc(ptr, size);
   if (alloc) return alloc;
   else app_abort("srealloc()", "Error: Failed to reallocate memory for buffer\n")
}

// Strong calloc, except it only takes the size in bytes, not number of elements and size of element.
void *scalloc(size_t size) {
   void *alloc = calloc(size, 1);
   if (alloc) return alloc;
   else app_abort("srealloc()", "Error: Failed to allocate zeroed memory for buffer\n")
}