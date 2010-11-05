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



/* Combine two boot tables. 
 * usage bfmerge file1 file2 [file3 [file4 [...]]]
 * The output is to stdout
 *
 * */

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


/* Copy a line from s to stdout */
void  lineCpy (FILE *s)
{
  char iline[132];

  fgets (iline, 131, s);
  fputs (iline, stdout);

}

/* Toss a line */
void lineToss (FILE *s)
{
  char iline[132];

  fgets (iline, 131, s);

}

/* Read a .b file. */
int readBFile (FILE *s, unsigned char *data, unsigned maxSize)
{
  unsigned char x, y;
  int byteCount = 0;

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

unsigned dwordConvert (unsigned char *data)
{
  unsigned value;
  unsigned char c[4];
  int i;

  c[0] = c[1] = c[2] = c[3] = 0;
  
  for (i = 0; i < 4; i++)  {
    c[i] = data[i];
  }

  value = c[3] | (c[2] << 8) | (c[1] << 16) | (c[0] << 24);

  return (value);

}


int bparse (unsigned char *dataSet, int l)
{
  unsigned int secsize;

  do  {

    secsize = dwordConvert (&dataSet[l]);
    secsize = (secsize + 3) & 0xfffffffc; /* segment pad */
    l = l + secsize + 8;  /* 4 bytes length, 4 bytes address */
  
  } while (secsize);

  /* Back up to the zero section size */
  l = l - 8;

  return (l);

}


#define SIZE    1000000
#define APPEND   100000

int main (int argc, char *argv[])
{
  FILE *strin;
  unsigned char *dataSet;
  unsigned char *appSet;
  unsigned int addr;
  unsigned int secsize;
  int asize;
  int i, j;
  int nbytes;
  int p;

  /* Must have at least two input files */ 
  if (argc < 3)  { 
    fprintf (stderr, "usage: %s file1 file2 [file3 [file4 [...]]]\n", argv[0]); 
    return (-1); 
  } 
  
  /* Allocate memory for the data set */ 
  dataSet = malloc (SIZE * sizeof (unsigned char));
  if (dataSet == NULL)  {
    fprintf (stderr, "%s: malloc failure\n", argv[0]);
    return (-1);
  }

  appSet = malloc (APPEND * sizeof (unsigned char ));
  if (appSet == NULL)  {
    fprintf (stderr, "%s malloc failure\n", argv[0]);
    return (-1);
  }


  /* The first data file is handled seperately */
  strin = fopen (argv[1], "r");
  if (strin == NULL)  {
    fprintf (stderr, "%s: Could not open file %s\n", argv[0], argv[1]);
    free (dataSet);
    return (-1);
  }

  /* Copy the first two lines of the input file to stdout */
  lineCpy (strin);
  lineCpy (strin);

  /* Read in the first data set */
  asize = readBFile (strin, dataSet, SIZE);
  fclose (strin);

  /* Parse the file, skipping the entry symbol */
  p = 4;
  p = bparse (dataSet, p);


  /* Add the remaining data sets */

  for (i = 2; i < argc; i++)  {

    strin = fopen (argv[i], "r");
    if (strin == NULL)  {
      fprintf (stderr, "%s: Could not open file %s\n", argv[0], argv[i]);
      free (dataSet);
      return (-1);
    }

    /* Toss the first two lines */
    lineToss (strin);
    lineToss (strin);

    asize = readBFile (strin, appSet, APPEND);
    fclose (strin);

    /* Append the new data set, skip the 4 bytes of entry symbol */
    for (j = 0; j < asize-4; j++)
      dataSet[p+j] = appSet[j+4];

    /* reparse from the current point */
    p = bparse (dataSet, p);

  }

  /* Add the 0 section size */
  for (i = 0; i < 4; i++)
    dataSet[p++] = 0;


  /* Write out the data */
  for (i = 0; i < p; i++)  {
    fprintf (stdout, "%02X", dataSet[i]);

    if ( ((i+1) % 24) )
      fprintf (stdout, " ");
    else
      fprintf (stdout, "\n");

  }

  /* Write the trailing character */
  fprintf (stdout, "\n%c",3);

  free (dataSet);

  return (0);

}

