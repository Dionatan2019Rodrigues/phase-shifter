#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"funcoes.h"

//Auxiliares --------------------------------------------

double *aloca_memory(int tam){
    double *ptr = (double*)malloc(tam*sizeof(double));
    if(ptr==NULL)
        printf("ERRO DE ALOCAÇÃO\n");

    return ptr;
}

FILE *open_file(FILE *arq, const char *nomeArquivo){
    arq = fopen(nomeArquivo, "r");
    if(arq==NULL)
        printf("ERRO LEITURA DE ARQUIVO\n");

    return arq;
}

void imprime_parte_vet(double *vet, int ini, int fim, const char *nomeVet){
    printf("%s ",nomeVet);
    for(int i=ini;i<=fim;i++){
        printf("%f   ",vet[i]);
    }
    printf("\n");
}

void imprime_intervalos(double *x, double *d, double *output,  char *nx, char *nd,  char *nout){
    int k_ini = 0, k_fim = 9;
    for(int i=0;i<5;i++){
        imprime_parte_vet(x,k_ini,k_fim,nx);
        imprime_parte_vet(output,k_ini,k_fim,nout);
        imprime_parte_vet(d,k_ini,k_fim,nd);
        k_ini += 10;
        k_fim += 10;
        printf("\n");
    } 
}

//Contagem de tamanho e atribuição do vetor --------------
int conta_elemento(FILE *arq){
    int numElementos = 0;
    char linha[MAX_TAM_LINE_CSV]; 

    while (fgets(linha, sizeof(linha), arq)) {
        char *token = strtok(linha, ",");
        while (token != NULL) {
            numElementos++;
            token = strtok(NULL, ",");
        }
    }
    return numElementos;
}

void atribui_vetor(FILE *arq, double *vet){
    int it = 0;
    char linha[MAX_TAM_LINE_CSV]; 

    // Reposiciona o ponteiro para o início do arquivo
    if (fseek(arq, 0, SEEK_SET) != 0) 
        printf("Erro ao reposicionar o ponteiro do arquivo.\n"); 
    
    while(fgets(linha, sizeof(linha), arq)) {
        char *token = strtok(linha, ",");
        while(token != NULL){
            vet[it] = atof(token); // atof -> converte char para double
            it++;
            token = strtok(NULL, ",");
        }
    }
}   

//Filtro adptativo  --------------------------------------
void adaptive_filter(double *x, double *d, double *output, int tam_sinal){
    double w[ORDEM_VET_PESOS]= {0}; //vetor de pesos inicializado com valores nulos
    double erro = 0; // error inicializado com zero
    const double n = PASSO_ADAPTACAO; // passo de adaptação inicializado

    for(int i=0;i<tam_sinal;i++){
        double y = 0;

        //calcula saída filtrada
        for(int j=0;j<ORDEM_VET_PESOS;j++){
            y += w[j] * x[i-ORDEM_VET_PESOS+1+j]; //k da formula utilizado como j ,porque iteram igualmente
            //printf("%d\n",i-ORDEM_VET_PESOS+1+j);
        }

        //calcula o error para cada amostra de saída y
        erro = d[i] - y;

        //atualiza coeficiêntes do filtro
        for(int j=0;j<ORDEM_VET_PESOS;j++){
            w[j] += n*erro*x[j];
        }
        //atualiza sinal de saída 
        output[i] = y;

    }
    /*
    imprime_parte_vet(x,10,20,"x  ->");
    imprime_parte_vet(output,10,20,"out->");
    imprime_parte_vet(d,10,20,"d  ->");
    */
    imprime_intervalos(x,d,output,"x  ->","d  ->","out->");

}

//Erro médio quadrádico no gráfico -----------------------
void escreve_arq(double *vet, int tam){
    FILE *arq = fopen("database/dados-mse.txt", "w");
    if (arq == NULL) 
        printf("Erro ao criar arquivo de dados.\n");

    // Escreve os dados no arquivo
    for (int i = 0; i < tam; i++) {
        fprintf(arq, " %f\n", vet[i]);
    }
    fclose(arq);
}

void calcula_mse(double *x, double *out,int tam){
    double erro;
    double *mse = aloca_memory(tam);

    for(int i=0;i<tam;i++) {
       // erro = x[i] - out[i];
        //mse[i] = erro * erro;
        mse[i]= out[i];
    }

    escreve_arq(mse,tam);
    free(mse);

}

void gera_grafico(){
    FILE *gnuplot = popen("gnuplot", "w");
    if (gnuplot == NULL) 
        printf("Erro ao abrir o gnuplot.\n");

    fprintf(gnuplot, "plot 'database/dados-mse.txt' with lines\n");
    fprintf(gnuplot, "pause 60\n");  // Mantém o gráfico aberto por 60s

    // Fecha o gnuplot
    pclose(gnuplot);
}
