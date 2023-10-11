#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"funcoes.h"
//#include"gnuplot.h"
//#include<mat.h>

int main(){
    double *d, *x, *output;
    FILE *arq_x, *arq_d;
    int tam_x, tam_d;

    //leitura de tamanho -----------------------------------------------
    arq_x = open_file(arq_x,"database/x.csv");
    arq_d = open_file(arq_d,"database/d.csv");
    tam_x = conta_elemento(arq_x);
    tam_d = conta_elemento(arq_d);

    //aloca memoria  ---------------------------------------------------
    x = aloca_memory(tam_x);
    d = aloca_memory(tam_d);
    output = aloca_memory(tam_x);

    //atribui vetor ----------------------------------------------------
    atribui_vetor(arq_x,x);
    atribui_vetor(arq_d,d);
    fclose(arq_x); fclose(arq_d);

    //filtro adaptativo ------------------------------------------------
    adaptive_filter(x,d,output,tam_x);

    //grafico
    calcula_mse(x,output,tam_x);
    gera_grafico();

    free(x); free(d); free(output);
    return 0;
}