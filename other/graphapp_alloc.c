/* GraphApp's copy of libjpeg takes its memory from GraphApp's own
   app_alloc() and app_free().  OpenGUI isn't GraphApp: here they are,
   on malloc() and free(). */
#include <stdlib.h>

void * app_alloc(long size) { return malloc(size > 0 ? (size_t) size : 1); }
void   app_free(void * ptr) { free(ptr); }
