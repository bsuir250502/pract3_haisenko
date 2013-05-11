#include <stdio.h>
#include <stdlib.h>
#define MAX(x, y) ((x > y) ? x : y)

const int stringSize = 30;
long long int count = 0;

typedef struct nonogram_t {
    int sizeX;
    int sizeY;
    int sizeOfLine;
    int **valuesX;
    int **valuesY;
    int **field;
} nonogram_t;

nonogram_t fNonogramInit();
nonogram_t solvingNonogram(nonogram_t nonogram);
int valueSet(nonogram_t nonogram, int x, int y);
int fieldValidation(nonogram_t nonogram, int x, int y);
void outputField(nonogram_t nonogram);

int main(int argc, char *argv[])
{
    nonogram_t nonogram;
    nonogram = fNonogramInit();
    outputField(nonogram);
    nonogram = solvingNonogram(nonogram);
    outputField(nonogram);
    printf("%lld\n", count);
    return 0;
}

nonogram_t fNonogramInit()
{
    int i, j;
    char tempString[stringSize], *tempPointer;
    nonogram_t nonogram;
    FILE *inputFile;
    inputFile = fopen("nonogram.in", "r");
    if (!inputFile) {
        printf("No input file.\n");
        exit(1);
    }

    fgets(tempString, stringSize, inputFile);
    fgets(tempString, stringSize, inputFile);
    nonogram.sizeX = strtol(tempString, &tempPointer, 10);
    nonogram.sizeY = strtol(tempPointer, &tempPointer, 10);
    fgets(tempString, stringSize, inputFile);
    if (nonogram.sizeX < 2 || nonogram.sizeY < 2) {
        printf("Wrong field size.\n");
        exit(1);
    }

    nonogram.sizeOfLine = MAX(nonogram.sizeX, nonogram.sizeY) / 2 + 1;
    nonogram.valuesX = (int **) calloc(nonogram.sizeY, sizeof(int *));
    for (i = 0; i < nonogram.sizeY; i++) {
        nonogram.valuesX[i] = (int *) calloc(nonogram.sizeOfLine, sizeof(int));
    }
    nonogram.valuesY = (int **) calloc(nonogram.sizeX, sizeof(int *));
    for (i = 0; i < nonogram.sizeX; i++) {
        nonogram.valuesY[i] = (int *) calloc(nonogram.sizeOfLine, sizeof(int));
    }
    nonogram.field = (int **) calloc(nonogram.sizeY, sizeof(int *));
    for (i = 0; i < nonogram.sizeY; i++) {
        nonogram.field[i] = (int *) calloc(nonogram.sizeX, sizeof(int));
    }

    for (i = 0; i < nonogram.sizeX; i++) {
        fgets(tempString, stringSize, inputFile);
        tempPointer = tempString;
        for (j = 0; *tempPointer != '\n'; j++) {
            nonogram.valuesY[i][j] = strtol(tempPointer, &tempPointer, 10);
        }
    }
    for (i = nonogram.sizeY - 1; i >= 0; i--) {
        fgets(tempString, stringSize, inputFile);
        tempPointer = tempString;
        for (j = 0; *tempPointer != '\n'; j++) {
            nonogram.valuesX[i][j] = strtol(tempPointer, &tempPointer, 10);
        }
    }
    return nonogram;
}

nonogram_t solvingNonogram(nonogram_t nonogram)
{
    if (valueSet(nonogram, 0, 0)) {
        printf("No solution.\n");
        outputField(nonogram);
        exit(1);
    }
    return nonogram;
}

int valueSet(nonogram_t nonogram, int x, int y) //ставить строками
{
    count++;
    if (count % 500000 == 0) {
        outputField(nonogram);
        printf("%lld\n", count);
    }

    int nextX, nextY, isLastCell;
    if (nonogram.sizeX < nonogram.sizeY) {  //checking
        ((x + 1) == nonogram.sizeX) ? (nextX = 0, nextY = y + 1) : (nextX = x + 1, nextY = y);
        (nextY == nonogram.sizeY) ? (isLastCell = 1) : (isLastCell = 0);
    } else {
        ((y + 1) == nonogram.sizeY) ? (nextY = 0, nextX = x + 1) : (nextY = y + 1, nextX = x);
        (nextX == nonogram.sizeX) ? (isLastCell = 1) : (isLastCell = 0);
    }

    nonogram.field[y][x] = 1;
    if (fieldValidation(nonogram, x, y)) {
        nonogram.field[y][x] = 0;
        if (fieldValidation(nonogram, x, y)) {
            return 1;
        }
    }

    if (!isLastCell && valueSet(nonogram, nextX, nextY)) {
        if (!nonogram.field[y][x]) {
            return 1;
        }
        nonogram.field[y][x] = 0;
        if (valueSet(nonogram, nextX, nextY)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
    return 1;
}

int fieldValidation(nonogram_t nonogram, int x, int y)
{
    int i, j, sum, usedValues;
/*
    if (nonogram.valuesY[x][0] == nonogram.sizeY && !nonogram.field[y][x]) {
        return 1;
    }
    if (nonogram.valuesX[y][0] == nonogram.sizeX && !nonogram.field[y][x]) {
        return 1;
    }*/
    for (i = 0; nonogram.valuesY[x][i] && i < nonogram.sizeOfLine; i++) {
        if ((y > nonogram.sizeY - nonogram.valuesY[x][i]) && (y < nonogram.valuesY[x][i]) && !nonogram.field[y][x]) {
            return 1;
        }
    }
    for (i = 0; nonogram.valuesX[y][i] && i < nonogram.sizeOfLine; i++) {
        if ((x > nonogram.sizeX - nonogram.valuesX[y][i]) && (x < nonogram.valuesX[y][i]) && !nonogram.field[y][x]) {
            return 1;
        }
    }

    for (i = 0, j = 0, sum = 0, usedValues = 0; i <= nonogram.sizeY; i++) {
        if ((i < nonogram.sizeY) && nonogram.field[i][x]) {
            sum++;
        } else {
            if (!sum) {
                continue;
            }
            if (sum <= nonogram.valuesY[x][j] && j < nonogram.sizeOfLine && nonogram.valuesY[x][j]) {
                if (sum == nonogram.valuesY[x][j]) {
                    usedValues++;
                }
                sum = 0;
            } else {
                return 1;
            }
            j++;
        }
    }
    if (y == (nonogram.sizeY - 1)) {
        for (i = 0; i < nonogram.sizeOfLine && nonogram.valuesY[x][i]; i++);
        if (usedValues != i) {
            return 1;
        }
    }

    for (i = 0, j = 0, sum = 0, usedValues = 0; i <= nonogram.sizeX; i++) {
        if ((i < nonogram.sizeX) && nonogram.field[y][i]) {
            sum++;
        } else {
            if (!sum) {
                continue;
            }
            if (sum <= nonogram.valuesX[y][j] && j < nonogram.sizeOfLine && nonogram.valuesX[y][j]) {
                if (sum == nonogram.valuesX[y][j]) {
                    usedValues++;
                }
                sum = 0;
            } else {
                return 1;
            }
            j++;
        }
    }
    if (x == (nonogram.sizeX - 1)) {
        for (i = 0; i < nonogram.sizeOfLine && nonogram.valuesX[y][i]; i++);
        if (usedValues != i) {
            return 1;
        }
    }
    
    return 0;
}

void outputField(nonogram_t nonogram)
{
    system("clear");
    int i, j, k;
    for (i = 0; i < nonogram.sizeOfLine; i++) {
        for (k = 0; k < nonogram.sizeOfLine; k++) {
            printf("  ");
        }
        printf(" ");
        for (j = 0; j < nonogram.sizeX; j++) {
            if (nonogram.valuesY[j][nonogram.sizeOfLine - i - 1]) {
                printf("%2d", nonogram.valuesY[j][nonogram.sizeOfLine - i - 1]);
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
    for (k = 0; k < nonogram.sizeOfLine; k++) {
        printf("  ");
    }
    printf(" ");
    for (k = 0; k < nonogram.sizeX; k++) {
        printf("--");
    }
    printf("\n");
    for (i = nonogram.sizeY - 1; i >= 0; i--) {
        for (j = nonogram.sizeOfLine - 1; j >= 0; j--) {
            if (nonogram.valuesX[i][j]) {
                printf("%2d", nonogram.valuesX[i][j]);
            } else {
                printf("  ");
            }
        }
        printf("|");
        for (j = 0; j < nonogram.sizeX; j++) {
            if (nonogram.field[i][j]) {
                printf("■ ");
            } else {
                printf("  ");
            }
        }
        printf("|\n");
    }
    for (k = 0; k < nonogram.sizeOfLine; k++) {
        printf("  ");
    }
    printf(" ");
    for (k = 0; k < nonogram.sizeX; k++) {
        printf("--");
    }
    printf("\n");
    //usleep(10000);
}