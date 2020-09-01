#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ps2mc.h"

void psv_resign(const char* src_file);
void get_psv_filename(char* psvName, const char* dirName);

int extractMCS(const char* mcsfile)
{
	char dstName[128];
	FILE *pf;
	psv_header_t psvh;
	uint8_t tmpFlags[] = {
		0x00, 0x20, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 
		0x03, 0x90, 0x00, 0x00};

	pf = fopen(mcsfile, "rb");
	if (!pf) {
		perror("Failed to open input file");
		return 0;
	}

	fseek(pf, 0, SEEK_END);
	size_t sz = ftell(pf);
	fseek(pf, 0, SEEK_SET);
	
	uint8_t *input = (unsigned char*) malloc(sz);
	fread(input, 1, sz, pf);
	fclose(pf);

	if (memcmp(input, "Q\x00", 2) != 0) {
		printf("Not a .mcs file\n");
		free(input);
		return 0;
	}
	
	get_psv_filename(dstName, (char*) &input[0x0A]);
	pf = fopen(dstName, "wb");
	if (!pf) {
		perror("Failed to open output file");
		free(input);
		return 0;
	}
	
	memset(&psvh, 0, sizeof(psv_header_t));
    psvh.headerSize = 0x00000014;
	psvh.saveType = 0x00000001;
    memcpy(&psvh.magic, "\0VSP", 4);
    memcpy(&psvh.salt, "www.bucanero.com.ar", 20);
    
    fwrite(&psvh, sizeof(psv_header_t), 1, pf);
	fwrite(tmpFlags, 0x24, 1, pf);

	memset(tmpFlags, 0, 0x20);
	memcpy(tmpFlags, input + 0x0A, 0x14);
	fwrite(tmpFlags, 0x20, 1, pf);

	fwrite(input + 0x80, sz - 0x80, 1, pf);
	fclose(pf);
	free(input);

	psv_resign(dstName);

	return 1;
}

int extractPSX(const char* mcsfile)
{
	char dstName[128];
	FILE *pf;
	psv_header_t psvh;
	uint8_t tmpFlags[] = {
		0x00, 0x20, 0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 
		0x03, 0x90, 0x00, 0x00};

	pf = fopen(mcsfile, "rb");
	if (!pf) {
		perror("Failed to open input file");
		return 0;
	}

	fseek(pf, 0, SEEK_END);
	size_t sz = ftell(pf);
	fseek(pf, 0, SEEK_SET);
	
	uint8_t *input = (unsigned char*) malloc(sz);
	fread(input, 1, sz, pf);
	fclose(pf);

	if (memcmp(&input[0x36], "SC", 2) != 0) {
		printf("Not a .psx file\n");
		free(input);
		return 0;
	}
	
	get_psv_filename(dstName, (char*) input);
	pf = fopen(dstName, "wb");
	if (!pf) {
		perror("Failed to open output file");
		free(input);
		return 0;
	}
	
	memset(&psvh, 0, sizeof(psv_header_t));
    psvh.headerSize = 0x00000014;
	psvh.saveType = 0x00000001;
    memcpy(&psvh.magic, "\0VSP", 4);
    memcpy(&psvh.salt, "www.bucanero.com.ar", 20);
    
    fwrite(&psvh, sizeof(psv_header_t), 1, pf);
	fwrite(tmpFlags, 0x24, 1, pf);

	memset(tmpFlags, 0, 0x20);
	memcpy(tmpFlags, input, 0x14);
	fwrite(tmpFlags, 0x20, 1, pf);

	fwrite(input + 0x36, sz - 0x36, 1, pf);
	fclose(pf);
	free(input);

	psv_resign(dstName);

	return 1;
}
