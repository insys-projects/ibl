/*******************************************************************************
 * FILE PURPOSE: A very simple test program used to verify IBL loading
 *******************************************************************************
 * FILE NAME: test1.c
 *
 * DESCRIPTION: A simple program that simply verifies the loading of
 *				several sections.
 *
 *  @file  test1.c
 *
 *  @brief
 *		Verify the IBL
 *
 ********************************************************************************/

/**
 *  @brief Test loading some const sections
 */
const unsigned int c1   = 0x12345678;
const unsigned int c2[] = {  0x11111111, 0x22222222 };


/**
 *  @brief Test load the cinit section
 */
unsigned int cini1 = 0xabcd1234;


/**
 * @brief The bss/far sections should not actually load
 */
unsigned int cbss[100];


extern cregister volatile unsigned int TSCL;
/**
 * @brief The text section which will include a switch
 */
void main (void)
{
    unsigned int v;

    /* Start the system timer */
    TSCL = 1;


    /* Read the timer and put the value in the bss */
    v = TSCL;

    cbss[0] = v;

    switch (v & 1)  {

        case 0: cbss[1] = c2[0];
                break;

        case 1: cbss[2] = c2[1];
                break;

    }

    for (;;);

}





 



