#include <stdio.h>
#include "device.h"

#define TRUE 1
#define FALSE 0
#define MAX_LINE_LENGTH 80
#define DEFAULT_ETHBOOT_IDX 0
#define EVM_C6678_ETHBOOT_IDX 2

/* Parameters defined in the input_file */
#define FILE_NAME      "file_name"
#define DEVICE_ID      "device"
#define OFFSET_ADDR    "offset"
#define DOBOOTP  "ethBoot-doBootp"
#define BOOTFORMAT  "ethBoot-bootFormat"
#define IPADDR  "ethBoot-ipAddr"
#define SERVERIP  "ethBoot-serverIp"
#define GATEWAYIP  "ethBoot-gatewayIp"
#define NETMASK  "ethBoot-netmask"
#define FILENAME  "ethBoot-fileName"


char *input_file = "./input.txt";
char        file_name[MAX_LINE_LENGTH];
uint32_t    device_id;
uint32_t    offset;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))
typedef ibl_t (*ibl_config_fn)(void);

int main (void)
{
    ibl_t ibl_params;
    FILE    *fp, *mfp;
    int    ret;
    ibl_config_fn cfg[] = {
	    [1] = &c6455_ibl_config,
	    [2] = &c6474_ibl_config,
	    [3] = &c6474l_ibl_config,
	    [4] = &c6457_ibl_config,
	    [5] = &c6472_ibl_config,
	    [6] = &c6678_ibl_config,
	    [7] = &c6670_ibl_config,
	    [8] = &c6657_ibl_config,
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


    fp = fopen (file_name, "r+b");
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
		
	mfp = fopen(input_file, "r");
	modifyIblConfig(mfp, &ibl_params);
	fclose(mfp);
    
    if (fwrite((void*)&ibl_params, sizeof(ibl_t), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }


    printf ("Generated updated binary %s\n", file_name);

    fclose(fp);
    return 0;
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

int modifyIblConfig(FILE *fp, ibl_t *ibl)
{
    char line[MAX_LINE_LENGTH];
    char tokens[] = " :=;\n\r";
    char *key, *data;
	unsigned char ethBootIdx=DEFAULT_ETHBOOT_IDX, i0, i1, i2, i3;
    if ((device_id == 6) || (device_id == 7) || (device_id == 8)) 
        ethBootIdx = EVM_C6678_ETHBOOT_IDX;
    memset(line, 0, MAX_LINE_LENGTH);

    while(fgets(line, MAX_LINE_LENGTH, fp) != '\0')
	{
		key  = (char *)strtok(line, tokens);
		data = (char *)strtok(NULL, tokens);
		if ( (key == NULL) || (data == NULL) ) {} 
		else if (strcmp(key, DOBOOTP) == 0)
		{
			if (strcmp(data, "TRUE") == 0)
				ibl->bootModes[ethBootIdx].u.ethBoot.doBootp = TRUE;
			else
				ibl->bootModes[ethBootIdx].u.ethBoot.doBootp = FALSE;
		}
		else if (strcmp(key, BOOTFORMAT) == 0)
		{
			if (strcmp(data, "ibl_BOOT_FORMAT_ELF") == 0)
				ibl->bootModes[ethBootIdx].u.ethBoot.bootFormat = ibl_BOOT_FORMAT_ELF;
			else
				ibl->bootModes[ethBootIdx].u.ethBoot.bootFormat = ibl_BOOT_FORMAT_BBLOB;
		}
		else if (strcmp(key, IPADDR) == 0)
		{
			i0 = atoi((unsigned char*)strtok(data, "."));
			i1 = atoi((unsigned char*)strtok(NULL, "."));
			i2 = atoi((unsigned char*)strtok(NULL, "."));
			i3 = atoi((unsigned char*)strtok(NULL, "."));
			SETIP(ibl->bootModes[ethBootIdx].u.ethBoot.ethInfo.ipAddr, i0, i1, i2, i3);
		}
		else if (strcmp(key, SERVERIP) == 0)
		{
			i0 = atoi((unsigned char*)strtok(data, "."));
			i1 = atoi((unsigned char*)strtok(NULL, "."));
			i2 = atoi((unsigned char*)strtok(NULL, "."));
			i3 = atoi((unsigned char*)strtok(NULL, "."));
			SETIP(ibl->bootModes[ethBootIdx].u.ethBoot.ethInfo.serverIp, i0, i1, i2, i3);
		}
		else if (strcmp(key, GATEWAYIP) == 0)
		{
			i0 = atoi((unsigned char*)strtok(data, "."));
			i1 = atoi((unsigned char*)strtok(NULL, "."));
			i2 = atoi((unsigned char*)strtok(NULL, "."));
			i3 = atoi((unsigned char*)strtok(NULL, "."));
			SETIP(ibl->bootModes[ethBootIdx].u.ethBoot.ethInfo.gatewayIp, i0, i1, i2, i3);
		}
		else if (strcmp(key, NETMASK) == 0)
		{
			i0 = atoi((unsigned char*)strtok(data, "."));
			i1 = atoi((unsigned char*)strtok(NULL, "."));
			i2 = atoi((unsigned char*)strtok(NULL, "."));
			i3 = atoi((unsigned char*)strtok(NULL, "."));
			SETIP(ibl->bootModes[ethBootIdx].u.ethBoot.ethInfo.netmask, i0, i1, i2, i3);
		}
		else if (strcmp(key, FILENAME) == 0)
		{
			strcpy(ibl->bootModes[ethBootIdx].u.ethBoot.ethInfo.fileName, data);
		}
	}
    return TRUE;
}
