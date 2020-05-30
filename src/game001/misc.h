#include <stdlib.h>

#define ARRSIZE(array) (sizeof(array)/sizeof(*(array)))
#define RADIANS(degrees) ((degrees)*(M_PI/180))
#define MAX(x, y) ((x) > (y)?(x):(y))

typedef signed int byte;

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
