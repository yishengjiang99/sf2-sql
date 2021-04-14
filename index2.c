#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/stat.h>
#include "index.h"

int main(int argc, char **argv)
{

	unsigned int size, size2L;

	char *filename = argc > 1 ? argv[1] : "file.sf2";

	char *dbname = argc > 2 ? argv[2] : "grepsf2";

	FILE *fd = fopen(filename, "r");
	FILE *mysql, *debug, *rmysql;
	debug = fopen("debug.sql", "w+");
	mysql = popen("cat", "w");
	header_t *header = (header_t *)malloc(sizeof(header_t));
	header2_t *h2 = (header2_t *)malloc(sizeof(header2_t));
	fread(header, sizeof(header_t), 1, fd);
	printf("%.4s %.4s %.4s %u", header->name, header->sfbk, header->list, header->size);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u", h2->name, h2->size);
	for (int i = 0; i < h2->size; i++)
		putchar(fgetc(fd) & 0x7f);
	//	fseek(fd, h2->size, SEEK_CUR);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u", h2->name, h2->size);
	short *data = (short *)malloc(h2->size);
	int nsamples = h2->size / sizeof(short);
	fread(data, sizeof(short), nsamples, fd);
	fread(h2, sizeof(header2_t), 1, fd);
	printf("\n%.4s %u\n", h2->name, h2->size);
	int npresets, npbags, npgens, npmods, nshdrs, ninst, nimods, nigens, ishdrs, nibags;
	section_header *sh = (section_header *)malloc(sizeof(section_header));

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	npresets = sh->size / sizeof(phdr);
	phdr *phdrs = (phdr *)malloc(sh->size);
	fread(phdrs, sizeof(phdr), sh->size / sizeof(phdr), fd);
	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	npbags = sh->size / sizeof(pbag);
	pbag *pbags = (pbag *)malloc(sh->size);
	fread(pbags, sizeof(pbag), sh->size / sizeof(pbag), fd);
	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	npmods = sh->size / sizeof(pmod);
	pmod *pmods = (pmod *)malloc(sh->size);
	fread(pmods, sizeof(pmod), sh->size / sizeof(pmod), fd);
	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	npgens = sh->size / sizeof(pgen_t);
	pgen_t *pgens = (pgen_t *)malloc(sh->size);
	fread(pgens, sizeof(pgen_t), sh->size / sizeof(pgen_t), fd);
	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	ninst = sh->size / sizeof(pgen_t);
	inst *insts = (inst *)malloc(sh->size);
	fread(insts, sizeof(inst), sh->size / sizeof(inst), fd);
	fread(sh, sizeof(section_header), 1, fd);
	nibags = sh->size / sizeof(ibag);
	printf("%.4s:%u %u\n", sh->name, sh->size, nibags);
	ibag *ibags = (ibag *)malloc(sh->size);
	fread(ibags, sizeof(ibag), sh->size / sizeof(ibag), fd);
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

	fread(sh, sizeof(section_header), 1, fd);
	printf("%.4s:%u\n", sh->name, sh->size);
	uint8_t *shdrs = (uint8_t *)malloc(sh->size);
	fread(shdrs, 1, sh->size, fd);

	for (int i = 0; i < npresets - 1; i++)
	{
		if (phdrs[i].bankId != 0)
			continue;
		int lastbag = phdrs[i + 1].pbagNdx;
		int instID = -1;
		int lastSampId = -1;

		for (int j = phdrs[i].pbagNdx; j < lastbag; j++)
		{
			pbag *pg = pbags + j;
			pgen_t *lastg = pgens + pg[j + 1].pgen_id;
			int pgenId = pg->pgen_id;
			int lastPgenId = j < npbags - 1 ? pbags[j + 1].pgen_id : npgens - 1;
			for (int k = pgenId; k < lastPgenId; k++)
			{
				pgen_t *pge = pgens + k;

				if (pge->operator== 44 || pge->operator== 43)
				{
					printf("\n%d, pbag, %d, %s, %d,%s,val, %hu-%hu",
								 phdrs[i].pid, j,
								 phdrs[i].name, pge->operator, generator[pge->operator], pge->val.ranges.lo, pge->val.ranges.hi);
				}
				else
				{

					printf("\n%d, pbag, %d, %s, %d, %s, val, %d",
								 phdrs[i].pid, j,
								 phdrs[i].name, pge->operator, generator[pge->operator], pge->val.shAmount);
				}
				if (pge->operator== 41)
				{
					if (pge->val.shAmount != instID)
					{
						instID = pge->val.shAmount;
						inst *ihead = insts + instID;
						int ibgId = ihead->ibagNdx;
						int lastibg = (ihead + 1)->ibagNdx;
						for (int ibg = ibgId; ibg < lastibg; ibg++)
						{
							ibag *ibgg = ibags + ibg;
							pgen_t *lastig = ibg < nibags - 1 ? igens + (ibgg + 1)->igen_id : igens + nigens - 1;
							for (pgen_t *ig = igens + ibgg->igen_id; ig->operator!= 60 && ig != lastig; ig++)
							{

								if (ig->operator== 44 || ig->operator== 43)
								{
									printf("\n%d, inst, %d, %d, %s, ", phdrs[i].pid, ibg, instID, ihead->name);
									printf("%d, %s,val, %hu-%hu", ig->operator, generator[ig->operator], ig->val.ranges.lo, ig->val.ranges.hi);
								}
								else
								{
									printf("\n%d, inst, %d, %d, %s, ", phdrs[i].pid, ibg, instID, ihead->name);
									printf("%hu, %s, val, %d", ig->operator, generator[ig->operator], ig->val.shAmount);
								}
								if (ig->operator== 53)
								{
									lastSampId = ig->val.ranges.lo | (ig->val.ranges.hi << 8);

									shdrcast *sample = (shdrcast *)(shdrs + lastSampId * 46);

									printf("\n%d, samp, %d, %d, %.20s, %u,%u,%u,%u,%u",
												 phdrs[i].pid,
												 lastSampId,
												 instID,
												 sample->name,
												 sample->start, sample->end, sample->startloop, sample->endloop, sample->sampleRate);
								}
							}
						}
					}
				}
			}
		}
	}
	printf("\n");
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