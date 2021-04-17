unsigned int readheaders(FILE *file, char *section)
{
	char riff[4];
	unsigned int size;

	fread(riff, 4, 1, file);
	fread(&size, 1, 4, file);
	fread(riff, 4, 1, file);
	fread(riff, 4, 1, file);
	while (1)
	{
		fread(&size, 1, 4, file);
		fread(riff, 4, 1, file);
		if (strcmp(riff, section) == 0)
		{
			return ftell(file);
		}
		else
		{
			fseek(file, size, SEEK_CUR);
		}
	}
}