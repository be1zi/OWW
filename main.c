#include<stdlib.h>
#include<stdio.h>
#include<time.h>

typedef enum {
    ROW,
    COLUMN
}direction;

struct Compress {
    int nonZeros;
    int valPtr;
    int ptr;
    int currentEl;

    double* valArr;
    int* firstIdxArr;
    int* secondIdxArr;
};

struct Matrix {

    double** array;
    int height;
    int width;
};

struct Vector {

    double* array;
    int size;
};

int getRandomIntegerValue(int start, int end);
double getRandomDoubleValue(double start, double end);

void setValue(double **arr, int i, int j);

void printMatrix(struct Matrix* matrix);
void printIntegerArray(char* title, int *arr, int size);
void printDoubleArray(char* title, double *arr, int size);

void allocArray(struct Matrix* matrix);
void fillArray(struct Matrix* matrix, int k, direction dir);

void createCRC(struct Matrix* matrix, struct Compress* compress);
void createCCS(struct Matrix* matrix);

int nonZerosValues(struct Matrix* matrix);

void testCRS(struct Matrix* matrix, struct Compress* compress);
void testCCS(struct Matrix* matrix, struct Compress* compress);

double* generateVector(int size);
struct Vector* multiplyMatrixByVectorUsingCRS(struct Matrix* matrix, struct Compress* compress, double* vector);
struct Vector* multiplyMatrixByVector(struct Matrix* matrix, double* vector);
void compareMulResult(struct Vector* first, struct Vector* second);

int main() {

    srand(time(NULL));

    int k;
    struct Matrix m;
    struct Matrix* matrix;
    matrix = &m;
    struct Compress c;
    struct Compress* compress = &c;
    double* vector;

    matrix->width = 6;
    matrix->height = 6;
    k = 0.5 * matrix->width;

    //wiersz po wierszu
    printf("Row by row:\n");
    allocArray(matrix);
    fillArray(matrix, k, ROW);

    printMatrix(matrix);
    createCRC(matrix, compress);

    vector = generateVector(matrix->width);
    struct Vector* CRSMulResult = multiplyMatrixByVectorUsingCRS(matrix, compress, vector);
    struct Vector* naiveMulResult = multiplyMatrixByVector(matrix, vector);
    compareMulResult(CRSMulResult, naiveMulResult);

    //createCCS(matrix);

    //kolumna po kolumnie
//    printf("Column by column:\n");
//    allocArray(matrix);
//    fillArray(matrix, k, COLUMN);
//
//    printMatrix(matrix);
//    createCRC(matrix, compress);
//    multiplyMatrixByVector(matrix, compress, generateVector(matrix->width));

    //createCCS(matrix);

    getchar();
    return 0;
}

//MARK: Create array

void allocArray(struct Matrix* matrix) {
    matrix->array = (double **)malloc(matrix->height * sizeof(double *));

    for (int i = 0; i < matrix->height; i++) {
        matrix->array[i] = (double *)malloc(matrix->width * sizeof(double));
    }

    for (int i = 0; i < matrix->height; i++) {
        for (int j = 0; j < matrix->width; j++) {
            matrix->array[i][j] = 0.f;
        }
    }
}

void fillArray(struct Matrix* matrix, int k, direction dir) {

    int loop = 0;
    int secondDirection = 0;

    switch (dir) {
        case ROW: {
            loop = matrix->height;
            secondDirection = matrix->width;
            break;
        }
        case COLUMN: {
            loop = matrix->width;
            secondDirection = matrix->height;
            break;
        }
    }

    for (int i = 0; i < loop; i++) {
        int localK = k;

        // wypełnianie przekątnej
        if (i < secondDirection) {
            setValue(matrix->array, i, i);
            localK--;
        }

        //wypełnianie pozostałych wartości
        switch (dir) {
            case ROW: {
                while (localK > 0) {
                    int positionInRow = getRandomIntegerValue(0, matrix->width - 1);

                    if (positionInRow != i) {
                        localK--;
                        setValue(matrix->array, i, positionInRow);
                    }
                }
                break;
            }
            case COLUMN: {
                while (localK > 0) {
                    int positionInColumn = getRandomIntegerValue(0, matrix->height - 1);

                    if (positionInColumn != i) {
                        localK--;
                        setValue(matrix->array, positionInColumn, i);
                    }
                }
                break;
            }
        }
    }
}

//MARK: Setter

void setValue(double **arr, int i, int j) {

    if (arr[i][j] > 0) {
        return;
    }

    if (i == j) {
        arr[i][j] = getRandomDoubleValue(1.0, 2.0);
    }
    else {
        arr[i][j] = getRandomDoubleValue(0.0, 1.0);
    }
}

//MARK: Generators

int getRandomIntegerValue(int start, int end) {

    int result = 0;

    result = rand() % (int)(end - start + 1) + start;

    return result;
}

double getRandomDoubleValue(double start, double end) {

    double result = 0.0;

    result = ((double)rand() * (end - start)) / RAND_MAX + start;

    return result;
}

//MARK: Present data

void printMatrix(struct Matrix* matrix) {
    for (int i = 0; i < matrix->height; i++) {
        for (int j = 0; j < matrix->width; j++) {
            printf("%f ", matrix->array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printIntegerArray(char* title, int *arr, int size) {

    printf("%s: ",title);

    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void printDoubleArray(char* title, double *arr, int size) {

    printf("%s: ", title);

    for (int i = 0; i < size; i++) {
        printf("%f ", arr[i]);
    }
    printf("\n");
}

//MARK: Save array in compressed format

void createCRC(struct Matrix* matrix, struct Compress* compress) {

    compress->nonZeros = nonZerosValues(matrix);
    compress->valPtr = 0;
    compress->ptr = 0;
    compress->currentEl = -1;
    compress->valArr = (double *)malloc(compress->nonZeros * sizeof(double));
    compress->firstIdxArr = (int *)malloc(compress->nonZeros * sizeof(int));
    compress->secondIdxArr = (int *)malloc(sizeof(int));

    for (int i = 0; i < matrix->height; i++) {
        for (int j = 0; j < matrix->width; j++) {
            if (matrix->array[i][j] > 0) {
                compress->valArr[compress->valPtr] = matrix->array[i][j];
                compress->firstIdxArr[compress->valPtr] = j;

                if (compress->currentEl != i) {
                    compress->currentEl = i;
                    compress->secondIdxArr = realloc(compress->secondIdxArr, (compress->ptr + 1) * sizeof(int));
                    compress->secondIdxArr[compress->ptr++] = compress->valPtr;
                }

                compress->valPtr++;
            }
        }
    }

    //Add last index
    compress->secondIdxArr = realloc(compress->secondIdxArr, (compress->ptr + 1) * sizeof(int));
    compress->secondIdxArr[compress->ptr++] = compress->valPtr;

    printf("CRS: \n");
    printDoubleArray("val", compress->valArr, compress->valPtr);
    printIntegerArray("col_ind", compress->firstIdxArr, compress->valPtr);
    printIntegerArray("row_ptr", compress->secondIdxArr, compress->ptr);

    testCRS(matrix, compress);
    printf("\n");
}

void createCCS(struct Matrix* matrix) {

    struct Compress c;
    struct Compress* compress = &c;

    compress->nonZeros = nonZerosValues(matrix);
    compress->valPtr = 0;
    compress->ptr = 0;
    compress->currentEl = -1;
    compress->valArr = (double *)malloc(compress->nonZeros * sizeof(double));
    compress->firstIdxArr = (int *)malloc(compress->nonZeros * sizeof(int));
    compress->secondIdxArr = (int *)malloc(sizeof(int));

    for (int i = 0; i < matrix->width; i++) {
        for (int j = 0; j < matrix->height; j++) {
            if (matrix->array[j][i] > 0) {
                compress->valArr[compress->valPtr] = matrix->array[j][i];
                compress->firstIdxArr[compress->valPtr] = j;

                if (compress->currentEl != i) {
                    compress->currentEl = i;
                    compress->secondIdxArr = realloc(compress->secondIdxArr, (compress->ptr + 1) * sizeof(int));
                    compress->secondIdxArr[compress->ptr++] = compress->valPtr;
                }

                compress->valPtr++;
            }
        }
    }
    compress->secondIdxArr = realloc(compress->secondIdxArr, (compress->ptr + 1) * sizeof(int));
    compress->secondIdxArr[compress->ptr++] = compress->valPtr;

    printf("CCS: \n");
    printDoubleArray("val", compress->valArr, compress->valPtr);
    printIntegerArray("row_ind", compress->firstIdxArr, compress->valPtr);
    printIntegerArray("col_ptr", compress->secondIdxArr, compress->ptr);

    testCCS(matrix, compress);
    printf("\n");

}

//MARK: Helper

int nonZerosValues(struct Matrix* matrix) {

    int result = 0;

    for (int i = 0; i < matrix->height; i++) {
        for (int j = 0; j < matrix->width; j++) {
            if (matrix->array[i][j] > 0) {
                result++;
            }
        }
    }

    return result;
}

//MARK: Test

void testCRS(struct Matrix* matrix, struct Compress* compress) {

    int localError = 0;
    int row = 0;

    for (int i = 0; i < compress->valPtr; i++) {

        //test values array
        if (matrix->array[row][compress->firstIdxArr[i]] != compress->valArr[i]) {
            localError++;
            break;
        }

        if ((i + 1) == compress->secondIdxArr[row + 1] ) {
            row++;
        }

    }

    if (localError > 0) {
        printf("ERROR!!!!!!!\n");
    } else {
        printf("Correctly saved\n");
    }
}

void testCCS(struct Matrix* matrix, struct Compress* compress) {

    int localError = 0;
    int col = 0;

    for (int i = 0; i < compress->valPtr; i++) {

        //test values array
        if (matrix->array[compress->firstIdxArr[i]][col] != compress->valArr[i]) {
            localError++;
            break;
        }

        if ((i + 1) == compress->secondIdxArr[col + 1] ) {
            col++;
        }

    }

    if (localError > 0) {
        printf("ERROR!!!!!!!\n");
    } else {
        printf("Correctly saved\n");
    }
}

//MARK: Matrix vector product

double* generateVector(int size) {

    double* arr = (double *)malloc(size * sizeof(double));

    for (int i = 0; i < size; i++) {
        arr[i] = getRandomDoubleValue(0.0, 1.0);
    }

    printDoubleArray("Vector: ", arr, size);

    return arr;
}

struct Vector* multiplyMatrixByVectorUsingCRS(struct Matrix* matrix, struct Compress* compress, double* vector) {

    struct Vector v;
    struct Vector* result = &v;
    result->array = (double *)malloc(matrix->width * sizeof(double));
    result->size = matrix->width;

    for (int i = 0; i < matrix->width; i++) {
        result->array[i] = 0.f;
        for (int j = compress->secondIdxArr[i]; j <= compress->secondIdxArr[i + 1] - 1; j++) {
            result->array[i] += compress->valArr[j] * vector[compress->firstIdxArr[j]];
        }
    }

    printDoubleArray("A*x using CRS:",  result->array, matrix->width);

    return result;
}

struct Vector* multiplyMatrixByVector(struct Matrix* matrix, double* vector) {

    struct Vector v;
    struct Vector* result = &v;
    result->array = (double *)malloc(matrix->width * sizeof(double));
    result->size = matrix->width;

    for (int i = 0; i < matrix->height; i++) {
        result->array[i] = 0.f;
        for (int j = 0; j < matrix->width; j++) {
            if (matrix->array[i][j] > 0) {
                result->array[i] += matrix->array[i][j] * vector[j];
            }
        }
    }

    printDoubleArray("A*x using naive:", result->array, matrix->width);

    return result;
}

void compareMulResult(struct Vector* first, struct Vector* second) {

    if (first->size != second->size) {
        printf("Error, vector arent the same");
        return;
    }

    for (int i = 0; i <first->size; i++) {
        if (first->array[i] != second->array[i]) {
            printf("Error, vector arent the same");
            return;
        }
    }

    printf("Vectors are the same");
}

