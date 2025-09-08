#include <stdio.h>

#define data_size 1024

int main(int argc, char const *argv[])
{
    FILE *fp;
    FILE *fp_w;
    char data[data_size] = {0};
    char file_name[data_size] = {0};
    unsigned char wdata = 0;

    if (argc == 1)
    {
        printf("Argument Error\n");
        return -1;
    }

    fp = fopen(argv[1], "r");
    snprintf(file_name, data_size, "%s_converted.bin", argv[1]);
    fp_w = fopen(file_name, "wb");

    if (fp == NULL || fp_w == NULL)
    {
        printf("File Open Error\n");
        return -1;
    }
    else
    {
        printf("File Open Success\n");
    }

    while (fgets(data, data_size, fp) != NULL)
    {
        wdata = ((data[0] - '0') * 128) + ((data[1] - '0') * 64) + ((data[2] - '0') * 32) + ((data[3] - '0') * 16) + ((data[4] - '0') * 8) + ((data[5] - '0') * 4) + ((data[6] - '0') * 2) + ((data[7] - '0') * 1);
        fwrite(&wdata, 1, 1, fp_w);
    }

    fclose(fp);
    fclose(fp_w);

    return 0;
}
