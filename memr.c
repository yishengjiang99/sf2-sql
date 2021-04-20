#define HAVE_FUNOPEN
#include "fmemopen.c"

int read(void *data, int n)
{
	int fd = fmemopen(data, n, "r");
	return 1;
}