#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// funções auxiliares ----------------------------------------------

double *aloca_memoria_double(int tam){
    double *ptr = (double *) malloc(tam*sizeof(double));
    if(ptr == NULL)
        printf("Erro de alocação:\n");
    return ptr;
}

void imprime_vetor_to_double(double *vet, int tam){
    for(int it=0;it<tam;it++){
        printf("%.4f\t",vet[it]);
    }
    printf("\n");
}

// gera vetores -----------------------------------------------------
double *gera_h(double *h,int length){

    h = aloca_memoria_double(length);

    double result[97] = {
          0.0008,	-0.0006,	-0.0019,	-0.0031,	-0.0041,	-0.0047,   -0.0049,   -0.0046,
         -0.0038,   -0.0024,	-0.0006,     0.0016,     0.0039,     0.0061,    0.0082,    0.0097,
          0.0106,	 0.0106,	 0.0095,	 0.0073,	 0.0039,	-0.0006,   -0.0061,	  -0.0122,
         -0.0188,	-0.0252,	-0.0311,    -0.0360,	-0.0392,	-0.0403,   -0.0388,   -0.0343,
         -0.0265,   -0.0152,	-0.0004,     0.0179,     0.0392,     0.0632,    0.0893,    0.1167,
          0.1447,    0.1723,     0.1985,     0.2226,     0.2437,     0.2609,    0.2737,    0.2816,
          0.2842,	 0.2816,     0.2737,     0.2609,     0.2437,     0.2226,    0.1985,	   0.1723,
          0.1447,	 0.1167,     0.0893,     0.0632,     0.0392,     0.0179,   -0.0004,   -0.0152,
         -0.0265,   -0.0343,    -0.0388,    -0.0403,    -0.0392,    -0.0360,   -0.0311,   -0.0252,
         -0.0188,   -0.0122,    -0.0061,    -0.0006,     0.0039,     0.0073,    0.0095,    0.0106,
          0.0106,    0.0097,     0.0082,     0.0061,     0.0039,     0.0016,   -0.0006,	  -0.0024,
         -0.0038,   -0.0046,    -0.0049,    -0.0047,    -0.0041,    -0.0031,   -0.0019,   -0.0006,
         0.0008
    };
    
    for(int i=0;i<length;i++){
        h[i] = result[i];
    }

    return h;
}

double *gera_vet(double *vet, int length){

}

double *zera_buffer(int length){
    double *ptr = aloca_memoria_double(length);
    for(int i=0;i<length;i++){
        ptr[i] = 0.0;
    }
    return ptr;
}

// funções convolução -----------------------------------------------
void fifo(double n, double *buffer, int tam){
    for(int i=tam-1;i>0;i--){
        buffer[i] = buffer[i-1]; 
    }
    buffer[0] = n;
}

double *convolucao(double *x, double *h, int h_len,
                    double *buffer, double *result, int tam_conv, bool same) {
    
    double *aux = aloca_memoria_double(tam_conv);   

    double soma;
    for(int i=0;i<tam_conv;i++){
        soma = 0;
        fifo(i<tam_conv-h_len+1?x[i]:0.0,buffer,h_len);
        for(int j=0;j<h_len;j++){
            soma += h[j]*buffer[j];
        } 
        aux[i] = soma; 
    }
  
    if(same){
        result = aloca_memoria_double(tam_conv-h_len+1);
        int it = h_len/2;
        for(int i=0;i<tam_conv-h_len-1;i++){
            result[i] = aux[it];
            it++;
        }
    }else{
        result = aloca_memoria_double(tam_conv);
        for(int i=0;i<tam_conv;i++){
            result[i] = aux[i];
        }
    }
    
    return result;
}


int main(){

    double *vet , *h;
    int N = 97, tam_vet = 100;

    h = gera_h(h,N); // vetor resposta ao impulso CORRETO
    vet = gera_vet(vet,tam_vet);



    return 0;
}