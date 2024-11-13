#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funcoes.h"

// Função de comparação usada para ordenar números inteiros
int comparar(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

// Função executada por cada thread para ordenar um bloco do vetor
void *ordenar_thread(void *arg) {
  DadosThread *dados = (DadosThread *)arg;
  struct timespec inicio, fim;
  clock_gettime(CLOCK_MONOTONIC, &inicio);

  qsort(dados->vetor, dados->tamanho, sizeof(int), comparar);

  clock_gettime(CLOCK_MONOTONIC, &fim);
  dados->tempo_execucao = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

  pthread_exit(NULL);
}

// Função para ler números de um arquivo e armazená-los em um vetor
int *ler_arquivo(const char *nome_arquivo, int *tamanho) {
  FILE *arquivo = fopen(nome_arquivo, "r");
  if (arquivo == NULL) {
    perror("Erro ao abrir o arquivo");
    exit(EXIT_FAILURE);
  }

  int *vetor = NULL;  // Ponteiro inicial nulo
  int contador = 0;

  int valor;
  while (fscanf(arquivo, "%d", &valor) == 1) {
    // Realoca o vetor para um espaço adicional para o novo número
    int *temp = realloc(vetor, (contador + 1) * sizeof(int));
    if (temp == NULL) {
      perror("Erro ao redimensionar memória");
      free(vetor);
      fclose(arquivo);
      exit(EXIT_FAILURE);
    }
    vetor = temp;

    vetor[contador] = valor;
    contador++;
  }

  *tamanho = contador;
  fclose(arquivo);
  return vetor;
}


// Função para mesclar blocos ordenados de números em um único vetor ordenado
void mesclar_blocos_ordenados(int *todos_numeros, int total_numeros, int num_threads, int tamanho_bloco, int *vetor_ordenado) {
  int *indices = malloc(num_threads * sizeof(int));
  memset(indices, 0, num_threads * sizeof(int));

  for (int i = 0; i < total_numeros; i++) {
    int menor_valor = __INT_MAX__;
    int indice_menor = -1;

    for (int j = 0; j < num_threads; j++) {
      int inicio = j * tamanho_bloco;
      int fim = (j == num_threads - 1) ? total_numeros : inicio + tamanho_bloco;

      if (indices[j] < (fim - inicio)) {
        int valor_atual = todos_numeros[inicio + indices[j]];
        if (valor_atual < menor_valor) {
          menor_valor = valor_atual;
          indice_menor = j;
        }
      }
    }

    vetor_ordenado[i] = menor_valor;
    indices[indice_menor]++;
  }

  free(indices);
}