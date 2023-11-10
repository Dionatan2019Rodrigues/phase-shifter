/* Desenvolvido por Dionatan Rodrigues - derodrigues@inf.ufsm.br*/
#include <stdio.h>
#include <stdlib.h> // necessário para usar função malloc() e rand()
#include <string.h> // necessário para usar função strtok()
#include <math.h>   // necessário para função gera_h_rrc()
#include <stdbool.h> // necessário para parâmetro same da função convolucao()
#include <complex.h> // necessário para transformada de fourier

#define pi 3.14159265358979323846

#define MAX_TAM_LINE_CSV 200000 // tamanho definido pelo programador
// escolhido para que todo o arquivo seja considerado na mesma linha 
// e assim separado corretamente apenas pelo delimitador ','

//funções auxiliares -------------------------------------------------
int *aloca_memoria(int *ptr , int tam){
    ptr = (int *) malloc(tam*sizeof(int));
    if(ptr == NULL)
        printf("Erro de alocação:\n");
    return ptr;
}

void plota_constelacao(float *i, float *q, int length){
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");

    if (gnuplotPipe == NULL)
        printf("Erro ao iniciar o GNUplot.\n");

    // Configurar o gráfico
    fprintf(gnuplotPipe, "set title 'Constelação 16-QAM (I vs. Q)'\n");
    fprintf(gnuplotPipe, "set xlabel 'I'\n");
    fprintf(gnuplotPipe, "set ylabel 'Q'\n");

    // Plotar os símbolos da constelação
    fprintf(gnuplotPipe, "plot '-' using 1:2 title '16-QAM' with points pointtype 7\n");

    for (int it=0;it<length;it++) {
        fprintf(gnuplotPipe, "%lf %lf\n", i[it], q[it]);
    }

    fprintf(gnuplotPipe, "e\n");
    fclose(gnuplotPipe);

}

float *aloca_memoria_float(int tam){
    float *ptr = (float *) malloc(tam*sizeof(float));
    if(ptr == NULL)
        printf("Erro de alocação:\n");
    return ptr;
}

FILE * open_file_sinal(const char *nomeArquivo){
    FILE *ptr = fopen(nomeArquivo, "r");
    if(ptr==NULL)
        printf("ERRO LEITURA DE ARQUIVO\n");

    return ptr;
}

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

void imprime_vetor(float *vet, int tam){
    for(int it=0;it<tam;it++){
        printf("%.4f\t",vet[it]);
    }
    printf("\n");
}

void imprime_vetor_toint(int *vet, int tam){
    for(int it=0;it<tam;it++){
        printf("%d ",vet[it]);
    }
    printf("\n");
}
 
void double_to_int(double *input, int *output, int length) {
    for (int i = 0; i < length; i++) {
        output[i] = (int)input[i];
    }
}

// atribui sinal recebido a um vetor ---------------------------------
void atribui_vetor(FILE *arq, int *vet, int tam){
    int it = 0;
    char linha[tam]; 

    // Reposiciona o ponteiro para o início do arquivo
    if (fseek(arq, 0, SEEK_SET) != 0) 
        printf("Erro ao reposicionar o ponteiro do arquivo.\n");
    
    while(fgets(linha, sizeof(linha), arq)) {
        char *token = strtok(linha, ",");
        while(token != NULL){
            vet[it] = atoi(token); // atoi -> converte char para int
            it++;
            token = strtok(NULL, ",");
        }
    }
}

// funções de modulação  ---------------------------------------------
void mapper_reserva(int *sequencia, float *Ii, float *Q, int tam) {
    int j = 0, sequencia_dec[tam/4];
    for (int i = 0; i < tam / 4; i++) {
        sequencia_dec[i] = sequencia[j] * 8 + sequencia[j + 1] * 4 + sequencia[j + 2] * 2 + sequencia[j + 3];
        j = j + 4;
    }

    for (int i = 0; i < tam / 4; i++) {
        switch (sequencia_dec[i]) {
            case 0:
                Ii[i] = -3;
                Q[i] = 3;
                break;
            case 1:
                Ii[i] = -3;
                Q[i] = 1;
                break;
            case 2:
                Ii[i] = -3;
                Q[i] = -3;
                break;
            case 3:
                Ii[i] = -3;
                Q[i] = -1;
                break;
            case 4:
                Ii[i] = -1;
                Q[i] = 3;
                break;
            case 5:
                Ii[i] = -1;
                Q[i] = 1;
                break;
            case 6:
                Ii[i] = -1;
                Q[i] = -3;
                break;
            case 7:
                Ii[i] = -1;
                Q[i] = -1;
                break;
            case 8:
                Ii[i] = 3;
                Q[i] = 3;
                break;
            case 9:
                Ii[i] = 3;
                Q[i] = 1;
                break;
            case 10:
                Ii[i] = 3;
                Q[i] = -3;
                break;
            case 11:
                Ii[i] = 3;
                Q[i] = -1;
                break;
            case 12:
                Ii[i] = 1;
                Q[i] = 3;
                break;
            case 13:
                Ii[i] = 1;
                Q[i] = 1;
                break;
            case 14:
                Ii[i] = 1;
                Q[i] = -3;
                break;
            case 15:
                Ii[i] = 1;
                Q[i] = -1;
                break;
            default:
                break;
        }
    }
}

void upsampler(float*vet, float *vet_up, int k, int tam_iq){
    
    for(int it=0;it<tam_iq;it++){
        vet_up[it * k] = vet[it];
        for(int itera=1;itera<k;itera++){
            vet_up[itera] = 0;
        }
        //printf("%d\t",it);
    }

}

// funções de filtragem 
void fifo(float n, float *buffer, int tam){
    for(int i=tam-1;i>0;i--){
        buffer[i] = buffer[i-1]; 
    }
    buffer[0] = n;
}

float *convolucao(float *x, float *h, int h_len,
                    float *buffer, float *result, int tam_conv, bool same) {
    
    float *aux = aloca_memoria_float(tam_conv);   

    float soma;
    for(int i=0;i<tam_conv;i++){
        soma = 0;
        fifo(i<tam_conv-h_len+1?x[i]:0.0,buffer,h_len);
        for(int j=0;j<h_len;j++){
            soma += h[j]*buffer[j];
        } 
        aux[i] = soma; 
    }
  
    if(same){
        result = aloca_memoria_float(tam_conv-h_len+1);
        int it = h_len/2;
        for(int i=0;i<tam_conv-h_len-1;i++){
            result[i] = aux[it];
            it++;
        }
    }else{
        result = aloca_memoria_float(tam_conv);
        for(int i=0;i<tam_conv;i++){
            result[i] = aux[i];
        }
    }
    //imprime_vetor(result,tam_conv-h_len+1);
    return result;
}

float *filtro_passa_baixa(float *vet, int tam_vet_up){
    float h[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    float buffer[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    int tam_h = 16, tam_conv = tam_vet_up + tam_h -1;
    float *result;
    //convolução
    result = convolucao(vet,h,tam_h,buffer,result,tam_conv,true);
    return result;
}

float *gaussiano(float *vet, int tam_vet){
    float h[] = {0.0030, 0.0118, 0.0390, 0.1051, 0.2367, 0.4445, 0.6972, 0.9138,
                   1.0000, 0.9138, 0.6972, 0.4445, 0.2367, 0.1051, 0.0390, 0.0118};
    float buffer[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    int tam_h = 16, tam_conv = tam_vet + 2*16 -1;
    float *result = aloca_memoria_float(tam_conv);

    //convolução 
    result = convolucao(vet,h,tam_h,buffer,result,tam_conv,false); 
    //imprime_vetor(result,tam_conv);
    return result;  
    
}

float *gera_h_rrc(float *h, int N, int k, float alpha){
    
    float result[97] = {
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
    for(int i=0;i<N;i++){
        h[i] = result[i];
    }

    return h;
    /*
    for(int i=0;i<N;i++){
        h[i] = (4*alpha)/(pi*k) *  

            ( cos(((i-((N-1)/2))/k)*pi*(1+alpha)) 
                + (pi*(1-alpha)/4*alpha) * sin(((i-((N-1)/2))/k)*pi*(1-alpha))
            
              / 1-(((i-((N-1)/2))/k)*4*alpha)
            );
    }
    */

}

float *zera_buffer(int tam){
    float *ptr = aloca_memoria_float(tam);
    for(int i=0;i<tam;i++){
        ptr[i] = 0.0;
    }
    return ptr;
}

float *rrc(float *vet, int k, int tam_vet, int N){
    float *h = aloca_memoria_float(N), alpha = 0.5, *buffer = zera_buffer(N);
    h = gera_h_rrc(h,N,k,alpha);

    int tam_conv = tam_vet + N -1;
    float *result = aloca_memoria_float(tam_conv);  

    //convolução
    result = convolucao(vet,h,N,buffer,result,tam_conv,false);

    return result;
}

/*
void abs_fftshift_fft_u(float *u, int length){
    complex double *aux = u;
    float *temp = fft(aux,temp,length);

    fftshift(temp);
    for(int i=0;i<length;i++){
        u[i] = abs(temp[i]);
    }
}*/

float *tx_heterodinacao(float *u, float *i, float *q, int length, float fc, float fs){
    float ui[length], uq[length];
    u = aloca_memoria_float(length);
    for(int it=0;it<length;it++){
        ui[it] = i[it] *cos(2*pi*fc*it/fs);
        uq[it] = q[it] *sin(2*pi*fc*it/fs);
        u[it] = ui[it] - uq[it];
    }
    return u;
}

void firpm(float *h, int filterLength, float *f, float *a) {
    //float sum;

    for(int i=0;i<filterLength;i++){
        h[i] = 0;
        for (int j=0;j<sizeof(f)/sizeof(f[0]);j++) {
            if (i == 0) 
                h[i] += 2.0 * f[j];
            else
                h[i] += 2.0 * f[j] * cos(2.0 * pi * i * f[j]); 
        }
        h[i] /= filterLength;
    }
    // Aplicar amplitudes desejadas
    for(int i = 0; i < filterLength; i++){
        h[i] *= a[i];
    }
}

float *att_z(float *z, int length, float value){
    for(int i=0;i<length-1;i++){
        z[i] = z[i+1];
    }
    z[length-1] = value;

    return z;
}

void fliplr(float *vet, int length){
    int inicio = 0;
    int fim = length - 1;
    
    while(inicio < fim){
        // Troque os elementos nas posições inicio e fim
        float aux = vet[inicio];
        vet[inicio] = vet[fim];
        vet[fim] = aux;
        
        inicio++;
        fim--;
    }
}

float matrix_multiplica(float *v, float *vet, int length){
    float result = 0;
    for (int i=0;i<length;i++){
        result += v[i] * vet[i];
    }
    return result;
}

void costas_loop(float *i, float *q, int length){
    float *r = i, f[]={0, 0.2, 0.3, 1}, a[]={1, 1, 0, 0};
    float h[length]; firpm(h,length,f,a); 
    int fl = 100, que = fl+1;
    float mu = 0.00001, f0 = 320000;

    float *th = zera_buffer(length);    th[0] = rand();
    float *z1 = zera_buffer(que);   float *z2 = zera_buffer(que);
    float *z3 = zera_buffer(que);   float *z4 = zera_buffer(que);

    for(int it=0;it<length;it++){
        float s = 2*r[it];
        z1 = att_z(z1,que,s*cos(2*pi*f0*it+th[it]));
        z2 = att_z(z2,que,s*cos(2*pi*f0*it+pi/4+th[it]));
        z3 = att_z(z3,que,s*cos(2*pi*f0*it+pi/2+th[it]));
        z4 = att_z(z4,que,s*cos(2*pi*f0*it+(3*pi/4)+th[it]));
        fliplr(h,length);
        float lpf1 = matrix_multiplica(h,z1,length);
        float lpf2 = matrix_multiplica(h,z2,length);
        float lpf3 = matrix_multiplica(h,z3,length);
        float lpf4 = matrix_multiplica(h,z4,length);

        th[it+1] = th[it]+mu*lpf1*lpf2*lpf3*lpf4;
    }

    for(int it=0;it<length;it++){
        i[it] = r[it]*sin(2*pi*it+th[it]);
        q[it] = q[it]*sin(2*pi*it+th[it]);
    }

}

void rx_heterodinacao(float *u, float *i, float *q, int length, float fc, float fs, int k, int N){
    float ui[length], uq[length];
    float delta_f = 0;

    for(int it=0;it<length;it++){
        i[it] = u[it] *cos(2*pi*(fc+delta_f)*it/fs);
        q[it] = u[it] *(-sin(2*pi*(fc+delta_f)*it/fs));
    }

    //printf("tam: %d\n",length);
    

    //passagem pelo filtro
    i = rrc(i,k,length,N);
    q = rrc(q,k,length,N);

    //imprime_vetor(q,length+N-1);
    

    //printf("\n\nnumber tam: %d\n\n",length+N-1+N-1);
    //imprime_vetor(i,length+N-1+N-1);
    //plota_constelacao(i,q,length+N-1);
  
    //costas_loop(i,q,length);

}

// funções de demodulação --------------------------------------------
float *downsampler(float *vet_down, float *vet_up, int k , int tam_iq){ 
    vet_down = aloca_memoria_float(tam_iq);

    for(int it=0;it<tam_iq;it++){
        vet_down[it] = vet_up[it*k];
    }
    return vet_down;
}

void demapper(int *x_dmp, float *i, float *q, int tam){
    int j=0;
    for(int it=0;it<tam;it+=4){
        // valores correspondentes a i
        if(i[j]>2.00){
            x_dmp[it] = 1;
            x_dmp[it+1] = 0;
        }else if(i[j]>0 && i[j]<=2.00){
            x_dmp[it] = 1;
            x_dmp[it+1] = 1;
        }else if(i[j]>-2.00 && i[j]<=0.00){
            x_dmp[it] = 0;
            x_dmp[it+1] = 1;
        }else if(i[j]<=-2.00){
            x_dmp[it] = 0;
            x_dmp[it+1] = 0;
        }

        // valores correspondentes a q
        if(q[j]>2.00){
            x_dmp[it+2] = 0;
            x_dmp[it+3] = 0;
        }else if (q[j]>0 && q[j]<=2.00){
            x_dmp[it+2] = 0;
            x_dmp[it+3] = 1;
        }else if (q[j]>-2.00 && q[j]<=0.00){
            x_dmp[it+2] = 1;
            x_dmp[it+3] = 1;
        }else if (q[j]<=-2.00){
            x_dmp[it+2] = 1;
            x_dmp[it+3] = 0;
        }

        j++;
    }
}

void plot_constelacao(float *Ii, float *Q, int tam){
    FILE *file = fopen("database/constelacao.csv", "w");
    
    if (file == NULL)
        printf("Erro ao abrir o arquivo.\n");

    fprintf(file, "I,Q\n");
    for (int i = 0; i < tam; i++) 
        fprintf(file, "%.2f,%.2f\n", Ii[i], Q[i]);
    

    fclose(file);
}

void escreve_vetor_to_CSV(const char *nomeArquivo, int * vetor, int tamanho){
    FILE *arquivo = fopen(nomeArquivo,"w");

    if (arquivo == NULL) 
        printf("Erro ao abrir o arquivo.\n");

    for (int i = 0; i < tamanho; i++) {
        fprintf(arquivo, "%d", vetor[i]);

        // Adiciona uma vírgula após todos os elementos, exceto o último
        if (i < tamanho - 1) 
            fprintf(arquivo, ",");
        
    }
    fclose(arquivo);

}

void contencao(float *i, float *q, int tam){
    for(int it=0;it<tam;it++){
        i[it] /= 5.8962;
        q[it] /= 5.8962; 
    }
}

int main() {
    //x é o vetor de 0 e 1 correspondete ao sinal que será modulado
    int *x, tam;
    FILE *arq;

    //abre o arquivo contendo o sinal a ser modularizado
    arq = open_file_sinal("database/sinal-renan.csv");
    tam = conta_elemento(arq); // conta a quantidadede elementos 
    x = aloca_memoria(x,tam); 
    atribui_vetor(arq,x,tam); // atribui os elementos ao vetor x
    fclose(arq); //fecha o arquivo que não será mais utilizado

    int tam_iq = tam/4;
    float *i = aloca_memoria_float(tam_iq);
    float *q = aloca_memoria_float(tam_iq);

    //-------------------------------------------------------------------------
    mapper_reserva(x,i,q,tam);
    plot_constelacao(i,q,tam_iq); 

    // Upsampler --------------------------------------------------------------
    int fs = 2560000; int symbolRate = 160000;
    int k = fs/symbolRate, tam_up = (k*tam_iq);

    float *i_up = aloca_memoria_float(tam_up);
    float *q_up = aloca_memoria_float(tam_up);

    upsampler(i,i_up,k,tam_iq);
    upsampler(q,q_up,k,tam_iq);

    // Filter modulação -------------------------------------------------------
    float *i_filtred, *q_filtred;
    int N = 97;

    i_filtred = rrc(i_up,k,tam_up,N);
    q_filtred = rrc(q_up,k,tam_up,N);

    // Tx ----------------------------------------------------------------------

    float *u,fc = 320000,fss =2560000;

    u = tx_heterodinacao(u,i_filtred,q_filtred,tam_up+N-1,fc,fss);

    // Rx ----------------------------------------------------------------------  

    rx_heterodinacao(u,i_filtred,q_filtred,tam_up+N-1,fc,fss,k,N);

    // Downsampler -------------------------------------------------------------
    float *i_down, *q_down;

    i_down = downsampler(i_down,&i_filtred[N],k,tam_iq);   
    q_down = downsampler(q_down,&q_filtred[N],k,tam_iq);

    plota_constelacao(i_down,q_down,tam_iq);
    contencao(i_down,q_down,tam_iq);
    //Dúvida para que serve a contenção ? 
    plota_constelacao(i_down,q_down,tam_iq);

    // Demapper ---------------------------------------------------------------
    int *x_dmp = aloca_memoria(x_dmp,tam);

    demapper(x_dmp,i_down,q_down,tam);
    escreve_vetor_to_CSV("database/demapper.csv",x_dmp,tam);
    
    return 0;
}