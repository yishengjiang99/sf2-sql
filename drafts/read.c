
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "generators.c"

typedef uint32_t DWORD; // uint32_t;
typedef DWORD FOURCC;
typedef struct
{
	uint8_t lo, hi;
} rangesType; //  Four-character code
typedef struct
{
	FOURCC ckID;	//  A chunk ID identifies the type of data within the chunk.
	DWORD ckSize; // The size of the chunk data in bytes, excluding any pad byte.
	char *ckDATA; // The actual data plus a pad byte if req'd to word align.
} RIFFCHUNKS;
typedef union
{
	rangesType ranges;
	short shAmount;
	unsigned short uAmount;
} genAmountType;

typedef struct
{
	char name[4];
	unsigned int size;
	char sfbk[4];
	char list[4];
} header_t;

typedef struct
{
	unsigned int size;
	char name[4];
} header2_t;
typedef struct
{
	char name[4];
	unsigned int size;
} section_header;
typedef enum
{
	monoSample = 1,
	rightSample = 2,
	leftSample = 4,
	linkedSample = 8,
	RomMonoSample = 0x8001,
	RomRightSample = 0x8002,
	RomLeftSample = 0x8004,
	RomLinkedSample = 0x8008
} SFSampleLink;
typedef struct
{
	char name[4];
	unsigned int size;
	char *data;
} pdta;
typedef struct
{
	char name[20];
	uint16_t pid, bankId, pbagNdx;
	char idc[12];
} phdr;
typedef struct
{
	unsigned short pgen_id, pmod_id;
} pbag;
typedef struct
{
	unsigned short igen_id, imod_id;
} ibag;
typedef struct
{
	unsigned short operator;
	genAmountType val;
} pgen_t;
typedef pgen_t pgen;
typedef struct
{
	char data[10];
} pmod;
typedef struct
{
	char name[20];
	unsigned short ibagNdx;
} inst;
typedef struct
{
	char data[10];
} imod;
typedef union
{
	uint8_t hi, lo;
	unsigned short val;
	short word;
} gen_val;

typedef pgen_t igen;

typedef struct
{
	char name[20];
	uint32_t start, end, startloop, endloop, sampleRate;
	uint8_t originalPitch, pitchCorrection, v2, v3, v4, v5;

} shdrcast;

typedef uint8_t shdr;
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
static void* info;
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

	fprintf(stderr, "%.4s %.4s %.4s %u", header->name, header->sfbk, header->list, header->size);
	fread(h2, sizeof(header2_t), 1, fd);
	fprintf(stderr, "\n%.4s %u", h2->name, h2->size);
	info=malloc(h2->size);
	fread(info, h2->size, 1, fd);

	fread(h2, sizeof(header2_t), 1, fd);
	fprintf(stderr, "\n%.4s %u", h2->name, h2->size);
	data = (short *)malloc(h2->size);
	nsamples = h2->size / sizeof(short);
	fread(data, sizeof(short), nsamples, fd);

	fread(h2, sizeof(header2_t), 1, fd);
	fprintf(stderr, "\n%.4s %u\n", h2->name, h2->size);

#define readSection(section)                        \
	fread(sh, sizeof(section_header), 1, fd);         \
	fprintf(stderr, "%.4s:%u\n", sh->name, sh->size); \
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