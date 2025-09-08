#include <stdio.h>

// 16bit addr , 2^16Byte
#define mem_size 65536
#define regfile_size 32
#define display_mem_size 64

unsigned char mem[mem_size] = {0};
unsigned char flag = 0x00;
unsigned short int acc = 0x0000;
unsigned short int pc = 0x0000;
unsigned short int regfile[regfile_size] = {0};

int readop(const char *path, unsigned int offset)
{
    int byte_num;
    // -1:error , 0:sucess
    FILE *fp = fopen(path, "rb");

    if (fp == NULL)
    {
        return -1;
    }
    else
    {
        byte_num = fread(&mem[offset], 1, 0x8000, fp);
        fclose(fp);

        return byte_num;
    }
}

void printmem(unsigned int offset)
{
    int i = offset;

    printf("-----Memory Dump (offset:0x%04x)-----\n", offset);
    printf("0x%04x  ", i);

    for (; i < mem_size; i++)
    {
        if (i % display_mem_size == display_mem_size-1)
        {
            printf("%02x\n", mem[i]);
            if (i != mem_size - 1)
            {
                printf("0x%04x  ", i + 1);
            }
        }
        else
        {
            printf("%02x ", mem[i]);
        }
    }

    printf("-----Memory Dump (offset:0x%04x) End-----\n", offset);
}

void printreg()
{
    int i = 0;

    printf("-----Registr Dump-----\n");
    printf("acc:%04x\n", acc);
    printf("pc:%04x\n", pc);
    printf("flag:%02x\n", flag);
    printf("\n");
    printf("$m%02d  ", i);

    for (; i < regfile_size; i++)
    {
        if (i % 8 == 7)
        {
            printf("%04x\n", regfile[i]);
            if (i != regfile_size - 1)
            {
                printf("$m%02d  ", i + 1);
            }
        }
        else
        {
            printf("%04x ", regfile[i]);
        }
    }

    printf("-----Registr Dump End-----\n");
}

void splitbits(unsigned char data, unsigned char *bits)
{
    unsigned char splitedbit[8] = {0};

    for (int i = 0; i < 8; i++)
    {
        splitedbit[i] = data % 2;
        data = data / 2;
        bits[i] = splitedbit[i];
    }
}

void autoassign(char type, unsigned char *mX, unsigned char *mY, unsigned char *cond, unsigned char *interrupt, unsigned short int *immediate, unsigned char *data, int count)
{
    if (type == 'F')
    {
        switch (count)
        {
        case 1:
            *mX = data[4] * 16 + data[3] * 8 + data[2] * 4 + data[1] * 2 + data[0];
            break;
        default:
            printf("Out Of Range Value:count");
            break;
        }
    }
    else if (type == 'J')
    {
        switch (count)
        {
        case 1:
            *mY = data[4] * 16 + data[3] * 8 + data[2] * 4 + data[1] * 2 + data[0];
            break;
        case 2:
            *cond = data[7] * 4 + data[6] * 2 + data[5];
            *mX = data[4] * 16 + data[3] * 8 + data[2] * 4 + data[1] * 2 + data[0];
            break;
        default:
            printf("Out Of Range Value:count");
            break;
        }
    }
    else if (type == 'I')
    {
        switch (count)
        {
        case 1:
            *interrupt = data[4] * 8 + data[3] * 8 + data[2] * 4 + data[1] * 2 + data[0];
            break;
        case 2:
            *immediate = data[7] * 128 + data[6] * 64 + data[5] * 32 + data[4] * 16 + data[3] * 8 + data[2] * 4 + data[1] * 2 + data[0];
            break;
        case 3:
            *immediate = *immediate + data[7] * 32768 + data[6] * 16384 + data[5] * 8192 + data[4] * 4096 + data[3] * 2048 + data[2] * 1024 + data[1] * 512 + data[0] * 256;
            break;
        default:
            printf("Out Of Range Value:count");
            break;
        }
    }
}

void flagset(unsigned char opecode, unsigned char mX)
{
    unsigned char flag_bits[8] = {0};

    splitbits(flag, flag_bits);

    switch (opecode)
    {
    case 0x0:
        acc = ~(acc | regfile[mX]);

        if (acc == 0)
        {
            flag_bits[2] = 1;
        }
        else
        {
            flag_bits[2] = 0;
        }

        if (acc >= 0x8000)
        {
            flag_bits[1] = 1;
        }
        else
        {
            flag_bits[1] = 0;
        }

        break;
    case 0x1:
        if (acc + regfile[mX] >= 0x10000)
        {
            flag_bits[0] = 1;
        }
        else
        {
            flag_bits[0] = 0;
        }

        acc = acc + regfile[mX];

        if (acc == 0)
        {
            flag_bits[2] = 1;
        }
        else
        {
            flag_bits[2] = 0;
        }

        if (acc >= 0x8000)
        {
            flag_bits[1] = 1;
        }
        else
        {
            flag_bits[1] = 0;
        }

        break;
    case 0x7:
        acc = acc >> 1;

        if (acc == 0)
        {
            flag_bits[2] = 1;
        }
        else
        {
            flag_bits[2] = 0;
        }

        break;
    default:
        printf("Not Change Flags\n");
        break;
    }

    flag = flag_bits[2] * 4 + flag_bits[1] * 2 + flag_bits[0];
}

int judgebranch(unsigned char cond)
{
    unsigned char flag_bits[8] = {0};

    splitbits(flag, flag_bits);

    switch (cond)
    {
    case 0:
        return 0;
        break;
    case 1:
        if (flag_bits[0] == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 2:
        if (flag_bits[1] == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 3:
        if (flag_bits[1] == 1 || flag_bits[0] == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 4:
        if (flag_bits[2] == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 5:
        if (flag_bits[2] == 1 || flag_bits[0] == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 6:
        if (flag_bits[2] == 1 || flag_bits[1] == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
        break;
    case 7:
        return 1;
        break;
    default:
        return 0;
        break;
    }
}

void exeop()
{
    unsigned char bits[8] = {0};
    unsigned char opecode = 0;
    unsigned char mX = 0;
    unsigned char mY = 0;
    unsigned char cond = 0;
    unsigned char interrupt = 0;
    unsigned short int immeditate = 0;

    splitbits(mem[pc], bits);
    opecode = bits[7] * 4 + bits[6] * 2 + bits[5];
    // printf("Opecode:%03x\n",opecode);

    switch (opecode)
    {
    case 0x0:
        printf("NOR\n");
        autoassign('F', &mX, &mY, &cond, &interrupt, &immeditate, bits, 1);
        // printf("mX:%01x\n", mX);
        flagset(opecode, mX);
        pc = pc + 1;
        break;
    case 0x1:
        printf("ADD\n");
        autoassign('F', &mX, &mY, &cond, &interrupt, &immeditate, bits, 1);
        // printf("mX:%01x\n", mX);
        flagset(opecode, mX);
        pc = pc + 1;
        break;
    case 0x2:
        printf("LD\n");
        autoassign('F', &mX, &mY, &cond, &interrupt, &immeditate, bits, 1);
        // printf("mX:%01x\n", mX);
        acc = mem[regfile[mX]];
        pc = pc + 1;
        break;
    case 0x3:
        printf("MOVE\n");
        autoassign('F', &mX, &mY, &cond, &interrupt, &immeditate, bits, 1);
        // printf("mX:%01x\n", mX);
        regfile[mX] = acc;
        pc = pc + 1;
        break;
    case 0x4:
        printf("SD\n");
        autoassign('F', &mX, &mY, &cond, &interrupt, &immeditate, bits, 1);
        // printf("mX:%01x\n", mX);
        mem[regfile[mX]] = acc;
        pc = pc + 1;
        break;
    case 0x5:
        printf("SETI\n");
        autoassign('I', &mX, &mY, &cond, &interrupt, &immeditate, bits, 1);
        // printf("interrupt:%01x immeditate:%04x\n", interrupt, immeditate);
        splitbits(mem[pc + 1], bits);
        autoassign('I', &mX, &mY, &cond, &interrupt, &immeditate, bits, 2);
        // printf("interrupt:%01x immeditate:%04x\n", interrupt, immeditate);
        splitbits(mem[pc + 2], bits);
        autoassign('I', &mX, &mY, &cond, &interrupt, &immeditate, bits, 3);
        // printf("interrupt:%01x immeditate:%04x\n", interrupt, immeditate);
        acc = immeditate;
        pc = pc + 3;
        break;
    case 0x6:
        printf("JL\n");
        autoassign('J', &mX, &mY, &cond, &interrupt, &immeditate, bits, 1);
        // printf("mX:%01x mY:%01x cond:%01x\n", mX, mY, cond);
        splitbits(mem[pc + 1], bits);
        autoassign('J', &mX, &mY, &cond, &interrupt, &immeditate, bits, 2);
        // printf("mX:%01x mY:%01x cond:%01x\n", mX, mY, cond);
        regfile[mX] = pc;

        if (judgebranch(cond))
        {
            pc = regfile[mY];
        }
        else
        {
            pc = pc + 2;
        }

        break;
    case 0x7:
        printf("SRL\n");
        autoassign('F', &mX, &mY, &cond, &interrupt, &immeditate, bits, 1);
        // printf("mX:%01x\n", mX);
        flagset(opecode, mX);
        pc = pc + 1;
        break;
    default:
        printf("Undefined Instruction\n");
        break;
    }
}

int main(int argc, char const *argv[])
{
    int byte_num;

    regfile[29] = 0x0000;
    regfile[30] = 0x0001;
    regfile[31] = 0xffff;
    printf("hello world\n");
    printf("argc:%d\n", argc);

    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]:%s\n", i, argv[i]);
    }

    if (argc >= 2)
    {
        byte_num = readop(argv[1], 0x0000);
        if (byte_num == -1)
        {
            printf("File Open Error\n");
        }
        else
        {
            printf("File Open Succeed\n");
            printf("Read Byte Num:%d\n", byte_num);

            while (1)
            {
                exeop();
                // printmem(0x0000);
                printreg();
                printf("\033[2J");
                printf("\033[H");

                // // Wait Time For My Windows PC
                // for (int i = 0; i < 500000000; i++)
                // {
                //     ;
                // }

                // Wait Time For My Ubuntu PC
                for (int i = 0; i < 2000000000; i++)
                {
                    ;
                }
            }
        }
    }
    else
    {
        printf("No File Specified In The Arguments\n");
    }

    // printmem(0x0000);
    // printreg();
    // exeop();
    // printf("Byte Size:%ld\n", sizeof(short int));

    return 0;
}
