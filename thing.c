#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "memread.c"
#include "index.h"
typedef struct _thing
{
	char *data;
	struct _thing *next;
	struct _thing *child;
} THANG;

THANG *newThing(void *data)
{
	THANG *newThing = (THANG *)malloc(sizeof(THANG));
	newThing->data = (char *)data;
	newThing->next = NULL;
	newThing->child = NULL;
	return newThing;
}
typedef struct _riff
{
	char name[4];
	uint32_t size;
} riff;
static phdr *phdrs;
static pbag *pbags;
static pmod *pmods;
static pgen_t *pgens;
static inst *insts;
static ibag *ibags;
static imod *imods;
static pgen_t *igens;
static uint8_t *shdrs;
static short *data;
static void *info;
static int nsamples;
static int nphdrs, npbags, npgens, npmods, nshdrs, ninsts, nimods, nigens, nibags, nshrs;
static THANG *head = NULL;
int cmp(void *A, void *B)
{
	return -1;
}

int reaadd(char *data, int n)
{

	int offset = 0;
	riff *r;
	phdr *phdrs;
	int section = 0;
	THANG **trace = &head;
	while (section++ < 10)
	{
		r = (riff *)(data + offset);
		*trace = newThing(phdrs);
		trace = &(*trace)->next;
		offset += 8 + r->size;
	}
	trace = &(head->child);
	for (offset = 8; offset < ((riff *)(head->data))
																		->size +
																8;)
	{
		phdrs = (phdr *)(data + offset);
		offset = offset + sizeof(phdr);
		*trace = newThing(phdrs);
		trace = &(*trace)->next;
	}

	return 1;

	// while (fd->offset < n - 100)
	// {
	// 	riff *r = (riff *)(data + offset);
	// 	offset += 8 + r->size;
	// 	printf("\n%.4s %d %d", r->name, offset, r->size);
	// 	insertNode(&head, newThing(r), &cmp);
	// 	fd->skip(r->size);
	// }
}

// riff /?*r3 = (riff *)(data + r2->size);

// riff *rc = (riff *)(data + 4);
// insertNode(&head, newThing(r), &cmp);
// insertNode(&head, newThing(r2), &cmp);
// insertNode(&head, newThing(r3), &cmp);

int main()
{
	FILE *fd = fopen("file.sf2", "rb");
	fseek(fd, 31028742, SEEK_SET);
	char *data = malloc(31281186 - 31028742);
	fread(data, 1, 31281186 - 31028742, fd);
	reaadd(data, 31281186 - 31028742);
	//	printNode(&head);
}