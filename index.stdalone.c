#include <stdint.h>
char *generator[60] = {"Gen_StartAddrOfs", "Gen_EndAddrOfs", "Gen_StartLoopAddrOfs", "Gen_EndLoopAddrOfs", "Gen_StartAddrCoarseOfs", "Gen_ModLFO2Pitch", "Gen_VibLFO2Pitch", "Gen_ModEnv2Pitch", "Gen_FilterFc", "Gen_FilterQ", "Gen_ModLFO2FilterFc", "Gen_ModEnv2FilterFc", "Gen_EndAddrCoarseOfs", "Gen_ModLFO2Vol", "Gen_Unused1", "Gen_ChorusSend", "Gen_ReverbSend", "Gen_Pan", "Gen_Unused2", "Gen_Unused3", "Gen_Unused4", "Gen_ModLFODelay", "Gen_ModLFOFreq", "Gen_VibLFODelay", "Gen_VibLFOFreq", "Gen_ModEnvDelay", "Gen_ModEnvAttack", "Gen_ModEnvHold", "Gen_ModEnvDecay", "Gen_ModEnvSustain", "Gen_ModEnvRelease", "Gen_Key2ModEnvHold", "Gen_Key2ModEnvDecay", "Gen_VolEnvDelay", "Gen_VolEnvAttack", "Gen_VolEnvHold", "Gen_VolEnvDecay", "Gen_VolEnvSustain", "Gen_VolEnvRelease", "Gen_Key2VolEnvHold", "Gen_Key2VolEnvDecay", "Gen_Instrument", "Gen_Reserved1", "Gen_KeyRange", "Gen_VelRange", "Gen_StartLoopAddrCoarseOfs", "Gen_Keynum", "Gen_Velocity", "Gen_Attenuation", "Gen_Reserved2", "Gen_EndLoopAddrCoarseOfs", "Gen_CoarseTune", "Gen_FineTune", "Gen_SampleId", "Gen_SampleModes", "Gen_Reserved3", "Gen_ScaleTune", "Gen_ExclusiveClass", "Gen_OverrideRootKey", "Gen_Dummy"};
typedef struct
{
	uint8_t lo, hi;
} rangesType;

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
		pgen_t igen;

typedef struct
{
	char name[20];
	uint32_t start, end, startloop, endloop, sampleRate;
	uint8_t originalPitch, pitchCorrection, v2, v3, v4, v5;

} shdrcast;

extern int *fopen(char *filename, char *mode);
extern void fread(void *dest, int width, int number, int *fd);
extern char fgetc(int *fd);
extern void ff(char *str, ...);
extern void *malloc(int size);

__attribute__((visibility("default"))) int memread(char *url)
{
	unsigned int size, size2L;

	char *filename = url;

	int *fd = fopen(filename, "r");
	header_t *header = (header_t *)malloc(sizeof(header_t));
	header2_t *h2 = (header2_t *)malloc(sizeof(header2_t));
	fread(header, sizeof(header_t), 1, fd);
	ff("%.4s %.4s %.4s %u", header->name, header->sfbk, header->list, header->size);
	fread(h2, sizeof(header2_t), 1, fd);
	ff("\n%.4s %u", h2->name, h2->size);
	for (int i = 0; i < h2->size; i++)
		fgetc(fd);
	//putchar(fgetc(fd) & 0x7f);
	//	fseek(fd, h2->size, SEEK_CUR);
	fread(h2, sizeof(header2_t), 1, fd);
	ff("\n%.4s %u", h2->name, h2->size);
	short *data = (short *)malloc(h2->size);
	int nsamples = h2->size / sizeof(short);
	fread(data, sizeof(short), nsamples, fd);
	fread(h2, sizeof(header2_t), 1, fd);
	ff("\n%.4s %u\n", h2->name, h2->size);
	int npresets, npbags, npgens, npmods, nshdrs, ninst, nimods, nigens, ishdrs, nibags;
	section_header *sh = (section_header *)malloc(sizeof(section_header));

	fread(sh, sizeof(section_header), 1, fd);
	ff("%.4s:%u\n", sh->name, sh->size);
	npresets = sh->size / sizeof(phdr);

	phdr *phdrs = (phdr *)malloc(sh->size);
	fread(phdrs, sizeof(phdr), sh->size / sizeof(phdr), fd);
	fread(sh, sizeof(section_header), 1, fd);
	ff("%.4s:%u\n", sh->name, sh->size);
	npbags = sh->size / sizeof(pbag);
	pbag *pbags = (pbag *)malloc(sh->size);
	fread(pbags, sizeof(pbag), sh->size / sizeof(pbag), fd);
	fread(sh, sizeof(section_header), 1, fd);
	ff("%.4s:%u\n", sh->name, sh->size);
	npmods = sh->size / sizeof(pmod);
	pmod *pmods = (pmod *)malloc(sh->size);
	fread(pmods, sizeof(pmod), sh->size / sizeof(pmod), fd);
	fread(sh, sizeof(section_header), 1, fd);
	ff("%.4s:%u\n", sh->name, sh->size);
	npgens = sh->size / sizeof(pgen_t);
	pgen_t *pgens = (pgen_t *)malloc(sh->size);
	fread(pgens, sizeof(pgen_t), sh->size / sizeof(pgen_t), fd);
	fread(sh, sizeof(section_header), 1, fd);
	ff("%.4s:%u\n", sh->name, sh->size);
	ninst = sh->size / sizeof(pgen_t);
	inst *insts = (inst *)malloc(sh->size);
	fread(insts, sizeof(inst), sh->size / sizeof(inst), fd);
	fread(sh, sizeof(section_header), 1, fd);
	nibags = sh->size / sizeof(ibag);
	ff("%.4s:%u %u\n", sh->name, sh->size, nibags);
	ibag *ibags = (ibag *)malloc(sh->size);
	fread(ibags, sizeof(ibag), sh->size / sizeof(ibag), fd);
	fread(sh, sizeof(section_header), 1, fd);
	ff("%.4s:%u\n", sh->name, sh->size);
	nimods = sh->size / sizeof(imod);
	imod *imods = (imod *)malloc(sh->size);
	fread(imods, sizeof(imod), sh->size / sizeof(imod), fd);

	fread(sh, sizeof(section_header), 1, fd);
	ff("%.4s:%u\n", sh->name, sh->size);
	nigens = sh->size / sizeof(pgen_t);
	pgen_t *igens = (pgen_t *)malloc(sh->size);
	fread(igens, sizeof(pgen_t), sh->size / sizeof(pgen_t), fd);

	fread(sh, sizeof(section_header), 1, fd);
	ff("%.4s:%u\n", sh->name, sh->size);
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
					ff("\n%d, pbag, %d, %s, %d,%s,val, %hu-%hu",
						 phdrs[i].pid, j,
						 phdrs[i].name, pge->operator, generator[pge->operator], pge->val.ranges.lo, pge->val.ranges.hi);
				}
				else
				{

					ff("\n%d, pbag, %d, %s, %d, %s, val, %d",
						 phdrs[i].pid, j,
						 phdrs[i].name, pge->operator, generator[pge->operator], pge->val.shAmount);
				}
				if (pge->operator== 41)
				{
					instID = pge->operator;
				}
			}
			if (instID != -1)
			{

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
							ff("\n%d, inst, %d, %d, %s, ", phdrs[i].pid, ibg, instID, ihead->name);
							ff("%d, %s,val, %hu-%hu", ig->operator, generator[ig->operator], ig->val.ranges.lo, ig->val.ranges.hi);
						}
						else
						{
							ff("\n%d, inst, %d, %d, %s, ", phdrs[i].pid, ibg, instID, ihead->name);
							ff("%hu, %s, val, %d", ig->operator, generator[ig->operator], ig->val.shAmount);
						}
						if (ig->operator== 53)
						{
							lastSampId = ig->val.ranges.lo | (ig->val.ranges.hi << 8);

							shdrcast *sample = (shdrcast *)(shdrs + lastSampId * 46);

							ff("\n%d, samp, %d, %d, %.20s, %u,%u,%u,%u,%u",
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
