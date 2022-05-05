#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
//I'd put N as number of processes to concurrent it
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

// Determine number of rows of the Matrix A, that is sent to each slave process
    // rows = N/slaveTaskCount;
// Offset variable determines the starting point of the row which sent to slave process
    offset = 0;

// Calculation details are assigned to slave tasks. Process 1 onwards;
// Each message's tag is 1
    // printf("\n SENDING Matrix A \n\n");
    // A[0][0] = 99;
    
    for (dest=1; dest <= slaveTaskCount; dest++)
    {   
        
      // Acknowledging the offset of the Matrix A
      MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      // Acknowledging the number of rows
    //   MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
      // Send rows of the Matrix A which will be assigned to slave process to compute
      MPI_Send(&(A[0][0]), N*N, MPI_INT,dest,1, MPI_COMM_WORLD);
      // Matrix B is sent
      MPI_Send(&(B[0][0]), N*N, MPI_INT, dest, 1, MPI_COMM_WORLD);
      
      // Offset is modified according to number of rows sent to each process
      offset = offset + 1;
    }

// Root process waits untill the each slave proces sent their calculated result with message tag 2
    for (int i = 1; i <= slaveTaskCount; i++)
    {
      source = i;
      // Receive the offset of particular slave process
      MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      // Receive the number of rows that each slave process processed
    //   MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      // Calculated rows of the each process will be stored int Matrix C according to their offset and
      // the processed number of rows
      MPI_Recv(&(C[0][0]), N*N, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
    //   for (int i = 0; i<N; i++) {
    //         for (int j = 0; j<N; j++)
    //             printf("%d\t", C[i][j]);
    //         printf ("\n");
    //     }
    //     printf("\n\n");
    for (int i = 0; i<N; i++) {
      for (int j = 0; j<N; j++)
      result[i][j] += C[i][j];
    //     printf("%d\t", C[i][j]);
    //   printf ("\n");
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

    // Slave process waits for the message buffers with tag 1, that Root process sent
    // Each process will receive and execute this separately on their processes

    // The slave process receives the offset value sent by root process
    MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    // printf("%d\n", offset);
    // The slave process receives number of rows sent by root process  
    // MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    // The slave process receives the sub portion of the Matrix A which assigned by Root 
    MPI_Recv((&A[0][0]), N*N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    // for (int i = 0; i<N; i++) {
    //   for (int j = 0; j<N; j++)
    //     printf("%d\t", A[i][j]);
    //   printf ("\n");
    // }
    // printf("\n Matrix A \n\n");
    // for (int i = 0; i<N; i++) {
    //   for (int j = 0; j<N; j++)
    //     printf("%.0f\t", A[i][j]);
    //   printf ("\n");
    // }
    // printf ("\n");
    // The slave process receives the Matrix B
    MPI_Recv(&(B[0][0]), N*N, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    // for (int i = 0; i<N; i++) {
    //   for (int j = 0; j<N; j++)
    //   printf ("\n");
    // }
    // Matrix multiplication
    if(offset == 0){
        
        firstShiftMatrix(A,0);
        firstShiftMatrix(B,1);
    //     for (int i = 0; i<N; i++) {
    //   for (int j = 0; j<N; j++)
    //     printf("%d\t", B[i][j]);
    //   printf ("\n");
    // }
    // for (int i = 0; i<N; i++) {
    //         for (int j = 0; j<N; j++)
    //             printf("%d\t", A[i][j] * B[i][j]);
    //         printf ("\n");
    //     }
    //     printf("\n\n");
    // printf("MATRIX A IN OFFSET: %d\n", offset);
    //     for (int i = 0; i<N; i++) {
    //         for (int j = 0; j<N; j++)
    //             printf("%d\t", A[i][j]);
    //         printf ("\n");
    //     }
    //     printf("\n\n");
    //     printf("MATRIX B IN OFFSET: %d\n", offset);
    //     for (int i = 0; i<N; i++) {
    //         for (int j = 0; j<N; j++)
    //             printf("%d\t", B[i][j]);
    //         printf ("\n");
    //     }
    //     printf("\n\n");
    }
    else {
        firstShiftMatrix(A,0);
        firstShiftMatrix(B,1);
        for(int i = 0; i < offset; i++){
            shiftMatrix(A,0);
            shiftMatrix(B,1);
        }
        // printf("MATRIX A IN OFFSET: %d\n", offset);
        // for (int i = 0; i<N; i++) {
        //     for (int j = 0; j<N; j++)
        //         printf("%d\t", A[i][j]);
        //     printf ("\n");
        // }
        // printf("\n\n");
        // printf("MATRIX B IN OFFSET: %d\n", offset);
        // for (int i = 0; i<N; i++) {
        //     for (int j = 0; j<N; j++)
        //         printf("%d\t", B[i][j]);
        //     printf ("\n");
        // }
        // printf("\n\n");
        
    }
    // for (int k = 0; k<N; k++) {
    //   for (int i = 0; i<rows; i++) {
    //     // Set initial value of the row summataion
    //     matrix_c[i][k] = 0.0;
    //     // Matrix A's element(i, j) will be multiplied with Matrix B's element(j, k)
    //     for (int j = 0; j<N; j++)
    //       matrix_c[i][k] = matrix_c[i][k] + matrix_a[i][j] * matrix_b[j][k];
    //   }
    // }
    // concatenateMatrixes(A, B, C[offset]);

    // Calculated result will be sent back to Root process (process 0) with message tag 2
    
    // Offset will be sent to Root, which determines the starting point of the calculated
    // value in matrix C 
    for (int i = 0; i<N; i++) {
            for (int j = 0; j<N; j++)
                C[i][j] = A[i][j] * B[i][j];
        }
    MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    // Number of rows the process calculated will be sent to root process
    // MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    // Resulting matrix with calculated rows will be sent to root process
    // printf("CREATED C MATRIX\n");
    // for (int i = 0; i<N; i++) {
    //   for (int j = 0; j<N; j++)
    //     printf("%d\t", C[i][j]);
    //   printf ("\n");
    // }
    // printf("\n");
    MPI_Send(&(C[0][0]), N*N, MPI_INT, 0, 2, MPI_COMM_WORLD);
  }

  MPI_Finalize();

    
    // showMatrixes(A, B, C);
    // // ---------------FROM THIS-----------------
    // for(int i = 0; i < N; i++) {
    //     // I see posibilities to concurrent this piece of code
    //     // On this time we can pass matrixes A,B and appropriate offset, 
    //     // which can say, how many times we have to shift matrix.
    //     if (i == 0) {
    //         firstShiftMatrix(A,0);
    //         firstShiftMatrix(B,1);
    //     }else{
    //         shiftMatrix(A,0);
    //         shiftMatrix(B,1);
    //     }
    //     concatenateMatrixes(A, B, C);
    // }
    // // --------------TO THIS--------------------
    // showMatrixes(A, B, C);
}

