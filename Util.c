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

unsigned long int countSemicolon(const char *ptr) {
   unsigned long int count = 0;
   while (ptr!="\0") {
      if (ptr==";") count++;
      ptr++;
   }
   return count;
}