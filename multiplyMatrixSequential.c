#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

// Estrutura usada para representar uma matriz
typedef struct {
    float * matrixArray;
    int cols;
    int rows;
} Matrix;

// Caso queira imprimir as matrizes descomentar
// #define PRINT

// Metodo que multiplica duas matrizes de dimensões NxM por M*K
void multiplyMatrix(Matrix * matrixA, Matrix * matrixB, Matrix * matrixC) {

    for (int i = 0; i < matrixC -> rows; i++) {
        for (int j = 0; j < matrixC -> cols; j++) {
            matrixC -> matrixArray[i * matrixB -> cols + j] = 0;
            for (int k = 0; k < matrixA -> cols; k++) {
                matrixC -> matrixArray[i * matrixB -> cols + j] += matrixA -> matrixArray[i * matrixA -> cols + k] * matrixB -> matrixArray[k * matrixB -> cols + j];
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

// Metodo que imprime a matriz
void printMatrix(Matrix *matrix) {
    for(int i=0; i<matrix -> rows; i++) {
        for(int j=0; j<matrix -> cols; j++)
            fprintf(stdout, "%.6f ", matrix -> matrixArray[i*matrix -> cols+j]);
        fprintf(stdout, "\n");
    }
}

// Metodo que escreve a mtriz em um arquivo binário
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


// Escreve os resultados no CSV
void writeToCSV(char *filename, char tipo, int numThreads, int rows, int cols, double init, double process, double end) {

    char *header = "dimensao,tipo,threads,inicialização,processamento,finalização";

    FILE *file;

    // Verifica se arquivo existe
    file = fopen(filename,"r");

    // Caso não exista cria o arquivo e escreve o header
    if (!file) {
        file = fopen(filename, "w");
        fprintf(file, "%s\n", header);
        fclose(file);
    }

    // Prepara o arquivo para acrescentar novas linhas
    file = fopen(filename, "a");

    fprintf(file, "%dx%d,%c,%d,%lf,%lf,%lf\n", rows, cols, tipo, numThreads, init, process, end);

    fclose(file);
}

// Metodo main que executa a multiplicação de matrizes de forma sequencial
int main(int argc, char*argv[]) {

    double startInit, finishInit, elapsedInit;
    GET_TIME(startInit);

    // Verifica argumentos passados para o programa
    if(argc < 2) {
        fprintf(stderr, "Digite: ./main <arquivoMatrixA> <arquivoMatrixB>\n");
        return -1;
    }

    // Le a primeira matriz
    Matrix *matrixA = readMatrixFromFile(argv[1]);
    if (!matrixA) {
        fprintf(stderr, "Erro ao ler matrixA do arquivo\n");
        free(matrixA);
        return -2;
    }

    // Le a segunda matriz
    Matrix *matrixB = readMatrixFromFile(argv[2]);
    if (!matrixB) {
        fprintf(stderr, "Erro ao ler matrixB do arquivo\n");
        free(matrixB);
        return -2;
    }

    // Verifica se as matrizes podem ser multiplicadas
    if (matrixA->cols != matrixB->rows) {
        fprintf(stderr, "Erro, para a multiplicação ser possivel o numero de colunas de A deve ser igual ao numero de linhas de B.");
        return -3;
    }

    // Aloca memótia para a matriz resultado
    Matrix *matrixC = malloc(sizeof(Matrix));
    if (!matrixC) {
        fprintf(stderr, "Erro ao alocar memoria para matrixC\n");
        free(matrixC);
        return -4;
    }

    // Aloca memoria para os elementos da matriz, representados na forma de um array
    float *mArray = (float *) malloc(sizeof(float) * matrixA->rows * matrixB->cols);
    if (!mArray) {
        fprintf(stderr, "Erro ao alocar memoria para matrixC\n");
        free(mArray);
        return -4;
    }

    matrixC -> matrixArray = mArray;
    matrixC -> rows = matrixA->rows;
    matrixC -> cols = matrixB->cols;

    int rows = matrixC -> rows;
    int cols = matrixC -> cols;

    GET_TIME(finishInit);
    elapsedInit = finishInit - startInit;

    printf("#Tempo (inicialização): %lf segundos\n", elapsedInit);

    double startProcess, finishProcess, elapsedProcess;
    GET_TIME(startProcess);

    // Executa multiplicação das mtrizes
    multiplyMatrix(matrixA, matrixB, matrixC);

    GET_TIME(finishProcess);
    elapsedProcess = finishProcess - startProcess;

    printf("#Tempo (processamento): %lf segundos\n", elapsedProcess);

    double startEnd, finishEnd, elapsedEnd;
    GET_TIME(startEnd);

#ifdef PRINT
    printMatrix(matrixC);
#endif

    // Escreve matriz resultado para arquivo
    writeMatrixToFile(matrixC, "seqMatrixC");

    // Libera memoria alcoada anteriormente
    free(mArray);
    free(matrixA);
    free(matrixB);
    free(matrixC);

    GET_TIME(finishEnd);
    elapsedEnd = finishEnd - startEnd;

    printf("#Tempo (finalização): %lf segundos\n", elapsedEnd);

    // Escreve medidas de tempo em CSV
    writeToCSV("medidasTempo.csv", 'S', 1, rows, cols, elapsedInit, elapsedProcess, elapsedEnd);

    return 0;
}

