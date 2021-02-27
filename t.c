#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#define CID(a, b, c, d) (((d) << 24) + ((c) << 16) + ((b) << 8) + ((a)))
static int get8(FILE *f)
{
	return fgetc(f);
}
#define CID_RIFF CID('R', 'I', 'F', 'F')
#define CID_LIST CID('L', 'I', 'S', 'T')
#define CID_INFO CID('I', 'N', 'F', 'O')
#define CID_sdta CID('s', 'd', 't', 'a')
#define CID_snam CID('s', 'n', 'a', 'm')
#define CID_smpl CID('s', 'm', 'p', 'l')
#define CID_pdta CID('p', 'd', 't', 'a')
#define CID_phdr CID('p', 'h', 'd', 'r')
#define CID_pbag CID('p', 'b', 'a', 'g')
#define CID_pmod CID('p', 'm', 'o', 'd')
#define CID_pgen CID('p', 'g', 'e', 'n')
#define CID_inst CID('i', 'n', 's', 't')
#define CID_ibag CID('i', 'b', 'a', 'g')
#define CID_imod CID('i', 'm', 'o', 'd')
#define CID_igen CID('i', 'g', 'e', 'n')
#define CID_shdr CID('s', 'h', 'd', 'r')
#define CID_ifil CID('i', 'f', 'i', 'l')
#define CID_isng CID('i', 's', 'n', 'g')
#define CID_irom CID('i', 'r', 'o', 'm')
#define CID_iver CID('i', 'v', 'e', 'r')
#define CID_INAM CID('I', 'N', 'A', 'M')
#define CID_IPRD CID('I', 'P', 'R', 'D')
#define CID_ICOP CID('I', 'C', 'O', 'P')
#define CID_sfbk CID('s', 'f', 'b', 'k')
#define CID_ICRD CID('I', 'C', 'R', 'D')
#define CID_IENG CID('I', 'E', 'N', 'G')
#define CID_ICMT CID('I', 'C', 'M', 'T')
#define CID_ISFT CID('I', 'S', 'F', 'T')

static void safe_sql_str(FILE *file, int size, FILE *output);

int main(int argc, char **argv)
{
	char *filename = argc > 1 ? argv[1] : "file.sf2";
	FILE *file = fopen(filename, "rb");
	char riff[4];
	int section;
	unsigned int size;

	fread(riff, 4, 1, file);
	fread(&size, 1, 4, file);
	fread(riff, 4, 1, file);
	fread(riff, 4, 1, file);

	while (1)
	{
		fread(&size, 1, 4, file);
		fread(&section, 4, 1, file);
		printf("\n%u %i", size, section);
		switch (section)
		{
		case CID_pdta:
			while (1)
			{
			}
			break;
		default:
			fseek(file, size, SEEK_CUR);
		}
	}
	return 1;
}