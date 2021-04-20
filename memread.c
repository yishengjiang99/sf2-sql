
#include <stdlib.h>
#include <string.h>
typedef struct
{
	void *(*read)(unsigned int size);
	int (*skip)(unsigned int size);
	void *ptr;
	int offset;
} fhandle;
static fhandle fh;
static inline void *mread(unsigned int size)
{
	void *dest = malloc(size);
	memcpy(dest, fh.ptr + fh.offset, size);
	fh.offset += size;
	return dest;
}
static inline int mskip(unsigned int size)
{
	fh.offset = fh.offset + size;
	return size;
}
static inline void mclose()
{
	fh.ptr = NULL;
}

fhandle *mem_open_file(void *ptr)
{
	fh.ptr = ptr;
	fh.offset = 0;
	fh.read = mread;
	fh.skip = mskip;
	return &fh;
}
