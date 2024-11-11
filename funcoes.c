#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funcoes.h"

int comparar(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

void *ordenar_thread(void *arg) {
  DadosThread *dados = (DadosThread *)arg;
  struct timespec inicio, fim;
  clock_gettime(CLOCK_MONOTONIC, &inicio);

  qsort(dados->vetor, dados->tamanho, sizeof(int), comparar);

  clock_gettime(CLOCK_MONOTONIC, &fim);
  dados->tempo_execucao = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

  pthread_exit(NULL);
}

int *ler_arquivo(const char *nome_arquivo, int *tamanho) {
  FILE *arquivo = fopen(nome_arquivo, "r");
  if (arquivo == NULL) {
    perror("Erro ao abrir o arquivo");
    exit(EXIT_FAILURE);
  }

  int *vetor = malloc(1000 * sizeof(int));
  int contador = 0;
  while (fscanf(arquivo, "%d", &vetor[contador]) == 1) {
    contador++;
  }
  *tamanho = contador;
  fclose(arquivo);
  return vetor;
}

void mesclar_blocos_ordenados(int *todos_numeros, int total_numeros, int num_threads, int tamanho_bloco, int *vetor_ordenado) {
  int indices[num_threads];
  memset(indices, 0, sizeof(indices));

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
}
