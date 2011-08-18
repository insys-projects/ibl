#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "device.h"

#define TRUE 1
#define FALSE 0
#define MAX_LINE_LENGTH 40
char *input_file = "input.txt";

/* Parameters defined in the input_file */
#define FILE_NAME      "file_name"
#define DEVICE_ID      "device"
#define OFFSET_ADDR    "offset"

char        file_name[MAX_LINE_LENGTH];
uint32_t    device_id;
uint32_t    offset = I2C_MAP_ADDR;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
typedef ibl_t (*ibl_config_fn)(void);

int main (void)
{
    ibl_t ibl_params;
    FILE    *fp;
    int    ret;
    ibl_config_fn cfg[] = {
	    [1] = &c6455_ibl_config,
	    [2] = &c6474_ibl_config,
	    [3] = &c6474l_ibl_config,
	    [4] = &c6457_ibl_config,
	    [5] = &c6472_ibl_config,
	    [6] = &c6678_ibl_config,
	    [7] = &c6670_ibl_config,
    };
    int ncfgs = ARRAY_SIZE(cfg);

    fp = fopen(input_file, "r");
    if (fp == NULL)
    {
        printf("Error in opening %s input file\n", input_file);
        return;
    }

    ret = parse_input_file(fp);
    fclose (fp);

    if (ret == FALSE)
    {
        printf("Error in parsing %s input file\n", input_file);
        return;
    }


    fp = fopen (file_name, "r+");
    if (fp == NULL)
    {
        printf ("Failed to open file %s\n", file_name);
        return;
    }

    printf ("Opened file %s\n", file_name);

    if (fseek(fp, offset, SEEK_SET)) {
        fclose(fp);
        return -1;
    }

    if (device_id > 0 && device_id < ncfgs)
	    ibl_params = (*cfg[device_id])();
    
    if (fwrite((void*)&ibl_params, sizeof(ibl_t), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    printf ("Generated updated binary %s\n", file_name);

    fclose(fp);
}

int32_t 
xtoi
(
    char            *xs, 
    uint32_t        *result
)
{
    uint32_t    szlen = strlen(xs);
    int32_t     i, xv, fact;
    
    if (szlen > 0)
    {
        /* Converting more than 32bit hexadecimal value? */
        if (szlen>8) return 2; /* exit */
        
        /* Begin conversion here */
        *result = 0;
        fact = 1;
        
        /* Run until no more character to convert */
        for(i=szlen-1; i>=0 ;i--)
        {
            if (isxdigit(*(xs+i)))
            {
                if (*(xs+i)>=97)
                {
                    xv = ( *(xs+i) - 97) + 10;
                }
                else if ( *(xs+i) >= 65)
                {
                    xv = (*(xs+i) - 65) + 10;
                }
                else
                {
                    xv = *(xs+i) - 48;
                }
                *result += (xv * fact);
                fact *= 16;
            }
            else
            {
                // Conversion was abnormally terminated
                // by non hexadecimal digit, hence
                // returning only the converted with
                // an error value 4 (illegal hex character)
                return 4;
            }
        }
        return 0;
    }
    
    // Nothing to convert
    return 1;
}

int parse_input_file(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    char tokens[] = " :=;\n\r";
    char *key, *data;

    memset(line, 0, MAX_LINE_LENGTH);

    fgets(line, MAX_LINE_LENGTH, fp);
    key  = (char *)strtok(line, tokens);
    data = (char *)strtok(NULL, tokens);

    if(strlen(data) == 0)
    {
       return FALSE;
    }

    if(strcmp(key, FILE_NAME) != 0)
    {
        return FALSE;
    }

    strcpy (file_name, data);

    fgets(line, MAX_LINE_LENGTH, fp);
    key  = (char *)strtok(line, tokens);
    data = (char *)strtok(NULL, tokens);

    if(strlen(data) == 0)
    {
       return FALSE;
    }

    if(strcmp(key, DEVICE_ID) != 0)
    {
        return FALSE;
    }

    device_id = (uint32_t)atoi(data);
    
    fgets(line, MAX_LINE_LENGTH, fp);
    key  = (char *)strtok(line, tokens);
    data = (char *)strtok(NULL, tokens);

    if(strlen(data) == 0)
    {
       return FALSE;
    }

    if(strcmp(key, OFFSET_ADDR) != 0)
    {
        return FALSE;
    }

    if ((data[0] == '0') && (data[1] == 'x' || data[1] == 'X'))
    {
        if (xtoi (&data[2], &offset) != 0)
        {
            return FALSE;
        }
    }
    else
    {
        offset = (uint32_t)atoi(data);
    }

    return TRUE;
}

