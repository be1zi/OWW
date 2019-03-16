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
void testCCS(struct Matrix* matrix, struct Compress compress);

double* generateVector(int size);
void multiplyMatrixByVector(struct Matrix* matrix, struct Compress* compress, double* vector);

int main() {

   srand(time(NULL));

   int k;
   struct Matrix m;
   struct Matrix* matrix;
   matrix = &m;
   struct Compress c;
   struct Compress* compress = &c;

   matrix->width = 6;
   matrix->height = 6;
   k = 0.7 * matrix->width;

   //wiersz po wierszu
   printf("Row by row:\n");
   allocArray(matrix);
   fillArray(matrix, k, ROW);


   printMatrix(matrix);
   createCRC(matrix, compress);
   //multiplyMatrixByVector(matrix, compress, generateVector(matrix->width));
   createCCS(matrix);

   //kolumna po kolumnie
   //printf("Column by column:\n");
   //allocArray(matrix);
   //fillArray(matrix, k, COLUMN);

   //printMatrix(matrix);

   //createCRC(matrix);
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

   for (int i = 0; i< matrix->height; i++) {
       for (int j = 0; j< matrix->width; j++) {
           matrix->array[i][j] = 0.f;
       }
   }

   matrix->array[0][0] = 10.0;
    matrix->array[0][4] = 2.0;

    matrix->array[1][0] = 3.0;
    matrix->array[1][1] = 9.0;
    matrix->array[1][5] = 3.0;

    matrix->array[2][1] = 7.0;
    matrix->array[2][2] = 8.0;
    matrix->array[2][3] = 7.0;

    matrix->array[3][0] = 3.0;
    matrix->array[3][2] = 8.0;
    matrix->array[3][3] = 7.0;
    matrix->array[3][4] = 5.0;

    matrix->array[4][1] = 8.0;
    matrix->array[4][3] = 9.0;
    matrix->array[4][4] = 9.0;
    matrix->array[4][5] = 13.0;

    matrix->array[5][1] = 4.0;
    matrix->array[5][4] = 2.0;
    matrix->array[5][5] = 1.0;

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

   printf("CRC: \n");
   printDoubleArray("val", compress->valArr, compress->valPtr);
   printIntegerArray("col_ind", compress->firstIdxArr, compress->valPtr);
   printIntegerArray("row_ptr", compress->secondIdxArr, compress->ptr);

   testCRS(matrix, compress);
   printf("\n");
}

void createCCS(struct Matrix* matrix) {

   struct Compress compress;
   compress.nonZeros = nonZerosValues(matrix);
   compress.valPtr = 0;
   compress.ptr = 0;
   compress.currentEl = -1;
   compress.valArr = (double *)malloc(compress.nonZeros * sizeof(double));
   compress.firstIdxArr = (int *)malloc(compress.nonZeros * sizeof(int));
   compress.secondIdxArr = (int *)malloc(sizeof(int));

   for (int i = 0; i < matrix->width; i++) {
   	for (int j = 0; j < matrix->height; j++) {
       	if (matrix->array[j][i] > 0) {
           	compress.valArr[compress.valPtr] = matrix->array[j][i];
           	compress.firstIdxArr[compress.valPtr] = j;

           	if (compress.currentEl != i) {
               	compress.currentEl = i;
               	compress.secondIdxArr = realloc(compress.secondIdxArr, (compress.ptr + 1) * sizeof(int));
               	compress.secondIdxArr[compress.ptr++] = compress.valPtr;
           	}

            compress.valPtr++;
       	}
   	}
   }
    compress.secondIdxArr = realloc(compress.secondIdxArr, (compress.ptr + 1) * sizeof(int));
    compress.secondIdxArr[compress.ptr++] = compress.valPtr;

   printf("CCS: \n");
   printDoubleArray("val", compress.valArr, compress.valPtr);
   printIntegerArray("row_ind", compress.firstIdxArr, compress.valPtr);
   printIntegerArray("col_ptr", compress.secondIdxArr, compress.ptr);

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

   int colPtr = 0;
   int rowPtr = 0;
   int valPtr = 0;
   int localError = 0;
   int used = 0;

   for (int i = 0; i < matrix->height; i++) {

   	if (used > 0) {
       	rowPtr++;
       	used = 0;
   	}

   	for (int j = 0; j < matrix->width; j++) {
       	if (matrix->array[i][j] > 0) {

           	if (compress->valArr[valPtr++] != matrix->array[i][j]) {
               	localError++;
               	break;
           	}

           	if (compress->firstIdxArr[colPtr++] != j) {
               	localError++;
               	break;
           	}

           	if (compress->secondIdxArr[rowPtr] != i) {
               	localError++;
               	break;
           	} else {
               	used = 1;
           	}
       	}
   	}
   }

   if (localError > 0) {
   	printf("ERROR!!!!!!!\n");
   } else {
   	printf("Correctly saved\n");
   }
}

void testCCS(struct Matrix* matrix, struct Compress compress) {

   int colPtr = 0;
   int rowPtr = 0;
   int valPtr = 0;
   int localError = 0;
   int used = 0;

   for (int i = 0; i < matrix->width; i++) {

   	if (used > 0) {
       	colPtr++;
       	used = 0;
   	}

   	for (int j = 0; j < matrix->height; j++) {
       	if (matrix->array[j][i] > 0) {

           	if (compress.valArr[valPtr++] != matrix->array[j][i]) {
               	localError++;
               	break;
           	}

           	if (compress.firstIdxArr[rowPtr++] != j) {
               	localError++;
               	break;
           	}

           	if (compress.secondIdxArr[colPtr] != i) {
               	localError++;
               	break;
           	} else {
               	used = 1;
           	}
       	}

       	if (localError > 0) {
           	break;
       	}
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

void multiplyMatrixByVector(struct Matrix* matrix, struct Compress* compress, double* vector) {

    double* y = (double *)malloc(matrix->width * sizeof(double));
    int j;

    printDoubleArray("Compress: ", compress->valArr, compress->valPtr);

    for (int i = 0; i < matrix->width; i++) {
   	 y[i] = 0.f;
   	 printf("I: %d, compress->secondIdxArr[i]: %d, compress->secondIdxArr[i + 1] - 1: %d\n", i, compress->secondIdxArr[i], compress->secondIdxArr[i + 1] - 1);

   	 for (int j = compress->secondIdxArr[i]; j <= compress->secondIdxArr[i + 1] - 1; j++) {
//   	     printf("%d", j);
   	     //   	     printf("Compress->valArr[j] = %f\n", compress->valArr[j]);

//   		 y[i] += compress->valArr[j] * vector[compress->firstIdxArr[j]];
   	 }
        printf("\n");
    }

    printDoubleArray("A*x", y, matrix->width);
}