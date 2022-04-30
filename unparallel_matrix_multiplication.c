#include <stdio.h>
#include <stdlib.h>

//I'd put N as number of processes to concurrent it
#define N 4



void showMatrixes(int A[N][N], int B[N][N], int C[N][N]) {
    printf("MATRIX A\n");
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    printf("MATRIX B\n");
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }

    printf("MATRIX C\n");
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
}
void initialize(int A[N][N], int B[N][N], int C[N][N]) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            A[i][j] = 1;
            B[i][j] = i+j;
            C[i][j] = 0;
        }
    }
}

void getPreviousValuesRow(int matrix[N], int previousValues[N]) {

    for(int i = 0; i < N; i++) {
        previousValues[i] = matrix[i];
    }
}

void getPreviousValuesColumn(int matrix[N][N], int previousValues[N], int column) {
    for(int i = 0; i < N; i++) {
        previousValues[i] = matrix[i][column];
    }
}

void shiftMatrix(int matrix[N][N], int direction) {
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

void firstShiftMatrix(int matrix[N][N], int direction) {
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

void concatenateMatrixes(int A[N][N], int B[N][N], int C[N][N]) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            C[i][j] += A[i][j] * B[i][j];
        }
    }
}
int main() {
    int A[N][N];
    int B[N][N];
    int C[N][N];

    initialize(A, B, C);
    showMatrixes(A, B, C);
    // ---------------FROM THIS-----------------
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
        concatenateMatrixes(A, B, C);
    }
    // --------------TO THIS--------------------
    showMatrixes(A, B, C);
}

