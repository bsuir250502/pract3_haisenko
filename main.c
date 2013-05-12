#include <stdio.h>
#include <stdlib.h>
#include "nonogram.h"

int main(int argc, char *argv[])
{
    nonogram_t nonogram;
    nonogram = fNonogramInit();
    solvingNonogram(nonogram);
    outputField(nonogram);
    freeNonogram(nonogram);
    return 0;
}
