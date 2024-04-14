#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// Caso queira imprimir as matrizes descomentar
// #define PRINT

// Define tamanho de 10ˆ9 para calculo de tempo (conversao de nanosegundos)
#define BILLION 1000000000

// Estrutura usada para representar uma matriz
typedef struct {
    float * matrixArray;
    int cols;
    int rows;
} Matrix;

// Estrutura usada para passar argumentos para Thread
typedef struct {
    int id;
    int numThreads;
    Matrix matrixA;
    Matrix matrixB;
    Matrix *matrixC;
} ThreadArgs;

// Metodo que multiplica duas matrizes de dimensões NxM por MxK de forma concorrente
void multiplyMatrixConcurrent(Matrix matrixA, Matrix matrixB, Matrix * matrixC, int id, int numThreads) {

    // Cada thread multiplica n linhas da matriz, distribuidas alternadamente
    for (int i = id; i < matrixC -> rows; i += numThreads) {
        for (int j = 0; j < matrixC -> cols; j++) {
            matrixC -> matrixArray[i * matrixB.cols + j] = 0;
            for (int k = 0; k < matrixA.cols; k++) {
                matrixC -> matrixArray[i * matrixB.cols + j] += matrixA.matrixArray[i * matrixA. cols + k] * matrixB. matrixArray[k * matrixB. cols + j];
            }
        }
    }
}

// Metodo que lê a matriz a partir de um arquivo binário
Matrix *readMatrixFromFile(char * filename) {

    FILE * file = fopen(filename, "rb");
    if(!file) {
        fprintf(stderr, "Erro na abertura do arquivo\n");
        return NULL;
    }

    int rows;
    int cols;

    size_t ret;

    ret = fread(&rows, sizeof(int), 1, file);
    if(!ret) {
        fprintf(stderr, "Erro de leitura das dimensoes da matrix arquivo \n");
        return NULL;
    }

    ret = fread(&cols, sizeof(int), 1, file);
    if(!ret) {
        fprintf(stderr, "Erro de leitura das dimensoes da matrix arquivo \n");
        return NULL;
    }

    float * matrixArray = (float*) malloc(sizeof(float) * rows * cols);
    if(!matrixArray) {
        fprintf(stderr, "Erro de alocao da memoria da matrix\n");
        return NULL;
    }

    ret = fread(matrixArray, sizeof(float), rows * cols , file);
    if(ret < rows * cols) {
        fprintf(stderr, "Erro de leitura dos elementos da matrix\n");
        return NULL;
    }

    Matrix *matrix = malloc(sizeof (*matrix));
    if (!matrix) {
        fprintf(stderr, "Erro de alocao da memoria da matrix\n");
        return NULL;
    }

    matrix -> rows = rows;
    matrix -> cols = cols;
    matrix -> matrixArray = matrixArray;

    fclose(file);

    return matrix;
}

void printMatrix(Matrix *matrix) {
    for(int i=0; i<matrix -> rows; i++) {
        for(int j=0; j<matrix -> cols; j++)
            fprintf(stdout, "%.6f ", matrix -> matrixArray[i*matrix -> cols+j]);
        fprintf(stdout, "\n");
    }
}

void *multiplyMatrixThread(void *args) {
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    ThreadArgs *tArgs = (ThreadArgs *)args;

    multiplyMatrixConcurrent(tArgs -> matrixA, tArgs -> matrixB, tArgs -> matrixC, tArgs -> id, tArgs -> numThreads);

    clock_gettime(CLOCK_MONOTONIC, &end);

    int64_t diff = ((int64_t)end.tv_sec - (int64_t)start.tv_sec) * (int64_t) BILLION
                   + ((int64_t)end.tv_nsec - (int64_t)start.tv_nsec);

    printf("--Thread %d terminou a execução. Tempo: %f segundos\n", tArgs -> id, (float)  diff / BILLION);

    free(args);
    pthread_exit(NULL);
}

void writeMatrixToFile(Matrix * matrix, char * filename) {

    size_t ret;
    long long int size = matrix->rows*matrix->cols;

    FILE * file = fopen(filename, "wb");
    if(!file) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return;
    }

    //escreve numero de linhas e de colunas
    ret = fwrite(&matrix->rows, sizeof(int), 1, file);
    ret = fwrite(&matrix->cols, sizeof(int), 1, file);

    ret = fwrite(matrix->matrixArray, sizeof(float), size, file);

    if(ret < size) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return;
    }

    fclose(file);
}

int main(int argc, char*argv[]) {

    if(argc < 3) {
        fprintf(stderr, "Digite: ./main <arquivoMatrixA> <arquivoMatrixB> <numThreads>\n");
        return -1;
    }

    Matrix *matrixA = readMatrixFromFile(argv[1]);
    if (!matrixA) {
        printf("Erro ao ler matrixA do arquivo\n");
        free(matrixA);
        return -2;
    }

    Matrix *matrixB = readMatrixFromFile(argv[2]);
    if (!matrixB) {
        printf("Erro ao ler matrixB do arquivo\n");
        free(matrixB);
        return -2;
    }

    if (matrixA->cols != matrixB->rows) {
        printf("Erro, para a multiplicação ser possivel o numero de colunas de A deve ser igual ao numero de linhas de B.");
        return -3;
    }

    Matrix *matrixC = malloc(sizeof(Matrix));
    if (!matrixC) {
        printf("Erro ao alocar memoria para matrixC\n");
        free(matrixC);
        return -4;
    }

    float *mArray = (float *) malloc(sizeof(float) * matrixA->rows * matrixB->cols);
    if (!mArray) {
        printf("Erro ao alocar memoria para matrixC\n");
        free(mArray);
        return -4;
    }

    matrixC -> matrixArray = mArray;
    matrixC -> rows = matrixA->rows;
    matrixC -> cols = matrixB->cols;

    int numThreads = atoi(argv[3]);
    pthread_t tid_sistema[numThreads];
    int threads[numThreads];

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < numThreads; i++) {

        ThreadArgs * args = malloc(sizeof (ThreadArgs));
        threads[i] = i;
        args->id = i;
        args->numThreads = numThreads;
        args->matrixA = *matrixA;
        args->matrixB = *matrixB;
        args->matrixC = matrixC;

        if (pthread_create(&tid_sistema[i], NULL, multiplyMatrixThread, args)) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(tid_sistema[i],NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    int64_t diff = ((int64_t)end.tv_sec - (int64_t)start.tv_sec) * (int64_t) BILLION
                   + ((int64_t)end.tv_nsec - (int64_t)start.tv_nsec);

    printf("#Tempo (concorrente): %f segundos\n", (float)  diff / BILLION);

#ifdef PRINT
    printMatrix(matrixC);
#endif

    writeMatrixToFile(matrixC, "concMatrixC");

    free(mArray);
    free(matrixA);
    free(matrixB);
    free(matrixC);

    return 0;
}