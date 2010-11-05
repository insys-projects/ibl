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



#include <stdio.h>

/* Program to convert a ccs file to a b format file. */


int main (int argc, char *argv[])
{
  FILE *strin;
  FILE *strout;
  int a, b, c, d, i;
  int nwords;
  char iline[132];

  if (argc != 3)  {
    fprintf (stderr, "usage: %s infile outfile\n", argv[0]);
	return (-1);
  }

  strin = fopen (argv[1], "r");
  if (strin == NULL)  {
    fprintf (stderr, "%s: could not open input file %s\n", argv[0], argv[1]);
    return (-1);
  }

  strout = fopen (argv[2], "w");
  if (strout == NULL)  {
    fprintf (stderr, "%s: could not open output file %s\n", argv[0], argv[2]);
    fclose (strin);
    return (-1);
  }

  fgets (iline, 132, strin);
  sscanf (iline, "%x %x %x %x %x", &a, &b, &c, &d, &nwords);


  fprintf (strout, "%c\n$A0000,\n",2);

  for (i = 0; i < nwords; i++)  {

    fgets (iline, 132, strin);
    iline[1] = '0';
    sscanf (iline, "%x", &a);
    fprintf (strout, "%02X %02X %02X %02X", (a>>24) & 0x00ff, (a>>16) & 0xff, (a>>8) & 0xff,
                                            a & 0xff);

    if ( ((i+1) % 6) )
      fprintf (strout, " ");
    else
      fprintf (strout, "\n");

  }

  fprintf (strout, "\n%c",3);

  fclose (strin);
  fclose (strout);

  return (0);

}
  



  




