/* ecc-test.c: generate ECC values for input data */

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

void print_ecc(void);

int main(int argc, const char* argv[])
{
    FILE* fp;
    int i;
    int index;
    int read_size;
    
    block_size = eccBytesPerBlock();
    ecc_size   = eccNumBytes();
    printf("ECC block size = %d  ECC value size = %d\n", block_size, ecc_size);
    if (block_size > sizeof(block) || ecc_size > sizeof(ecc_value) )
    {
	fprintf(stderr, "Error max block size = %d and max ecc value size =%d\n", sizeof(block), sizeof(ecc_value));
	exit(2);
    }
    
    if (argc != 2)
    {
	fprintf(stderr, "Error, need input data filename as only argument\n");
	exit(2);
    }
    
    fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
	fprintf(stderr, "can't open pattern input file %s", argv[1]);
	exit(2);
    }
    
    /* each bit number in the first byte */
    for ( i=0; TRUE; i++)
    {
	block[0] = 1 << i;
	read_size = fread(block, 1, block_size, fp);
	if (feof(fp) && read_size == 0)
	{
	    break;
	}
	if (ferror(fp)) 
	{ 
	    fprintf(stderr, "error reading from input file\n"); 
	    exit(2);
	}
	if (read_size != block_size)
	{
	    fprintf(stderr, "ignoring extra %d bytes at end of input\n", read_size);
	    break;
	}
	eccComputeECC(block, ecc_value);
	printf("block %6d: ", i);
	print_ecc();
    }
}

void print_ecc(void)
{
    int i;
    
    for (i=0; i < ecc_size; i++)
    {
	printf("0x%2.2X ", ecc_value[i]);
    }
    printf("\n");
}