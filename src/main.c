/*
*
*	PSV Save Converter - (c) 2020 by Bucanero - www.bucanero.com.ar
*
* This tool is based on the ps3-psvresigner by @dots_tb (https://github.com/dots-tb/ps3-psvresigner)
*
* PS2 Save format code from:
*	- https://github.com/PMStanley/PSV-Exporter
*	- https://github.com/root670/CheatDevicePS2
*
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>

#include "aes.h"
#include "sha1.h"

#define SEED_OFFSET 0x8
#define HASH_OFFSET 0x1c
#define TYPE_OFFSET 0x3C

#define PSV_MAGIC 0x50535600

const uint8_t key[2][0x10] = {
							{0xFA, 0x72, 0xCE, 0xEF, 0x59, 0xB4, 0xD2, 0x98, 0x9F, 0x11, 0x19, 0x13, 0x28, 0x7F, 0x51, 0xC7}, 
							{0xAB, 0x5A, 0xBC, 0x9F, 0xC1, 0xF4, 0x9D, 0xE6, 0xA0, 0x51, 0xDB, 0xAE, 0xFA, 0x51, 0x88, 0x59}
						};
const uint8_t iv[0x10] = {0xB3, 0x0F, 0xFE, 0xED, 0xB7, 0xDC, 0x5E, 0xB7, 0x13, 0x3D, 0xA6, 0x0D, 0x1B, 0x6B, 0x2C, 0xDC};


int extractPSU(const char *save);
int extractMAX(const char *save);
int extractMCS(const char *save);
int extractPSX(const char *save);
int extractCBS(const char *save);
int extractXPS(const char *save);

char* endsWith(const char * a, const char * b)
{
	int al = strlen(a), bl = strlen(b);
    
	if (al < bl)
		return NULL;

	a += (al - bl);
	while (*a)
		if (toupper(*a++) != toupper(*b++)) return NULL;

	return (char*) (a - bl);
}

void XorWithByte(uint8_t* buf, uint8_t byte, int length)
{
	for (int i = 0; i < length; ++i) {
    	buf[i] ^= byte;
	}
}

static void usage(char *argv[])
{
	printf("This tool converts and resigns PS1/PS2 saves to PS3 .PSV savegame format.\n\n");
	printf("USAGE: %s <filename>\n\n", argv[0]);
	printf("INPUT FORMATS\n");
	printf(" .mcs            PS1 MCS File\n");
	printf(" .psx            PS1 AR/GS/XP PSX File\n");
	printf(" .cbs            PS2 CodeBreaker File\n");
	printf(" .max            PS2 ActionReplay Max File\n");
	printf(" .psu            PS2 EMS File (uLaunchELF)\n\n");
	return;
}

void generateHash(uint8_t *input, uint8_t *dest, size_t sz, int type) {
	struct AES_ctx aes_ctx;

	uint8_t salt[0x40];
	uint8_t work_buf[0x14];
		
	uint8_t *salt_seed = input + SEED_OFFSET;
	memset(salt , 0, sizeof(salt));
	
	printf("Type detected: %x\n", type);
	if(type == 1) {//PS1
		//idk why the normal cbc doesn't work.
		AES_init_ctx_iv(&aes_ctx, key[1], iv);
		memcpy(work_buf, salt_seed, 0x10);
		AES_ECB_decrypt(&aes_ctx, work_buf);
		memcpy(salt, work_buf, 0x10);

		memcpy(work_buf, salt_seed, 0x10);
		AES_ECB_encrypt(&aes_ctx, work_buf);
		memcpy(salt + 0x10, work_buf, 0x10);

		XorWithIv(salt, iv);
			
		memset(work_buf, 0xFF, sizeof(work_buf));
		memcpy(work_buf, salt_seed + 0x10, 0x4);
		XorWithIv(salt + 0x10, work_buf);
		
	} else if(type == 2) {//PS2
		uint8_t laid_paid[16]  = {	
			0x10, 0x70, 0x00, 0x00, 0x02, 0x00, 0x00, 0x01, 0x10, 0x70, 0x00, 0x03, 0xFF, 0x00, 0x00, 0x01 };

		memcpy(salt, salt_seed, 0x14);
		XorWithIv(laid_paid, key[0]);
		AES_init_ctx_iv(&aes_ctx, laid_paid, iv);
		AES_CBC_decrypt_buffer(&aes_ctx, salt, 0x40);
	}
	
	memset(salt + 0x14, 0, sizeof(salt) - 0x14);
	
	XorWithByte(salt, 0x36, 0x40);
		
	SHA1_CTX sha1_ctx_1;	
	SHA1Init(&sha1_ctx_1);
	
	SHA1Update(&sha1_ctx_1, salt, 0x40);

	memset(input + HASH_OFFSET, 0, 0x14);
	SHA1Update(&sha1_ctx_1, input, sz);
				
	XorWithByte(salt, 0x6A, 0x40);

	SHA1Final(work_buf, &sha1_ctx_1);

	SHA1_CTX sha1_ctx_2;
	SHA1Init(&sha1_ctx_2);
	SHA1Update(&sha1_ctx_2, salt, 0x40);
	SHA1Update(&sha1_ctx_2, work_buf, 0x14);

	SHA1Final(dest, &sha1_ctx_2);
}

void psv_resign(const char* src_file)
{
	FILE *fin = 0, *fout = 0;
	fin = fopen(src_file, "rb");
	if (!fin) {
		perror("Failed to open input file");
		goto error;
	}

	fseek(fin, 0, SEEK_END);
	size_t sz = ftell(fin);
	printf("\nPSV File Size: %ld bytes\n", sz);
	fseek(fin, 0, SEEK_SET);
	
	uint8_t *input = (unsigned char*) calloc (1, sz);
	uint32_t *input_ptr = (uint32_t*) input;
	fread(input, sz,1,fin);
	
	if(input_ptr[0] != PSV_MAGIC) {
		perror("Not a PSV file");
		free(input);
		goto error;
	}
	
	printf("Old signature: ");
	for(int i = 0; i < 0x14; i++ ) {
		printf("%02X ",  input[HASH_OFFSET + i]);
	}
	printf("\n");
	generateHash(input, input + HASH_OFFSET, sz, *(input + TYPE_OFFSET));
	
		
	printf("New signature: ");
	for(int i = 0; i < 0x14; i++ ) {
		printf("%02X ", input[HASH_OFFSET + i]);
	}
	printf("\n");

	fout = fopen(src_file, "wb");
	if (!fout) {
		perror("Failed to open output file");
		free(input);
		goto error;
	}
	fwrite(input,  1, sz, fout);
	free(input);
	printf("PSV resigned successfully: %s\n", src_file);


error:
	if (fin)
		fclose(fin);
	if (fout)
		fclose(fout);	

}

int main(int argc, char **argv)
{
	printf("\n psv-save-converter v1.1.0 - (c) 2020 by Bucanero\n");
	printf(" (based on ps3-psvresigner by @dots_tb)\n\n");

	if (argc != 2) {
		usage(argv);
		return 1;
	}

	if (endsWith(argv[1], ".max"))
		extractMAX(argv[1]);

	else if (endsWith(argv[1], ".psu"))
		extractPSU(argv[1]);

	else if (endsWith(argv[1], ".mcs"))
		extractMCS(argv[1]);

	else if (endsWith(argv[1], ".psx"))
		extractPSX(argv[1]);

	else if (endsWith(argv[1], ".cbs"))
		extractCBS(argv[1]);

	else if (endsWith(argv[1], ".xps"))
		extractXPS(argv[1]);

	else
		usage(argv);

	return 0;
}
