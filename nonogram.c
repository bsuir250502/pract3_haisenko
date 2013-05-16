#include "nonogram.h"

const int stringSize = 30;  //why not in .h?

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

    nonogram.sizeOfLine = MAXVALUE(nonogram.sizeX, nonogram.sizeY) / 2 + 1;
    nonogram.valuesX = (int **) calloc(nonogram.sizeY, sizeof(int *));
    for (i = 0; i < nonogram.sizeY; i++) {
        nonogram.valuesX[i] = (int *) calloc(nonogram.sizeOfLine, sizeof(int));
    }
    nonogram.valuesY = (int **) calloc(nonogram.sizeX, sizeof(int *));
    for (i = 0; i < nonogram.sizeX; i++) {
        nonogram.valuesY[i] = (int *) calloc(nonogram.sizeOfLine, sizeof(int));
    }
    nonogram.field = (struct cells_t **) calloc(nonogram.sizeY, sizeof(struct cells_t *));
    for (i = 0; i < nonogram.sizeY; i++) {
        nonogram.field[i] = (struct cells_t *) calloc(nonogram.sizeX, sizeof(struct cells_t));
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
    fclose(inputFile);
    return nonogram;
}

nonogram_t solvingNonogram(nonogram_t nonogram)
{
    //pthread_t outputThread;
    //pthread_create(&outputThread, NULL, outputProcessOfSolving, (void *) &nonogram);

    //previewValues(nonogram);
    if (valueSet(nonogram, 0, 0)) {
        printf("No solution.\n");
    }
    //outputField(nonogram);
    //pthread_cancel(outputThread);
    return nonogram;
}

static void *outputProcessOfSolving(void *vptr_args)
{
    nonogram_t *nonogram;
    nonogram = vptr_args;
    while (1) {
        outputField(*nonogram);
        usleep(40000);
    }
    return NULL;
}

int previewValues(nonogram_t nonogram) 
{
    int i, j, k, isAnyChanges = 0;

    /* checking digits more than half size */
    for (i = 0; i < nonogram.sizeX; i++) {
        for (j = 0; nonogram.valuesY[i][j] && j < nonogram.sizeOfLine; j++) {
            //outputField(nonogram);
            for (k = nonogram.valuesY[i][j] - 1; k > nonogram.sizeY - nonogram.valuesY[i][j]; k--) {
                nonogram.field[k][i].cell = 1;
                nonogram.field[k][i].isDefinitely = 1;
                isAnyChanges = 1;
            }
        }
    }

    for (i = 0; i < nonogram.sizeY; i++) {
        for (j = 0; nonogram.valuesX[i][j] && j < nonogram.sizeOfLine; j++) {
            //outputField(nonogram);
            for (k = nonogram.sizeX - nonogram.valuesX[i][j]; k < (nonogram.valuesX[i][j] - 1); k++) {
                nonogram.field[i][k].cell = 1;
                nonogram.field[i][k].isDefinitely = 1;
                isAnyChanges = 1;
            }
        }
    }

    if(isAnyChanges) {
        return 1;
    } else {
        return 0;
    }
    return 1;
}

int valueSet(nonogram_t nonogram, int x, int y)
{
    int nextX, nextY, isLastCell;
    ((x + 1) == nonogram.sizeX) ? (nextX = 0, nextY = y + 1) : (nextX = x + 1, nextY = y);  //need checking X or Y?
    (nextY == nonogram.sizeY) ? (isLastCell = 1) : (isLastCell = 0);

    if (nonogram.field[y][x].isDefinitely) {
        if (isLastCell) {
            return 0;
        } else {
            valueSet(nonogram, nextX, nextY);
        }
    }

    nonogram.field[y][x].cell = 1;
    if (fieldValidation(nonogram, x, y)) {
        nonogram.field[y][x].cell = 0;
        if (fieldValidation(nonogram, x, y)) {
            return 1;
        }
    }

    if (!isLastCell && valueSet(nonogram, nextX, nextY)) {
        if (!nonogram.field[y][x].cell) {
            return 1;
        }
        nonogram.field[y][x].cell = 0;
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
    int i, j, sum, usedValues, lastY = 0, lastX = 0;

    /* checking digits more than half */
    for (i = 0; nonogram.valuesY[x][i] && i < nonogram.sizeOfLine; i++) {
        if ((y > nonogram.sizeY - nonogram.valuesY[x][i]) && (y < nonogram.valuesY[x][i]) && !nonogram.field[y][x].cell) {
            return 1;
        }
    }
    for (i = 0; nonogram.valuesX[y][i] && i < nonogram.sizeOfLine; i++) {
        if ((x > nonogram.sizeX - nonogram.valuesX[y][i]) && (x < nonogram.valuesX[y][i]) && !nonogram.field[y][x].cell) {
            return 1;
        }
    }

    /* main validation loop for y */
    for (i = 0, j = 0, sum = 0, usedValues = 0; i <= nonogram.sizeY; i++) {
        if ((i < nonogram.sizeY) && nonogram.field[i][x].cell) {  //don't change
            sum++;
        } else {
            if (y > lastY && usedValues != j) {
                return 1;
            }
            if (!sum) {
                continue;
            }
            if (sum <= nonogram.valuesY[x][j] && j < nonogram.sizeOfLine) {
                lastY = i;
                if (usedValues != j) {
                    return 1;
                }
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

    /* whether all the numbers posted. Checking at the end of line */
    if (y == (nonogram.sizeY - 1)) {
        for (i = 0; i < nonogram.sizeOfLine && nonogram.valuesY[x][i]; i++);
        if (usedValues != i) {
            return 1;
        }
    }

    /* main validation loop for i */
    for (i = 0, j = 0, sum = 0, usedValues = 0; i <= nonogram.sizeX; i++) {
        if ((i < nonogram.sizeX) && nonogram.field[y][i].cell) {  //don't change
            sum++;
        } else {
            if (x > lastX && usedValues != j) {
                return 1;
            }
            if (!sum) {
                continue;
            }
            if (sum <= nonogram.valuesX[y][j] && j < nonogram.sizeOfLine) {
                lastX = i;
                if (usedValues != j) {
                    return 1;
                }
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

    /* whether all the numbers posted. Checking at the end of line */
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
            if (nonogram.field[i][j].cell) {
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
}

void freeNonogram(nonogram_t nonogram)
{
    int i;
    for (i = 0; i < nonogram.sizeY; i++) {
        free(nonogram.valuesX[i]);
        free(nonogram.field[i]);
    }
    for (i = 0; i < nonogram.sizeX; i++) {
        free(nonogram.valuesY[i]);
    }
    free(nonogram.valuesX);
    free(nonogram.valuesY);
    free(nonogram.field);
}
