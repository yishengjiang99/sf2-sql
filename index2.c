#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
typedef uint8_t BYTE;
typedef uint32_t DWORD; // uint32_t;
typedef DWORD FOURCC;
typedef struct
{
	uint8_t lo, hi;
} rangesType; //  Four-character code
typedef struct
{
	FOURCC ckID;  //  A chunk ID identifies the type of data within the chunk.
	DWORD ckSize; // The size of the chunk data in bytes, excluding any pad byte.
	BYTE *ckDATA; // The actual data plus a pad byte if req'd to word align.
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

typedef struct
{
	unsigned short operator;
	genAmountType val;
} igen;
typedef struct
{
	char achSampleName[20];
	DWORD dwStart;
	DWORD dwEnd;
	DWORD dwStartloop;
	DWORD dwEndloop;
	DWORD dwSampleRate;
	BYTE byOriginalKey;
	char chCorrection;
	int8_t wSampleLink;
	SFSampleLink sfSampleType;
} shdr;
typedef struct
{
	short delay, attack, hold, decay, release;
	unsigned short sustain;
} envelope;
typedef struct
{
	uint8_t lokey, hikey, lovel, hivel;
	shdr *sampl;
	short attentuation, lpf_cutff, lpf_q;
	float pitchAdjust;
	envelope vol, lfomod;
} zone;
typedef struct
{
	phdr *header;
	zone *zones;
} preset;

int main(int argc, char **argv)
{
	unsigned int size, size2L;

	printf("\nd %d", argc);
	char *filename = argc > 1 ? argv[1] : "Chaos.sf2";
	char *dbname = argc > 2 ? argv[2] : "grepsf2";
	FILE *fd = fopen(filename, "r");
	FILE *mysql, *debug, *rmysql;
	debug = fopen("debug.sql", "w+");
	mysql = popen("mysql -u root", "w");
	fprintf(mysql, "drop database if exists %s; create database %s;", dbname, dbname);

	fprintf(mysql, "use %s; source sf2.sql;source sf22.sql;", dbname);

	header_t *header = (header_t *)malloc(sizeof(header_t));
	header2_t *h2 = (header2_t *)malloc(sizeof(header2_t));
	fread(header, sizeof(header_t), 1, fd);
	printf("%.4s %.4s %.4s %u", header->name, header->sfbk, header->list, header->size);

	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u", h2->name, h2->size);
	fseek(fd, h2->size, SEEK_CUR);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u", h2->name, h2->size);
	fseek(fd, h2->size, SEEK_CUR);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u\n", h2->name, h2->size);
	int npresets, npbags, npgens, npmods, nshdrs, ninst, nimods, nigens, ishdrs, nibags;
	section_header *sh = (section_header *)malloc(sizeof(section_header));

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	npresets = sh->size / sizeof(phdr);
	phdr *phdrs = (phdr *)malloc(sh->size);
	fread(phdrs, sizeof(phdr), sh->size / sizeof(phdr), fd);
	for (int i = 0; i < npresets; i++)
	{
		fprintf(mysql, "insert ignore into phdr values (%d,\'%s\',%u,%u);", (phdrs + i)->pbagNdx, (phdrs + i)->name, (phdrs + i)->bankId, (phdrs + i)->pbagNdx);
	}

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	npbags = sh->size / sizeof(pbag);
	pbag *pbags = (pbag *)malloc(sh->size);
	fread(pbags, sizeof(pbag), sh->size / sizeof(pbag), fd);
	for (int i = 0; i < npbags; i++)
	{
		fprintf(mysql, "\ninsert ignore into pbag values (%d, %u,%u);", i, (pbags + i)->pgen_id, (pbags + i)->pmod_id); //, (phdrs + i)->bankId, (phdrs + i)->pbagNdx);
	}

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	npmods = sh->size / sizeof(pmod);
	pmod *pmods = (pmod *)malloc(sh->size);
	fread(pmods, sizeof(pmod), sh->size / sizeof(pmod), fd);

	for (int i = 0; i < npbags; i++)
	{
	}

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	npgens = sh->size / sizeof(pgen_t);
	pgen_t *pgens = (pgen_t *)malloc(sh->size);
	fread(pgens, sizeof(pgen_t), sh->size / sizeof(pgen_t), fd);
	for (int i = 0; i < npgens; i++)
	{
		fprintf(mysql, "insert ignore into pgen values (%d, %hu,%hu,%hu,%d);", i, (pgens + i)->operator,(pgens + i)->val.ranges.lo, (pgens + i)->val.ranges.hi, (pgens + i)->val.shAmount); //, (phdrs + i)->bankId, (phdrs + i)->pbagNdx);
	}

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	ninst = sh->size / sizeof(pgen_t);
	inst *insts = (inst *)malloc(sh->size);
	fread(insts, sizeof(inst), sh->size / sizeof(inst), fd);
	for (int i = 0; i < ninst; i++)
	{
		inst *ii = (insts + i);

		fprintf(mysql, "insert into inst ( id, ibag_id) value ");

		fprintf(mysql, "(%d, %u);", i, ii->ibagNdx);
	}
	fread(sh, sizeof(section_header), 1, fd);
	nibags = sh->size / sizeof(ibag);
	printf("%.4s:%u %u\n", sh->name, sh->size, nibags);
	ibag *ibags = (ibag *)malloc(sh->size);
	fread(ibags, sizeof(ibag), sh->size / sizeof(ibag), fd);
	for (int i = 0; i < nibags; i++)
	{
		fprintf(mysql, "insert ignore into ibag values (%d,%hu,%hu);", i, (ibags + i)->igen_id, (ibags + i)->imod_id); //", (pgens + i)->operator,(pgens + i)->val.ranges.lo, (pgens + i)->val.ranges.hi); //, (phdrs + i)->bankId, (phdrs + i)->pbagNdx);
	}

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	nimods = sh->size / sizeof(imod);
	imod *imods = (imod *)malloc(sh->size);
	fread(imods, sizeof(imod), sh->size / sizeof(imod), fd);

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	nigens = sh->size / sizeof(pgen_t);
	pgen_t *igens = (pgen_t *)malloc(sh->size);
	fread(igens, sizeof(pgen_t), sh->size / sizeof(pgen_t), fd);
	for (int i = 0; i < nigens - 1; i++)
	{
		//("%d", i);
		fprintf(mysql, "\ninsert ignore into igen (id,operator,lo,hi) values (%d, %hu, %hu, %hu);",
				i, (igens + i)->operator,(igens + i)->val.ranges.lo, (igens + i)->val.ranges.hi); //, (phdrs + i)->bankId, (phdrs + i)->pbagNdx);
																								  //printf("%d", i);
	}
	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	nshdrs = sh->size / sizeof(shdr);
	shdr *shdrs = (shdr *)malloc(sh->size);
	fread(shdrs, sizeof(shdr), sh->size / sizeof(shdr), fd);

	for (int i = 0; i < ninst - 1; i++)

	{
		for (int j = insts[i].ibagNdx; j < insts[i + 1].ibagNdx; j++)
		{

			for (int k = ibags[j].igen_id; k < ibags[j + 1].igen_id; k++)
			{
				fprintf(mysql, "insert into ibag_gen (ibag_id,igen_id) values (%d,%d);", j, k);
			}
		}
	}
	for (int i = 0; i < npresets - 1; i++)
	{
		for (int j = phdrs[i].pbagNdx; j < phdrs[i + 1].pbagNdx; j++)
		{
			printf("insert into preset_pbag (preset_id,bank_id,ibag_id) values (%d,%d,%d);", (phdrs + i)->pid, (phdrs + i)->bankId, j);
			fprintf(mysql, "insert into preset_pbag (preset_id,bank_id,ibag_id) values (%d,%d,%d);", (phdrs + i)->pid, (phdrs + i)->bankId, j);

			for (int k = pbags[j].pgen_id; k < pbags[j + 1].pgen_id; k++)
			{
				fprintf(mysql, "insert into pbag_gen values (%d,%d);", j, k);
			}
		}
	}
	pclose(fd);
}
static void safe_sql_str(FILE *file, int size, FILE *output)
{
	fputc('\'', output);
	for (int ci = 0; ci < size; ci++)
	{
		int c = fgetc(file);
		if (c == 20 || c >= 60 && c <= 122)
			fputc(c, output);
	}
	fputc('\'', output);
}