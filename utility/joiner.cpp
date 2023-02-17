#include <stdlib.h>
#include <stdio.h>
#include <cstring>


#define ARGVPROCESS                         \
if(currentArg < argc)                       \
{                                           \
    sscanf(argv[currentArg], "%d", &tembb); \
    printf("\t%d\n", tembb);                \
}                                           \
else                                        \
{                                           \
    gets(inputBuf);                         \
    sscanf(inputBuf, "%d", &tembb);         \
}                                           \
++currentArg;


int main(int argc, char** argv)
{
    puts("Po³¹czacz");
    int mapNum = 0;
    int tembb;
    char outW = 0;
    char outD = 0;
    char outH = 0;

    printf("Argc: %d\n", argc);

    int currentArg = 1;

    char inputBuf[256];
    memset(inputBuf, 0, sizeof(inputBuf));
    puts("Najpierw daj mnie tutaj numer map do scalenia");
    ARGVPROCESS
    mapNum = tembb;

    memset(inputBuf, 0, sizeof(inputBuf));
    puts("A teraz dawaj width");
    ARGVPROCESS
    outW = tembb;

    memset(inputBuf, 0, sizeof(inputBuf));
    puts("A teraz dawaj depth");
    ARGVPROCESS
    outD = tembb;

    printf("%d -> %dx%d\n", mapNum, outW, outD);

    char outputBuf[256];
    memset(outputBuf, 0, sizeof(outputBuf));
    puts("A teraz dawaj plikkq output");

    if(currentArg < argc)
    {
        strcpy(outputBuf, argv[currentArg]);
        puts(outputBuf);
    }
    else
    {
        gets(outputBuf);
    }
    currentArg++;

    if(mapNum*100 != outW*outD || mapNum == 0)
    {
        puts("Tyle map nie zrobi takiej mapy");
        return 1;
    }

    char** sbase = new char* [mapNum];

    long int rozmjar = 0;
    {
        memset(inputBuf, 0, sizeof(inputBuf));
        puts("A teraz dawaj pierszy input plik");
        if(currentArg < argc)
        {
            strcpy(inputBuf, argv[currentArg]);
            puts(inputBuf);
        }
        else
        {
            gets(inputBuf);
        }
        currentArg++;


        FILE *f = fopen(inputBuf, "rb");
        if(f == NULL)
        {
            delete[] sbase;
            puts("Plik sie zejjjsra³");
            return 2;
        }
        fseek(f, 0, SEEK_END);
        rozmjar = ftell(f);
        fseek(f, 0, SEEK_SET);

        for(int w=0; w<mapNum; w++)
            sbase[w] = new char[rozmjar];

        fread(sbase[0], 1, rozmjar, f);
        fclose(f);

        outH = sbase[0][2];

        if(sbase[0][0] != 10 || sbase[0][1] != 10)
        {
            puts("To nie jest 1x1");
            delete[] sbase;
            return 3;
        }
    }

    for(int q=1; q<mapNum; q++)
    {
        memset(inputBuf, 0, sizeof(inputBuf));
        printf("A teraz dawaj input plik #%d\n", q+1);
        if(currentArg < argc)
        {
            strcpy(inputBuf, argv[currentArg]);
            puts(inputBuf);
        }
        else
        {
            gets(inputBuf);
        }
        currentArg++;

        FILE *f = fopen(inputBuf, "rb");
        fseek(f, 0, SEEK_END);
        int temprozmjar = ftell(f);
        fseek(f, 0, SEEK_SET);
        fread(sbase[q], 1, rozmjar, f);
        fclose(f);

        if (temprozmjar != rozmjar)
        {
            puts("Pliki s¹ ró¿ne");
            for(int q=0; q<mapNum; q++)
                delete[] sbase[q];
            delete[] sbase;
            return 4;
        }
    }

    FILE *f = fopen(outputBuf, "wb");
    if(f == NULL)
    {
        printf("Plikq wyjœciowy sie zejjsra³");
        for(int q=0; q<mapNum; q++)
            delete[] sbase[q];
        delete[] sbase;
        return 5;
    }

    fwrite(&outD, 1, 1, f);
    fwrite(&outW, 1, 1, f);
    fwrite(&outH, 1, 1, f);
    for(int h = 0; h<outH; ++h)
    {
        for(int d=0; d<(outD/10); ++d)
        {
            for(int e=0; e<10; ++e)
            {
                for(int w=0; w<outW/10; ++w)
                {
                    fwrite(sbase[d*outW/10 + w] + 3 + 400*h + 40*e, 1, 40, f);
                }
            }
        }

    }
    fclose(f);

    for(int q=0; q<mapNum; q++)
        delete[] sbase[q];
    delete[] sbase;

    puts("OK... chyba");
    return 0;
}
