#include <iostream>
#include <cstdio>
#include <cmath>

struct wav_header_t
{
	char chunkID[4]; //"RIFF" = 0x46464952
	unsigned long chunkSize; //28 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes] + sum(sizeof(chunk.id) + sizeof(chunk.size) + chunk.size)
	char format[4]; //"WAVE" = 0x45564157
	char subchunk1ID[4]; //"fmt " = 0x20746D66
	unsigned long subchunk1Size; //16 [+ sizeof(wExtraFormatBytes) + wExtraFormatBytes]
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long byteRate;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	//[WORD wExtraFormatBytes;]
	//[Extra format bytes]
};

struct chunk_t
{
	char ID[4]; //"data" = 0x61746164
	unsigned long size;  //Chunk data bytea
};

int WavReader(const char* fileName,int* channles,int* sampleRate,short** soundBuffer)
{
	FILE *fin = fopen(fileName, "rb");

	wav_header_t header;
	fread(&header, sizeof(header), 1, fin);

	*channles = header.numChannels;
	*sampleRate = header.sampleRate;

	chunk_t chunk;

	while (true)
	{
		fread(&chunk, sizeof(chunk), 1, fin);
		
		if (*(unsigned int*)&chunk.ID == 0x61746164)
		{
			break;
		}

		fseek(fin, chunk.size, SEEK_CUR);
	}

	int sample_size = header.bitsPerSample / 8;
	int samples_count = chunk.size * 8 / header.bitsPerSample;

	short int *value = new short int[samples_count];
	memset(value, 0, sizeof(short int) * samples_count);

	for (int i = 0; i < samples_count; i++)
	{
		fread(&value[i], sample_size, 1, fin);
	}

	*soundBuffer = value;

	fclose(fin);
	return samples_count;
}