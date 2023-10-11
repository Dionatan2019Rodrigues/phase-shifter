#ifndef FUNCOES_H
#define FUNCOES_H

#define MAX_TAM_LINE_CSV 200000 // tamanho definido pelo programador
// escolhido para que todo o arquivo seja considerado na mesma linha 
// e assim separado corretamente apenas pelo delimitador ','

#define PASSO_ADAPTACAO 0.4 
#define ORDEM_VET_PESOS 3

//Auxiliares --------------------------------------------
double *aloca_memory(int tam);

FILE *open_file(FILE *arq, const char *nomeArquivo);

void imprime_parte_vet(double *vet, int ini, int fim, const char *nomeVet);

void imprime_intervalos(double *x, double *d, double *output, char *nx, char *nd,  char *nout);

//Contagem de tamanho e atribuição do vetor --------------
int conta_elemento(FILE *arq);

void atribui_vetor(FILE *arq, double *vet);

//Filtro adptativo  --------------------------------------
void adaptive_filter(double *x, double *d, double *output, int tam_sinal);

//Erro médio quadrádico no gráfico -----------------------
void escreve_arq(double *vet, int tam);

void calcula_mse(double *x, double *out,int tam);

void gera_grafico();

#endif // FUNCOES_H