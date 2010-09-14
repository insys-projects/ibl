/********************************************************************************
 * Simple test program that supports a binary blob load
 ********************************************************************************/


#pragma DATA_SECTION(nint_c0, ".data0")
int nint_c0 = 1024;

extern const unsigned int c0[];

unsigned int sum;

void main (void)
{

    int i;

    sum = 0;

    for (i = 0; i < nint_c0; i++)
        sum = sum + c0[i];


    for (;;);

}

    





