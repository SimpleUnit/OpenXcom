#include <stdlib.h>
#include <stdio.h>
#include <cstring>

#define ARGVPROCESS                         \
if(currentArg < argc)                       \
{                                           \
    strcpy(fileNameBuf, argv[currentArg]);  \
    puts(fileNameBuf);                      \
}                                           \
else                                        \
{                                           \
    gets(fileNameBuf);                      \
}                                           \
currentArg++;


int main(int argc, char** argv)
{
    puts("Rozdzielacz");
    printf("Argc: %d\n", argc);
    puts("Dawaj mnie tu file name");
    char fileNameBuf[256];
    memset(fileNameBuf, 0, sizeof(fileNameBuf));
    int currentArg = 1;
    ARGVPROCESS
    FILE *f = fopen(fileNameBuf, "rb");
    if(f == NULL)
    {
        puts("file open error");
        return 0;
    }

    fseek(f, 0, SEEK_END);
    long int rozmjar = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* sbase = new char[rozmjar];
    fread(sbase, 1, rozmjar, f);
    fclose(f);

    unsigned char mapDepth = sbase[0]; //(Y)
    unsigned char mapWidth = sbase[1]; //(X)
    unsigned char mapHeight = sbase[2]; //(Z)
    const int ileMap = (mapDepth / 10) * (mapWidth / 10);

    printf("Mapa ma rozmiar %dx%dx%d\n", mapDepth, mapWidth, mapHeight);

    printf("\nA teraz daj nazwê outputu (bez numeru i bez .MAP):\n");
    memset(fileNameBuf, 0, sizeof(fileNameBuf));
    printf("%d <?> %d\n", currentArg, argc);
    ARGVPROCESS
    char* koncowka = fileNameBuf + strlen(fileNameBuf);
    for(int fileIdx = 0; fileIdx < ileMap; ++fileIdx)
    {
        sprintf(koncowka, "%02d.MAP", fileIdx);
        printf("\tTeraz pisanie do %s\n", fileNameBuf);
        FILE *out = fopen(fileNameBuf, "wb");
        const unsigned char header[3] = {10, 10, mapHeight};
        fwrite(header, 1, 3, out);

        for(int h = 0; h < mapHeight; ++h)
        {
            for(int q = 0; q < 10; ++q)
            {
                int offset = 3;
                int idxW = fileIdx % (mapWidth / 10);
                int idxH = fileIdx / (mapWidth / 10);

                offset += 40 * idxH * mapWidth; //depth
                offset += 40 * idxW; //width
                offset += 4 * mapWidth * mapDepth * h; //height
                offset += 4 * mapWidth * q;

                fwrite(sbase + offset, 1, 40, out);
            }
        }
        fclose(out);
    }
    delete [] sbase;
    puts("OK... chyba");
    return 0;
}
