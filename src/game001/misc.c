#include <stdio.h>
#include <stdlib.h>

#include "game001.h"

char *program_name;

void *xmalloc(size_t size) {
	void *result = malloc(size);

	if(result == NULL) {
		exit(1);
		fprintf(stderr, "%s: failed to allocate memory.\n", program_name);
	}

	return result;
}

void *xrealloc(void *ptr, size_t size) {
	void *result = realloc(ptr, size);

	if(result == NULL) {
		exit(1);
		fprintf(stderr, "%s: failed to reallocate memory.\n", program_name);
	}

	return result;
}
