#ifndef FUNCOES_H
#define FUNCOES_H

typedef struct {
  int *vetor;
  int tamanho;
  int id_thread;
  double tempo_execucao;
} DadosThread;

int comparar(const void *a, const void *b);
void *ordenar_thread(void *arg);
int *ler_arquivo(const char *nome_arquivo, int *tamanho);
void mesclar_blocos_ordenados(int *todos_numeros, int total_numeros, int num_threads, int tamanho_bloco, int *vetor_ordenado);

#endif
