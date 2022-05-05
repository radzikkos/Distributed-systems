#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 4


void showMatrixes(int** A, int** B, int** C) {
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

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}

void freeMatrixes(int**A, int**B, int**C, int** result ){ 
    free(A[0]);
    free(A);
    free(B[0]);
    free(B);
    free(C[0]);
    free(C);
    free(result[0]);
    free(result);
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

int main(int argc, char **argv) {
    int processCount, processId, slaveTaskCount, source, dest, rows, offset;
    int** A;
    int** B;
    int** C;
    int** result;

    A = alloc_2d_int(N,N);
    B = alloc_2d_int(N,N);
    C = alloc_2d_int(N,N);
    result = alloc_2d_int(N,N);
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            A[i][j] = 1;
            B[i][j] = i+j;
            C[i][j] = 0;
            result[i][j] = 0;
        }
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Status status;
    slaveTaskCount = processCount - 1;

    // Root (Master) process
 if (processId == 0) {

    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            A[i][j] = 1;
            B[i][j] = i+j;
            C[i][j] = 0;
        }
    }

    offset = 0;

    
    for (dest=1; dest <= slaveTaskCount; dest++)
    {   
        
      MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      MPI_Send(&(A[0][0]), N*N, MPI_INT,dest,1, MPI_COMM_WORLD);
      MPI_Send(&(B[0][0]), N*N, MPI_INT, dest, 1, MPI_COMM_WORLD);
      
      offset = offset + 1;
    }

    for (int i = 1; i <= slaveTaskCount; i++)
    {
      source = i;
      MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);

      MPI_Recv(&(C[0][0]), N*N, MPI_INT, source, 2, MPI_COMM_WORLD, &status);

    for (int i = 0; i<N; i++) {
      for (int j = 0; j<N; j++)
      result[i][j] += C[i][j];
    }
    }

// Print the result matrix
    printf("\nResult Matrix C = Matrix A * Matrix B:\n\n");
    for (int i = 0; i<N; i++) {
      for (int j = 0; j<N; j++)
        printf("%d\t", result[i][j]);
      printf ("\n");
    }
    printf ("\n");
  }

  // Slave Processes 
  if (processId > 0) {

    // Source process ID is defined
    source = 0;

    MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);

    MPI_Recv((&A[0][0]), N*N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);

    MPI_Recv(&(B[0][0]), N*N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);

    if(offset == 0){
        
        firstShiftMatrix(A,0);
        firstShiftMatrix(B,1);

    }
    else {
        firstShiftMatrix(A,0);
        firstShiftMatrix(B,1);
        for(int i = 0; i < offset; i++){
            shiftMatrix(A,0);
            shiftMatrix(B,1);
        }

    }

    for (int i = 0; i<N; i++) {
            for (int j = 0; j<N; j++)
                C[i][j] = A[i][j] * B[i][j];
        }
    MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    MPI_Send(&(C[0][0]), N*N, MPI_INT, 0, 2, MPI_COMM_WORLD);
  }

  MPI_Finalize();
    freeMatrixes(A,B,C,result);
}

