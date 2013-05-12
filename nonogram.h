#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <pthread.h>
#define MAXVALUE(x, y) ((x > y) ? x : y)

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
void freeNonogram(nonogram_t nonogram);
static void *outputProcessOfSolving(void *vptr_args);