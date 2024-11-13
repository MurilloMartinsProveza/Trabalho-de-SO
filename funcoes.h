#ifndef FUNCOES_H
#define FUNCOES_H

// Estrutura para armazenar informações de cada thread
typedef struct {
  int *vetor;            // Ponteiro para o vetor de números que a thread irá ordenar
  int tamanho;           // Tamanho do vetor que a thread irá processar
  int id_thread;         // ID da thread, para identificação
  double tempo_execucao; // Tempo de execução da thread, armazenado para análise de desempenho
} DadosThread;

// Função para comparar dois elementos, usada para ordenação
int comparar(const void *a, const void *b);

// Função para ordenar um vetor de números em uma thread
void *ordenar_thread(void *arg);

// Função para ler números de um arquivo e armazená-los em um vetor
int *ler_arquivo(const char *nome_arquivo, int *tamanho);

// Função para mesclar blocos ordenados de números em um único vetor ordenado
void mesclar_blocos_ordenados(int *todos_numeros, int total_numeros, int num_threads, int tamanho_bloco, int *vetor_ordenado);

#endif
