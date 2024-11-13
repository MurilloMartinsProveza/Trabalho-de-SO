#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "funcoes.h"  // Inclui as funções auxiliares (como `ler_arquivo`, `ordenar_thread`, e `mesclar_blocos_ordenados`)

int main(int argc, char *argv[]) {

  // Verifica os argumentos da linha de comando
  if (argc < 4) 
  {
    fprintf(stderr, "Uso: %s <num_threads> <arquivo1> <arquivo2> ... -o <arquivo_saida>\n", argv[0]);
    return 1;
  }

  // Converte o primeiro argumento para o número de threads desejado
  int num_threads = atoi(argv[1]);

  // Verifica se o número de threads é válido (2, 4 ou 8)
  if (num_threads != 2 && num_threads != 4 && num_threads != 8) 
  {
    fprintf(stderr, "Erro: O número de threads deve ser 2, 4 ou 8.\n");
    return 1;
  }

  // Define o número de arquivos de entrada e o nome do arquivo de saída
  int num_arquivos = argc - 4;
  char *arquivo_saida = argv[argc - 1];
  char **arquivos_entrada = &argv[2];

  // Inicializa variáveis para armazenar os dados dos arquivos
  int total_numeros = 0;
  int **vetores = malloc(num_arquivos * sizeof(int *));
  int *tamanhos = malloc(num_arquivos * sizeof(int));

  // Lê cada arquivo e armazena seus dados em vetores
  for (int i = 0; i < num_arquivos; i++) {
    vetores[i] = ler_arquivo(arquivos_entrada[i], &tamanhos[i]);
    total_numeros += tamanhos[i];
  }

  // Cria um vetor único para armazenar todos os números
  int *todos_numeros = malloc(total_numeros * sizeof(int));
  int posicao = 0;

  // Copia os dados de cada vetor individual para o vetor único
  for (int i = 0; i < num_arquivos; i++) {
    memcpy(&todos_numeros[posicao], vetores[i], tamanhos[i] * sizeof(int));
    posicao += tamanhos[i];
  }

  // Define o tamanho de cada bloco para cada thread
  int tamanho_bloco = total_numeros / num_threads;

  // Aloca memória dinamicamente para as threads e dados de cada thread
  pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
  DadosThread *dados_thread = malloc(num_threads * sizeof(DadosThread));

  // Marca o tempo de início do processamento
  struct timespec inicio_total, fim_total;
  clock_gettime(CLOCK_MONOTONIC, &inicio_total);

  // Cria threads para ordenar cada bloco
  for (int i = 0; i < num_threads; i++) {
    dados_thread[i].vetor = &todos_numeros[i * tamanho_bloco];
    dados_thread[i].id_thread = i;
    dados_thread[i].tamanho = (i == num_threads - 1) ? total_numeros - (i * tamanho_bloco) : tamanho_bloco;
    pthread_create(&threads[i], NULL, ordenar_thread, (void *)&dados_thread[i]);
  }

  // Espera todas as threads terminarem e exibe o tempo de execução de cada uma
  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
    printf("Tempo de execução do Thread %d: %.5f segundos.\n", dados_thread[i].id_thread, dados_thread[i].tempo_execucao);
  }

  // Marca o tempo de fim do processamento
  clock_gettime(CLOCK_MONOTONIC, &fim_total);
  double tempo_execucao_total = (fim_total.tv_sec - inicio_total.tv_sec) + (fim_total.tv_nsec - inicio_total.tv_nsec) / 1e9;
  printf("Tempo total de execução: %.5f segundos.\n", tempo_execucao_total);

  // Mescla os blocos ordenados em um vetor final ordenado
  int *vetor_ordenado = malloc(total_numeros * sizeof(int));
  mesclar_blocos_ordenados(todos_numeros, total_numeros, num_threads, tamanho_bloco, vetor_ordenado);

  // Grava o vetor ordenado no arquivo de saída
  FILE *saida = fopen(arquivo_saida, "w");
  if (saida == NULL) {
    perror("Erro ao abrir o arquivo de saída");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < total_numeros; i++) {
    fprintf(saida, "%d\n", vetor_ordenado[i]);
  }
  fclose(saida);

  // Libera a memória alocada
  for (int i = 0; i < num_arquivos; i++) {
    free(vetores[i]);
  }
  free(vetores);
  free(tamanhos);
  free(todos_numeros);
  free(vetor_ordenado);
  free(threads);
  free(dados_thread);

  return 0;
}
