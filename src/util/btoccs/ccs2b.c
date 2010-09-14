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
  



  




