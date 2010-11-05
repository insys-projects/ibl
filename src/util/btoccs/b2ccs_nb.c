/*
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/



/* Second attempt to create the ccs version file for testing the rom boot loader */
/* No i2c blocking is performed */

#include <stdio.h>
#include <malloc.h>


int asciiByte (unsigned char c)
{
  if ((c >= '0') && (c <= '9'))
    return (1);

  if ((c >= 'A') && (c <= 'F'))
    return (1);

  return (0);
}

int toNum (unsigned char c)
{
  if ((c >= '0') && (c <= '9'))
    return (c - '0');

  return (c - 'A' + 10);

}


void  stripLine (FILE *s)
{
  char iline[132];

  fgets (iline, 131, s);

}

/* Read a .b file. */
int readBFile (FILE *s, unsigned char *data, unsigned maxSize)
{
  unsigned char x, y;
  int byteCount = 0;

  /* Strip the 1st two lines */
  stripLine (s);
  stripLine (s);

  for (;;) {

    /* read the 1st ascii char */
    do  {
      x = fgetc (s);
      if (x == (unsigned char)EOF)
        return (byteCount);

    } while (!asciiByte(x));

    /* Read the next ascii char */
    y = fgetc (s);
    if (y == (unsigned char)EOF)
      return (byteCount);
    if (asciiByte(y))
      data[byteCount++] = (toNum(x) << 4) | toNum (y);

    if (byteCount >= maxSize)  {
      fprintf (stderr, "Max input array size exceeded\n");
      return (-1);
    }

  }


}


int copyBlock (unsigned char *source, int idx, int maxSize, unsigned char *dest, int count)
{
  int i;

  for (i = 0; i < count; i++)  {
    if (idx >= maxSize)
      break;
    dest[i] = source[idx++];
  }

  return (i);

}

unsigned dwordConvert (unsigned char *data, int idx, int iMax)
{
  unsigned value;
  unsigned char c[4];
  int i;

  c[0] = c[1] = c[2] = c[3] = 0;
  
  for (i = 0; i < 4; i++)  {
    if (idx >= iMax)
      break;
    c[i] = data[idx++];
  }

  value = c[0] | (c[1] << 8) | (c[2] << 16) | (c[3] << 24);

  return (value);

}



#define SIZE	0x10000   /* max array size */

int main (int argc, char *argv[])
{
  FILE *strin;
  FILE *strout;

  unsigned char *dataSet1;

  unsigned pIn;
  unsigned pOut;

  int inSize;
  int i;

  /* Arg check */
  if (argc != 3)  {
    fprintf (stderr, "usage: %s infile outfile\n", argv[0]);
    return (-1);
  }

  /* Open the input file */
  strin = fopen (argv[1], "r");
  if (strin == NULL)  {
    fprintf (stderr, "%s: Could not open file %s for reading\n", argv[0], argv[1]);
    return (-1);
  }

  /* Allocate the two data set memories */
  dataSet1 = malloc (SIZE * sizeof (unsigned char));
  if (dataSet1 == NULL)  {
    fprintf (stderr, "%s: Malloc failure\n", argv[0]);
    return (-1);
  }

  /* Read the data into the byte stream */
  if ((inSize = readBFile (strin, dataSet1, SIZE)) < 0)
    return (inSize);
  fclose (strin);

  /* Copy the resulting data set into the output file in ccs format */
  strout = fopen (argv[2], "w");
  if (strout == NULL)  {
    fprintf (stderr, "%s: Could not open %s for writing\n", argv[0], argv[2]);
    return (-1);
  }

  /* Write the ccs header. The number of lines must be computed from the
   * array size */
  fprintf (strout, "1651 1 b000 1 %x\n", (inSize + 3) / 4);

  for (i = 0; i < inSize; i += 4)
    fprintf (strout, "0x%08x\n", dwordConvert (dataSet1, i, inSize));

  fclose (strout);

  return (0);

}



    




