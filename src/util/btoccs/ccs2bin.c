/* Convert a ccs file to a raw binary file
 *
 *  usage: ccs2bin [-swap] ccsfile binfile
 */

#include <stdio.h>

unsigned int swap(unsigned int v)
{
    unsigned int w;

    w = (((v >> 24) & 0xff) <<  0)  |
        (((v >> 16) & 0xff) <<  8)  |
        (((v >>  8) & 0xff) << 16)  |
        (((v >>  0) & 0xff) << 24);

    return (w);

}
FILE *fin  = NULL;
FILE *fout = NULL;
int doswap = 0;

#define USAGE  "usage: %s [-swap] ccsfile binfile"

int parseit (int argc, char *argv[])
{
    int i;

    if ((argc != 3) && (argc != 4))  {
       fprintf (stderr, USAGE, argv[0]);
       return (-1);
    }

    for (i = 1; i < argc; i++)  {

        if (!strcmp (argv[i], "-swap"))
            doswap = 1;

        else if (fin == NULL)  {
            fin = fopen (argv[i], "r");
            if (fin == NULL)  {
		        fprintf (stderr, "%s: Could not open file %s\n", argv[0], argv[i]);
                return (-1);
            }

        }  else if (fout == NULL)  {
            fout = fopen (argv[i], "wb");
            if (fout == NULL)  {
                fprintf (stderr, "%s: Could not open file %s\n", argv[0], argv[i]);
                fclose (fin);
                return (-1);
            }

        } else  {

            fprintf (stderr, USAGE, argv[0]);
            fclose (fout);
            fclose (fin);
            return (-1);
        }
    }

    return (0);

}

        



int main (int argc, char *argv[])
{
	unsigned int n;
	unsigned int v;
	unsigned int i;

	int a, b, c, d;


	char iline[132];

    if (parseit (argc, argv))
        return (-1);

	fgets (iline, 131, fin);
	sscanf (iline, "%x %x %x %x %x", &a, &b, &c, &d, &n);


	for (i = 0; i < n; i++)  {
		fgets (iline, 131, fin);
		sscanf (&iline[2], "%x", &v);
        if (doswap)
            v = swap(v);
		fwrite (&v, sizeof(unsigned int), 1, fout);
	}

	fclose (fout);
	fclose (fin);

	return (0);

}






