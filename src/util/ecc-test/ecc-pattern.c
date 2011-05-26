/* ecc-pattern.c: generate ECC test patterns */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "types.h"
#include "ecc/ecc.h"

uint8_t block[4096];
uint8_t ecc_value[64];
int block_size;
int ecc_size;

int main(int argc, const char* argv[])
{
    FILE* fp;
    int i;
    int index;
    
    block_size = eccBytesPerBlock();
    ecc_size   = eccNumBytes();
    printf("ECC block size = %d  ECC value size = %d\n", block_size, ecc_size);
    if (block_size > sizeof(block) || ecc_size > sizeof(ecc_value) )
    {
	fprintf(stderr, "Error max block size = %d and max ecc value size =%d\n", sizeof(block), sizeof(ecc_value));
	exit(2);
    }
    
    fp = fopen("ecc-pattern.bin", "wb");
    if (fp == NULL)
    {
	fprintf(stderr, "can't open pattern output file");
	exit(2);
    }
    
    /* start with all zeros */
    memset(block, 0, block_size);
    fwrite(block, 1, block_size, fp);
    
    /* each bit number in the first byte */
    for (i=0; i < 8; i++)
    {
	block[0] = 1 << i;
	fwrite(block, 1, block_size, fp);
    }

    /* first bit of each byte on a power of 2 */
    block[0] = 0;
    for (index = 1; index < block_size; index = index << 1)
    {
	block[index] = 1;
	fwrite(block, 1, block_size, fp);
	block[index] = 0;
    }
}
