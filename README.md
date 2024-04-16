# multiplyMatrixConcurrent

Atividade 1 do Laboratorio 3 da disciplina de programação concorrente.

Objetivo: Projetar, implementar em C a multiplicação de matrizes concorrentes e criar um relatório sobre sua performance.

Roteiro:

Roteiro para implementação e avaliação:
1. Comece implementando uma versa ̃o sequencial do programa para usá-la como referência para os testes de corretude.
2. Implemente o programa que realiza a multiplicação das matrizes de entrada de forma concorrente, seguindo todos os requisitos de implementação descritos acima.
3. Verifique a corretude da sua solução (matriz de sa ́ıda correta). Para isso, pode-se comparar a matriz de saída da versão concorrente com a matriz de sa ́ıda da versa ̃o sequencial. (sugestão, usar o comando diff < arq1 >< arq2 >, se retornar vazio significa que os arquivos sa ̃o iguais).
4. Avalie o tempo de execução de cada parte do programa usando matrizes de en- trada de dimensões 500X500, 1000X1000 e 2000X2000. Importante: Repita a execução de cada configuração pelo menos 3 vezes e registre o valor médio das medidas tomadas.
5. Calcule a aceleração e a eficiência alcançada, executando o programa concorrente com 1, 2, 4 e 8 threads.
6. Registre todos osdados levantados e calculados em uma TABELA, gere os gráficos de aceleração e eficiência, informe a configuração da máquina usada para os testes e reporte tudo isso em um documento PDF.

Requisistos de implementação: 

Os seguintes requisitos de implementação deverão ser atendidos:
1. As matrizes de entrada e saída serão do tipo float, com N linhas e M colunas.
2. As matrizes de entrada devem ser carregadas de arquivos binários previamente gerados, onde os dois primeiros valores (do tipo inteiro) indicam as dimenso ̃es da matriz (N e M), e os demais elementos (do tipo float) são a sequência de valores
da matriz.
3. As matrizes deverão ser representadas internamente como vetores de float (variável
do tipo ponteiro, alocada dinamicamente). (Nas vídeo-aulas e códigos que acompanham este Laboratório há exemplos dessa forma de representação.)
4. A matriz de saída deverá ser escrita em um arquivo bina ́rio, no mesmo formato dos arquivos de entrada.
5. O programa deverá receber como entrada, na linha de comando, os nomes dos arquivos de entrada e de sa ́ıda, e a quantidade de threads de processamento.
6. O programa deverá incluir chamadas de tomada de tempo de execução interna
do programa, separando as partes de inicialização, processamento e finalização do programa.

## Instruções do Programa

Rode os comandos a seguir para compilar as classes necessárias

``` bash
❯ gcc ./cods-lab3/geraMatrizBinario.c -o geraMatrizBinario
```

``` bash
gcc multiplyMatrixSequential.c -o seq  
```

``` bash
gcc -pthread multiplyMatrixConcurrent.c -o conc 
```

1. Para criar as matrizes binarias execute:

``` bash
./geraMatrizBinario <N> <M> <nome_arquivo>
```

Exemplo:
``` bash
./geraMatrizBinario 500 500 matrizA
```


2. Após gerar suas matrizes execute o seguinte comando para rodar de forma sequencial:
``` bash
./seq <matriz_a> <matriz_b> 
```

Exemplo:
``` bash
./seq matrizA matrizB
```
3. Execute o seguinte comando para rodar de forma concorrent:
``` bash
./conc <matriz_a> <matriz_b> <num_threads>
```

Exemplo:
``` bash
./conc matrizA matrizB 4
```