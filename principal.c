#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funcoes.h"

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Uso: %s <num_threads> <arquivo1> <arquivo2> ... -o <arquivo_saida>\n", argv[0]);
    return 1;
  }

  int num_threads = atoi(argv[1]);
  if (num_threads != 2 && num_threads != 4 && num_threads != 8) {
    fprintf(stderr, "Erro: O número de threads deve ser 2, 4 ou 8.\n");
    return 1;
  }

  int num_arquivos = argc - 4;
  char *arquivo_saida = argv[argc - 1];
  char **arquivos_entrada = &argv[2];

  int total_numeros = 0;
  int **vetores = malloc(num_arquivos * sizeof(int *));
  int *tamanhos = malloc(num_arquivos * sizeof(int));
  for (int i = 0; i < num_arquivos; i++) {
    vetores[i] = ler_arquivo(arquivos_entrada[i], &tamanhos[i]);
    total_numeros += tamanhos[i];
  }

  int *todos_numeros = malloc(total_numeros * sizeof(int));
  int posicao = 0;
  for (int i = 0; i < num_arquivos; i++) {
    memcpy(&todos_numeros[posicao], vetores[i], tamanhos[i] * sizeof(int));
    posicao += tamanhos[i];
  }

  int tamanho_bloco = total_numeros / num_threads;
  pthread_t threads[num_threads];
  DadosThread dados_thread[num_threads];

  struct timespec inicio_total, fim_total;
  clock_gettime(CLOCK_MONOTONIC, &inicio_total);

  for (int i = 0; i < num_threads; i++) {
    dados_thread[i].vetor = &todos_numeros[i * tamanho_bloco];
    dados_thread[i].id_thread = i;
    dados_thread[i].tamanho = (i == num_threads - 1) ? total_numeros - (i * tamanho_bloco) : tamanho_bloco;
    pthread_create(&threads[i], NULL, ordenar_thread, (void *)&dados_thread[i]);
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
    printf("Tempo de execução do Thread %d: %.5f segundos.\n", dados_thread[i].id_thread, dados_thread[i].tempo_execucao);
  }

  clock_gettime(CLOCK_MONOTONIC, &fim_total);
  double tempo_execucao_total = (fim_total.tv_sec - inicio_total.tv_sec) + (fim_total.tv_nsec - inicio_total.tv_nsec) / 1e9;
  printf("Tempo total de execução: %.5f segundos.\n", tempo_execucao_total);

  int *vetor_ordenado = malloc(total_numeros * sizeof(int));
  mesclar_blocos_ordenados(todos_numeros, total_numeros, num_threads, tamanho_bloco, vetor_ordenado);

  FILE *saida = fopen(arquivo_saida, "w");
  if (saida == NULL) {
    perror("Erro ao abrir o arquivo de saída");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < total_numeros; i++) {
    fprintf(saida, "%d\n", vetor_ordenado[i]);
  }
  fclose(saida);

  for (int i = 0; i < num_arquivos; i++) {
    free(vetores[i]);
  }
  free(vetores);
  free(tamanhos);
  free(todos_numeros);
  free(vetor_ordenado);

  return 0;
}
