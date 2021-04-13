#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "read.c"

void find(int pid, int bankId, int vel, int key);
int main(int argc, char **argv)
{
	char *filename = argc > 1 ? argv[1] : "file.sf2";
	int pid = 0, bankId = 0, vel = -1, key = -1;
	if (argc > 2)
		pid = atoi(argv[2]);
	if (argc > 3)
		key = atoi(argv[3]);
	if (argc > 4)
		key = atoi(argv[4]);

	readsf(argc, argv);

	shdrcast *sample;
	for (int i = 0; i < nphdrs - 1; i++)
	{
		int lastbag = phdrs[i + 1].pbagNdx;
		int instID = -1;
		int lastSampId = -1;
		int sampOutOffset = 0;
		if (bankId > -1 && phdrs[i].bankId != bankId)
			continue;
		if (pid > -1 && phdrs[i].pid != pid)
			continue;

		char sampleFileName[1024];
		sprintf(sampleFileName, "sample_%s_%d.pcm", filename, pid);
		FILE *samplePCM = fopen(sampleFileName, "wb+");

		for (int j = phdrs[i].pbagNdx; j < lastbag; j++)
		{
			pbag *pg = pbags + j;
			pgen_t *lastg = pgens + pg[j + 1].pgen_id;
			int pgenId = pg->pgen_id;
			int lastPgenId = j < npbags - 1 ? pbags[j + 1].pgen_id : npgens - 1;
			for (int k = pgenId; k < lastPgenId; k++)
			{
				pgen_t *pge = pgens + k;
				if (j == phdrs[i].pbagNdx)
				{
				}
				if (pge->operator== 44) // || pge->operator== 43)
				{
					if (vel > -1 && (pge->val.ranges.lo > vel || pge->val.ranges.hi < vel))
						break;
				}
				else if (pge->operator== 43) // || pge->operator== 43)
				{
					if (key > -1 && (pge->val.ranges.lo > key || pge->val.ranges.hi < key))
						break;
				}
				fprintf(stdout, "\n%d, %s, %d, %s",
								phdrs[i].pid, j == phdrs[i].pbagNdx ? "defp" : "pbag", j,
								phdrs[i].name);
				fprintf(stdout, "%hu, %10s, val, %d,%hu,%hu",
								pge->operator, generator[pge->operator],
																				 pge->val.shAmount, pge->val.ranges.lo, pge->val.ranges.hi);

				if (pge->operator== 41 && pge->val.shAmount != instID)
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

							if (ibg == ibgId)
							{
								fprintf(stdout, "\n%d, defi, %d, %10s,", phdrs[i].pid, ibg, ihead->name);
								fprintf(stdout, "%hu, %10s, val, %d", ig->operator, generator[ig->operator], ig->val.shAmount);
								continue;
							}

							if (ig->operator== 44)
							{
								if (vel > -1 && (ig->val.ranges.lo > vel || ig->val.ranges.hi < vel))
									break;
								fprintf(stdout, "\n%d, inst, %d, %10s,", phdrs[i].pid, ibg, ihead->name);

								fprintf(stdout, "%hu, %10s, range, %hu-%hu", ig->operator, generator[ig->operator], pge->val.ranges.lo, pge->val.ranges.hi);
							}
							else if (ig->operator== 43) // || pge->operator== 43)
							{
								if (key > -1 && (ig->val.ranges.lo > key || ig->val.ranges.hi < key))
									break;
								fprintf(stdout, "\n%d, inst, %d, %10s,", phdrs[i].pid, ibg, ihead->name);

								fprintf(stdout, "%hu, %s, range, %hu-%hu", ig->operator, generator[ig->operator], pge->val.ranges.lo, pge->val.ranges.hi);
							}
							else
							{
								fprintf(stdout, "\n%d, inst, %d, %10s,", phdrs[i].pid, ibg, ihead->name);

								fprintf(stdout, "%hu, %10s, val, %d", ig->operator, generator[ig->operator], ig->val.shAmount);
							}

							if (ig->operator== 53)
							{
								lastSampId = ig->val.ranges.lo | (ig->val.ranges.hi << 8);

								sample = (shdrcast *)(shdrs + lastSampId * 46);
								fwrite(data + sample->start * 2, sizeof(short), (sample->end - sample->start), samplePCM);
								fprintf(stdout, "\n%d, samp, %d, %d, %.20s, %u,%u,%u,%u,%u,%u",
												phdrs[i].pid,
												lastSampId,
												instID,
												sample->name,
												sampOutOffset,
												sample->end - sample->start + sampOutOffset,
												sample->startloop - sample->start + sampOutOffset,
												sample->endloop - sample->start + sampOutOffset,
												sample->sampleRate,
												sample->originalPitch);
							}
						}
					}
				}
			}
		}
	}

	fprintf(stdout, "\n");
}
