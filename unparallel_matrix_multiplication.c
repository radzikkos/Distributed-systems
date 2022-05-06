#include <stdio.h>
#include <stdlib.h>

//I'd put N as number of processes to concurrent it
#define N 4



void showMatrixes(int** A, int** B, int** C) {
    // printf("MATRIX A\n");
    // for(int i = 0; i < N; i++) {
    //     for(int j = 0; j < N; j++) {
    //         printf("%d ", A[i][j]);
    //     }
    //     printf("\n");
    // }

    // printf("MATRIX B\n");
    // for(int i = 0; i < N; i++) {
    //     for(int j = 0; j < N; j++) {
    //         printf("%d ", B[i][j]);
    //     }
    //     printf("\n");
    // }

    printf("MATRIX C\n");
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
}

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}

void freeMatrixes(int**A, int**B, int**C ){ 
    free(A[0]);
    free(A);
    free(B[0]);
    free(B);
    free(C[0]);
    free(C);
}

void initialize(int** A, int** B, int** C) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            A[i][j] = 1;
            B[i][j] = i+j;
            C[i][j] = 0;
        }
    }
}



void getPreviousValuesRow(int* matrix, int* previousValues) {

    for(int i = 0; i < N; i++) {
        previousValues[i] = matrix[i];
    }
}

void getPreviousValuesColumn(int** matrix, int* previousValues, int column) {
    for(int i = 0; i < N; i++) {
        previousValues[i] = matrix[i][column];
    }
}

void shiftMatrix(int** matrix, int direction) {
    //left
    int previousValues[N] = {0};
    if(direction == 0) {
        for(int row = 1; row < N; row++) {
            getPreviousValuesRow(matrix[row], previousValues);
            for(int i = 0; i < N; i++) {
                matrix[row][i] = previousValues[(i+1) % N];
            }
        }
    }
    //up
    else {
        for(int i = 0; i < N; i++){
            int firstElement = matrix[0][i];
            for(int j = 0; j < N; j++){
                if(j == N - 1){
                    matrix[j][i] = firstElement;
                }
                else {
                    matrix[j][i] = matrix[j + 1][i];
                }
            }
        }
    }
}

void firstShiftMatrix(int** matrix, int direction) {
    // left
    int previousValues[N] = {0};
    if(direction == 0) {
        for(int row = 1; row < N; row++) {
            getPreviousValuesRow(matrix[row], previousValues);
            for(int i = 0; i < N; i++) {
                matrix[row][i] = previousValues[(i+row) % N];
            }
        }
    }
    //up
    else {
        for(int column = 1; column < N; column++) {
            getPreviousValuesColumn(matrix, previousValues, column);
            for(int i = 0; i < N; i++) {
                matrix[i][column] = previousValues[(i+column) % N];
            }
        }
    }

}

void concatenateMatrixes(int** A, int** B, int** C) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            C[i][j] += A[i][j] * B[i][j];
        }
    }
}
int main() {
    int** A;
    int** B;
    int** C;
    A = alloc_2d_int(N,N);
    B = alloc_2d_int(N,N);
    C = alloc_2d_int(N,N);
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            A[i][j] = 1;
            B[i][j] = i+j;
            C[i][j] = 0;
        }
    }
    // initialize(A, B, C);
    // showMatrixes(A, B, C);
    // // ---------------FROM THIS-----------------
    for(int i = 0; i < N; i++) {
        // I see posibilities to concurrent this piece of code
        // On this time we can pass matrixes A,B and appropriate offset, 
        // which can say, how many times we have to shift matrix.
        if (i == 0) {
            firstShiftMatrix(A,0);
            firstShiftMatrix(B,1);
        }else{
            shiftMatrix(A,0);
            shiftMatrix(B,1);
        }
    //     printf("CREATED C MATRIX\n");
    // for (int i = 0; i<N; i++) {
    //   for (int j = 0; j<N; j++)
    //     printf("%d\t", A[i][j] * B[i][j]);
    //   printf ("\n");
    // }
    // printf("\n");
    printf("Matrix A in %d\n", i);
    for (int i = 0; i<N; i++) {
      for (int j = 0; j<N; j++)
        printf("%d\t", A[i][j]);
      printf ("\n");
    }

    printf("Matrix B in %d\n", i);
    for (int i = 0; i<N; i++) {
      for (int j = 0; j<N; j++)
        printf("%d\t", B[i][j]);
      printf ("\n");
    }
    printf("\n");
        concatenateMatrixes(A, B, C);
    }
    // --------------TO THIS--------------------
    showMatrixes(A, B, C);

    freeMatrixes(A,B,C);
}

