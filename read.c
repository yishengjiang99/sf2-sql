
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "index.h"

typedef pgen_t pgen;
typedef pgen_t igen;
static int nphdrs, npbags, npgens, npmods, nshdrs, ninsts, nimods, nigens, nibags, nshrs;
static phdr *phdrs;
static pbag *pbags;
static pmod *pmods;
static pgen *pgens;
static inst *insts;
static ibag *ibags;
static imod *imods;
static igen *igens;
static shdr *shdrs;
static short *data;
static int nsamples;
int readsf(int argc, char **argv);

int readsf(int argc, char **argv)
{
	section_header *sh = (section_header *)malloc(sizeof(section_header));
	unsigned int size, size2L;
	char *filename = argc > 1 ? argv[1] : "file.sf2";
	FILE *fd = fopen(filename, "r");
	header_t *header = (header_t *)malloc(sizeof(header_t));
	header2_t *h2 = (header2_t *)malloc(sizeof(header2_t));
	fread(header, sizeof(header_t), 1, fd);
	fprintf(stdout, "%.4s %.4s %.4s %u", header->name, header->sfbk, header->list, header->size);
	fread(h2, sizeof(header2_t), 1, fd);
	fprintf(stdout, "\n%.4s %u", h2->name, h2->size);
	for (int i = 0; i < h2->size; i++)
		putchar(fgetc(fd) & 0x7f);

	fread(h2, sizeof(header2_t), 1, fd);
	fprintf(stdout, "\n%.4s %u", h2->name, h2->size);
	data = (short *)malloc(h2->size);
	nsamples = h2->size / sizeof(short);
	fread(data, sizeof(short), nsamples, fd);

	fread(h2, sizeof(header2_t), 1, fd);
	fprintf(stdout, "\n%.4s %u\n", h2->name, h2->size);

#define readSection(section)                        \
	fread(sh, sizeof(section_header), 1, fd);         \
	fprintf(stdout, "%.4s:%u\n", sh->name, sh->size); \
	n##section##s = sh->size / sizeof(section);       \
	section##s = (section *)malloc(sh->size);         \
	fread(section##s, sizeof(section), sh->size / sizeof(section), fd);

	readSection(phdr);
	readSection(pbag);
	readSection(pmod);
	readSection(pgen);
	readSection(inst);
	readSection(ibag);
	readSection(imod);
	readSection(igen);
	readSection(shdr);
	return 1;
}