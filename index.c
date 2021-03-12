#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>

static void safe_sql_str(FILE *file, int size, FILE *output);
char *generator[60] = {"Gen_StartAddrOfs", "Gen_EndAddrOfs", "Gen_StartLoopAddrOfs", "Gen_EndLoopAddrOfs", "Gen_StartAddrCoarseOfs", "Gen_ModLFO2Pitch", "Gen_VibLFO2Pitch", "Gen_ModEnv2Pitch", "Gen_FilterFc", "Gen_FilterQ", "Gen_ModLFO2FilterFc", "Gen_ModEnv2FilterFc", "Gen_EndAddrCoarseOfs", "Gen_ModLFO2Vol", "Gen_Unused1", "Gen_ChorusSend", "Gen_ReverbSend", "Gen_Pan", "Gen_Unused2", "Gen_Unused3", "Gen_Unused4", "Gen_ModLFODelay", "Gen_ModLFOFreq", "Gen_VibLFODelay", "Gen_VibLFOFreq", "Gen_ModEnvDelay", "Gen_ModEnvAttack", "Gen_ModEnvHold", "Gen_ModEnvDecay", "Gen_ModEnvSustain", "Gen_ModEnvRelease", "Gen_Key2ModEnvHold", "Gen_Key2ModEnvDecay", "Gen_VolEnvDelay", "Gen_VolEnvAttack", "Gen_VolEnvHold", "Gen_VolEnvDecay", "Gen_VolEnvSustain", "Gen_VolEnvRelease", "Gen_Key2VolEnvHold", "Gen_Key2VolEnvDecay", "Gen_Instrument", "Gen_Reserved1", "Gen_KeyRange", "Gen_VelRange", "Gen_StartLoopAddrCoarseOfs", "Gen_Keynum", "Gen_Velocity", "Gen_Attenuation", "Gen_Reserved2", "Gen_EndLoopAddrCoarseOfs", "Gen_CoarseTune", "Gen_FineTune", "Gen_SampleId", "Gen_SampleModes", "Gen_Reserved3", "Gen_ScaleTune", "Gen_ExclusiveClass", "Gen_OverrideRootKey", "Gen_Dummy"};
static uint8_t readu8(FILE *file) { return (fgetc(file) & 0xff); }

static uint16_t read16(FILE *file)
{
	return (fgetc(file) << 8) | fgetc(file);
}
static uint32_t read32(FILE *file) { return (fgetc(file) << 24) | (fgetc(file) << 16) | (fgetc(file) << 8) | fgetc(file); }
int main(int argc, char **argv)
{

	char riff[4];
	unsigned int size, size2L;
	char name[20];
	char idc[12];
	FILE *mysql, *debug;
	printf("\nd %d", argc);
	char *filename = argc > 1 ? argv[1] : "Chaos.sf2";
	char *dbname = argc > 2 ? argv[2] : "grepsf2";
	mysql = popen("mysql -u root", "w");
	fprintf(mysql, "drop database if exists %s; create database %s;", dbname, dbname, dbname);

	fprintf(mysql, "use %s; source sf2.sql;", dbname);

	FILE *file = fopen(filename, "rb");

	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	fread(&size, 1, 4, file);
	printf("\n%u", size);
	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);

	fread(&size, 1, 4, file);
	printf("\n%u", size);
	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	fseek(file, size, SEEK_CUR);

	fread(&size, 1, 4, file);
	printf("\n%u", size);
	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	fseek(file, size, SEEK_CUR);

	fread(&size, 1, 4, file);
	printf("\n%u", size);
	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);

	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	//	assert(strcmp(riff,"pdta")==0);

	fread(&size, 1, 4, file);
	printf("\n%u kk", size);

	//	while(fgetc(file)==0x00){}
	printf("LOC-%lu", ftell(file));

	for (int i = 0; i < size; i += 38)
	{
		fprintf(mysql, "\n insert ignore into phdr (name, id, bank_id, pbag_id) values ");
		fputc('(', mysql);
		safe_sql_str(file, 20, mysql);
		fputc(',', mysql);
		fprintf(mysql, "%u, %u, %u);", read16(file), read16(file), read16(file));
		read32(file), read32(file), read32(file);
	}

	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);

	fread(&size, 1, 4, file);
	for (int i = 0; i < size / 4; i++)
	{
		fprintf(mysql, "\ninsert into pbag (id, pgen_id, pmod_id) values ");
		fprintf(mysql, " (%i, %u, %u ); ", i, read16(file), read16(file));
	}

	printf("LOC-%lu", ftell(file));

	fread(riff, 4, 1, file);
	fread(&size, 1, 4, file);
	printf("\n%u", size);

	fprintf(mysql, "\ninsert into pmod(id, mod_src, mod_dest, mod_amt, mod_amt_src, transpose) values "); //mod src, mod dest, mod amount, mod amt, src mod, transope");
	for (int i = 0; i < size / 10; i++)
	{
		fprintf(mysql, "(%i,%u,%u,%u,%u,%u),", i, read16(file), read16(file), read16(file), read16(file), read16(file));
	}
	fprintf(mysql, "(-1,0,0,0,0,0);");

	fread(riff, 4, 1, file);
	//assert(strcmp(riff, "pgen") == 0);
	fread(&size, 1, 4, file);

	for (int i = 0; i < size / 4; i++)
	{
		fprintf(mysql, "\ninsert into pgen(id,operator,lo,hi) values ");
		fprintf(mysql, "(%i, %hu,%hu,%hu);", i, read16(file), readu8(file), readu8(file));
	}

	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	fread(&size, 1, 4, file);
	printf("\n%u", size);

	for (int i = 0; i < size / 22; i++)
	{
		fprintf(mysql, "insert into inst (name, id, ibag_id) value ");
		fputc('(', mysql);
		safe_sql_str(file, 20, mysql);
		fputc(',', mysql);
		fprintf(mysql, "%i, %u);", i, read16(file));
	}

	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	fread(&size, 1, 4, file);
	printf("\n%u", size);

	for (int i = 0; i < size / 10; i++)
	{
		fprintf(mysql, "\ninsert into imod(id, mod_src, mod_dest, mod_amt, mod_amt_src, transpose) values "); //mod src, mod dest, mod amount, mod amt, src mod, transope");

		fprintf(mysql, "(%i,%u,%u,%u,%u,%u);", i, read16(file), read16(file), read16(file), read16(file), read16(file));
	}

	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	fread(&size, 1, 4, file);
	printf("\n%u", size);

	for (int i = 0; i < size / 4; i++)
	{
		fprintf(mysql, "\ninsert into igen (id,operator,lo,hi) values ");
		fprintf(mysql, " (%d, %hu, %hu,%hu); ", i, read16(file), readu8(file), readu8(file));
	}

	fread(riff, 4, 1, file);
	printf("\n%.4s", riff);
	fread(&size, 1, 4, file);
	printf("\n%u", size);

	for (int i = 0; i < size; i += (20 + 4 * 5 + 1 + 1 + 4))
	{
		//	fread(riff, 20, 1, file);
		// struct tsf_hydra_shdr { tsf_char20 sampleName; tsf_u32 start, end, startLoop, endLoop, sampleRate;
		// tsf_u8 originalPitch; tsf_s8 pitchCorrection; tsf_u16 sampleLink, sampleType; };

		fprintf(mysql, "insert into shdr (start,end,startLoop,endLoop,sampleRate,originalPitch,pitchCorrection,sampleLink,sampleType) values ");

		fread(name, 20, 1, file);
		fprintf(mysql, "(%u,%u,%u,%u,%u,", read32(file), read32(file), read32(file), read32(file), read32(file));
		fprintf(mysql, "%i,%i,%d,%d);", readu8(file), fgetc(file), read16(file), read16(file));
		if (name[0] == 'E' && name[1] == 'O' && name[2] == 'S')
		{
			break;
		}
	}

	pclose(mysql);
	return 0;
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