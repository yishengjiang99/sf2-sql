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
	FILE *obf;
	shdrcast *sample;

	char *filename = argc > 1 ? argv[1] : "file.sf2";
	int pid = 0, bankId = 0, vel = -1, key = -1;
	if (argc > 2)
		pid = atoi(argv[2]);
	if (argc > 3)
		key = atoi(argv[3]);
	if (argc > 4)
		key = atoi(argv[4]);

	readsf(argc, argv);

	for (int i = 0; i < nphdrs - 1; i++)
	{
		int lastbag = phdrs[i + 1].pbagNdx;
		int instID = -1;
		int lastSampId = -1;
		int sampOutOffset = 0;
		if (phdrs[i].bankId != 0)
			continue;

		char sampleFileName[1024];
		sprintf(sampleFileName, "info_%s_%d_%d.js", filename, phdrs[i].pid, phdrs[i].bankId);
		obf = fopen(sampleFileName, "w");

		fprintf(obf, "\n export function find_%d_%d_presets(key, velocity){ ", phdrs[i].pid, phdrs[i].bankId);

		fprintf(obf, "\n \t const preset_%d_%d_zones =[];", phdrs[i].pid, phdrs[i].bankId);

		for (int j = phdrs[i].pbagNdx; j < lastbag; j++)
		{
			int jsIndent = 0;
			fprintf(obf, "\n /* %d %d */\n const presetDefault =  new SFZone();", phdrs[i].pid, phdrs[i].bankId);
			pbag *pg = pbags + j;
			pgen_t *lastg = pgens + pg[j + 1].pgen_id;
			int pgenId = pg->pgen_id;
			int lastPgenId = j < npbags - 1 ? pbags[j + 1].pgen_id : npgens - 1;
			for (int k = pgenId; k < lastPgenId; k++)
			{
				pgen_t *pge = pgens + k;
				if (j == phdrs[i].pbagNdx)
				{
					fprintf(obf
					, "\n presetDefault.apply(new SFGenerator(sf_gen_ids.%s,%d)); //%hu,%hu",
							 generator[pge->operator],pge->val.shAmount, pge->val.ranges.lo, pge->val.ranges.hi);
				}
				int jsIndent = 0;
				if (pge->operator== 44) // || pge->operator== 43)
				{
					jsIndent++;
					fprintf(obf, "\nif(val>%d && vel < %d) { ", pge->val.ranges.lo, pge->val.ranges.hi);
				}
				else if (pge->operator== 43) // || pge->operator== 43)
				{
					fprintf(obf, "\t if(key>%d && key < %d) { \n", pge->val.ranges.lo, pge->val.ranges.hi);

					if (key > -1 && (pge->val.ranges.lo > key || pge->val.ranges.hi < key))
						break;
				}
				if (pge->operator== 41)
				{
					instID = pge->val.shAmount;
					inst *ihead = insts + instID;
					int ibgId = ihead->ibagNdx;
					int lastibg = (ihead + 1)->ibagNdx;

					for (int ibg = ibgId; ibg < lastibg; ibg++)
					{
						int ijsindent = 0;

						ibag *ibgg = ibags + ibg;
						pgen_t *lastig = ibg < nibags - 1 ? igens + (ibgg + 1)->igen_id : igens + nigens - 1;

						for (pgen_t *ig = igens + ibgg->igen_id; ig->operator!= 60 && ig != lastig; ig++)
						{

							if (ibg == ibgId)
							{
								fprintf(obf
								, "\n izone.apply(new SFGenerator(sf_gen_ids.%s,%d)); //%hu,%hu",
											generator[ig->operator], ig->val.shAmount, ig->val.ranges.lo, ig->val.ranges.hi);
							}

							if (ig->operator== 44)
							{

								ijsindent++;
								fprintf(obf, "\n if(vel>%d && vel < %d) { \n", ig->val.ranges.lo, ig->val.ranges.hi);
								fprintf(obf
								, "\n\t izone.apply(new SFGenerator(sf_gen_ids.%s,%d)); //%hu,%hu",
									generator[ig->operator], ig->val.shAmount, ig->val.ranges.lo, ig->val.ranges.hi);
							}
							else if (ig->operator== 43) // || pge->operator== 43)
							{

								ijsindent++;
								fprintf(obf, "\nif(key>%d && key < %d) { \n", ig->val.ranges.lo, ig->val.ranges.hi);
							}
							else
							{
								 fprintf(obf
								 , "\n\tizone.apply(new SFGenerator(sf_gen_ids.%s,%d)); //%hu,%hu",
											generator[ig->operator], ig->val.shAmount, ig->val.ranges.lo, ig->val.ranges.hi);
							}

							if (ig->operator== 53)
							{
								lastSampId = ig->val.ranges.lo | (ig->val.ranges.hi << 8);

								shdrcast *sample = malloc(46);
								sample = (shdrcast *)(shdrs + lastSampId * 46);
								//		fwrite(data + sample->start * 2, sizeof(short), (sample->end - sample->start), samplePCM);
								fprintf(obf, "\n\tizone.sampleID = %d;", lastSampId);
								fprintf(obf, "\n\tizone.sample.start = %d;", sampOutOffset);
								fprintf(obf, "\n\tizone.sample.startLoop = %d;", sample->startloop - sample->start + sampOutOffset);
								fprintf(obf, "\n\tizone.sample.endLoop = %d;", sample->endloop - sample->start + sampOutOffset);
								fprintf(obf, "\n\tizone.sample.end = %d;", sample->end - sample->start + sampOutOffset);
								fprintf(obf, "\n\tizone.sample.sampleRate = %d;", sample->sampleRate);
								fprintf(obf, "\n\tizone.sample.originalPitch = %d;", sample->originalPitch);

								sampOutOffset += (sample->end - sample->start) * 2;
							}
						}
						while (ijsindent-- > 0)
							fprintf(obf, "\n }");
					}
				}
				fprintf(obf, "\nfor (const g of Object.values(pzone.generators)) izone.apply(g, -1);\n");
				fprintf(obf, "\n preset_%d_%d_zones.push(izone);", phdrs[i].pid, phdrs[i].bankId);
				while (jsIndent-- > 0)
					fprintf(obf, "\n }");
			}
		}
		fprintf(obf, "} /*end of %d %d */", phdrs[i].bankId, phdrs[i].pid);
		fclose(obf);
	}

	fprintf(obf, "\n");
}
