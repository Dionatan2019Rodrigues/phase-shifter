/* Desenvolvido por Dionatan Rodrigues - derodrigues@inf.ufsm.br*/
#include <stdio.h>
#include <stdlib.h> // necessário para usar função malloc() e rand()
#include <string.h> // necessário para usar função strtok()
#include <math.h>   // necessário para função gera_h_rrc()
#include <stdbool.h> // necessário para parâmetro same da função convolucao()

#define pi 3.14159265358979323846

#define MAX_TAM_LINE_CSV 200000 // tamanho definido pelo programador
// escolhido para que todo o arquivo seja considerado na mesma linha 
// e assim separado corretamente apenas pelo delimitador ','

// Struct utilizada apenas para retorno de dois vetores na função 
// de heterodinação do RX 
typedef struct I_and_Q{
    double *i;
    double *q;
} I_and_Q;

//funções auxiliares -------------------------------------------------

int *aloca_memoria_int(int *ptr , int tam){
    ptr = (int *) malloc(tam*sizeof(int));
    if(ptr == NULL)
        printf("Erro de alocação:\n");
    return ptr;
}

I_and_Q *aloca_memoria_IandQ(I_and_Q *ptr , int tam){
    ptr = (I_and_Q *) malloc(tam*sizeof(I_and_Q));
    if(ptr == NULL)
        printf("Erro de alocação:\n");
    return ptr;
}

double *aloca_memoria_double(int tam){
    double *ptr = (double *) malloc(tam*sizeof(double));
    if(ptr == NULL)
        printf("Erro de alocação:\n");
    return ptr;
}

void plota_constelacao(double *i, double *q, int length, char *palavra){
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");

    if (gnuplotPipe == NULL)
        printf("Erro ao iniciar o GNUplot.\n");

    // Configurar o gráfico
    fprintf(gnuplotPipe, "set title 'Constelação (I vs. Q)'\n");
    fprintf(gnuplotPipe, "set xlabel 'I'\n");
    fprintf(gnuplotPipe, "set ylabel 'Q'\n");

    // Plotar os símbolos da constelação
    fprintf(gnuplotPipe, "plot '-' using 1:2 title '%s' with points pointtype 7\n",palavra);

    for (int it=0;it<length;it++) {
        fprintf(gnuplotPipe, "%lf %lf\n", i[it], q[it]);
    }

    fprintf(gnuplotPipe, "e\n");
    fclose(gnuplotPipe);

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

void imprime_vetor_to_double(double *vet, int tam){
    for(int it=0;it<tam;it++){
        printf("%.15lf,\t",vet[it]);
    }
    printf("\n");
}

void imprime_vetor_to_int(int *vet, int tam){
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

void show_constelacao_csv(double *I, double *Q, int tam){
    FILE *file = fopen("database/constelacao.csv", "w");
    
    if (file == NULL)
        printf("Erro ao abrir o arquivo.\n");

    fprintf(file, "I,Q\n");
    for (int i = 0; i < tam; i++) 
        fprintf(file, "%.2f,%.2f\n", I[i], Q[i]);
    

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

void mapper_16_qam(int *sequencia, double *I, double *Q, int tam) {
    int j = 0, sequencia_dec[tam/4];
    for (int i = 0; i < tam / 4; i++) {
        sequencia_dec[i] = sequencia[j] * 8 + sequencia[j + 1] * 4 + sequencia[j + 2] * 2 + sequencia[j + 3];
        j = j + 4;
    }

    for (int i = 0; i < tam / 4; i++) {
        switch (sequencia_dec[i]) {
            case 0:
                I[i] = -3;
                Q[i] = 3;
                break;
            case 1:
                I[i] = -3;
                Q[i] = 1;
                break;
            case 2:
                I[i] = -3;
                Q[i] = -3;
                break;
            case 3:
                I[i] = -3;
                Q[i] = -1;
                break;
            case 4:
                I[i] = -1;
                Q[i] = 3;
                break;
            case 5:
                I[i] = -1;
                Q[i] = 1;
                break;
            case 6:
                I[i] = -1;
                Q[i] = -3;
                break;
            case 7:
                I[i] = -1;
                Q[i] = -1;
                break;
            case 8:
                I[i] = 3;
                Q[i] = 3;
                break;
            case 9:
                I[i] = 3;
                Q[i] = 1;
                break;
            case 10:
                I[i] = 3;
                Q[i] = -3;
                break;
            case 11:
                I[i] = 3;
                Q[i] = -1;
                break;
            case 12:
                I[i] = 1;
                Q[i] = 3;
                break;
            case 13:
                I[i] = 1;
                Q[i] = 1;
                break;
            case 14:
                I[i] = 1;
                Q[i] = -3;
                break;
            case 15:
                I[i] = 1;
                Q[i] = -1;
                break;
            default: break;
        }
    }
}

void mapper_qpsk(int *sequencia, double *I, double *Q, int tam){
    int j= 0, sequencia_dec[tam/2];
    for(int it=0;it<tam/2;it++){
        sequencia_dec[it] = sequencia[j]*2 + sequencia[j+1];
        j += 2;
    }

    for(int it=0;it<tam/2;it++){
        switch(sequencia_dec[it]){
            case 0:
                I[it] = 1;
                Q[it] = 1;
                break;
            case 1:
                I[it] = -1;
                Q[it] = 1;
                break;
            case 2:
                I[it] = 1;
                Q[it] = -1;
                break;
            case 3:
                I[it] = -1;
                Q[it] = -1;
                break; 
            default: break;   
        }
    }
    
}

void upsampler(double*vet, double *vet_up, int k, int tam_iq){
    
    for(int it=0;it<tam_iq;it++){
        vet_up[it * k] = vet[it];
        for(int itera=1;itera<k;itera++){
            vet_up[itera] = 0;
        }
        //printf("%d\t",it);
    }

}

// funções de filtragem 
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
            //printf("%.4lf\t%.4lf\t%.4lf\n",h[j]*buffer[j],h[j],buffer[j]);
            soma += h[j]*buffer[j];
        } 
        //printf("\npula fora soma:\t\t\t --------------------------\n");
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
    
    //imprime_vetor(result,tam_conv-h_len+1);
    return result;
}

double *filtro_passa_baixa(double *vet, int tam_vet_up){
    double h[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double buffer[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    int tam_h = 16, tam_conv = tam_vet_up + tam_h -1;
    double *result;
    //convolução
    result = convolucao(vet,h,tam_h,buffer,result,tam_conv,true);
    return result;
}

double *gaussiano(double *vet, int tam_vet){
    double h[] = {0.0030, 0.0118, 0.0390, 0.1051, 0.2367, 0.4445, 0.6972, 0.9138,
                   1.0000, 0.9138, 0.6972, 0.4445, 0.2367, 0.1051, 0.0390, 0.0118};
    double buffer[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    int tam_h = 16, tam_conv = tam_vet + 2*16 -1;
    double *result = aloca_memoria_double(tam_conv);

    //convolução 
    result = convolucao(vet,h,tam_h,buffer,result,tam_conv,false); 
    //imprime_vetor(result,tam_conv);
    return result;  
    
}

double *gera_h_rrc(double *h, int N, int k, double alpha){
    
    double result[97] = {
          0.000758020010898,	-0.000574180613695,	-0.001911446068766,	-0.003132122856295,
         -0.004113762976042,    -0.004743493616427, -0.004928553347545, -0.004606178216716, 
         -0.003752007629866,    -0.002386227566719, -0.000576777741495,  0.001560884487000,
          0.003867755994290,     0.006149037610289,  0.008185433303110,  0.009747498219627,
          0.010612280152568,     0.010581280537572,	 0.009498596314057,  0.007268007346790,
          0.003867755994290,    -0.000638168720174, -0.006093280097580, -0.012247185791501,
         -0.018760038149330,    -0.025213013194532, -0.031124613750254, -0.035972210785445,
         -0.039217873704933,    -0.040337219403073, -0.038849753959261, -0.0343490089179343,
         -0.026530700381420,    -0.015217171954757, -0.000376523594801,  0.017864927605222,
          0.039217873704933,     0.063233540462927,  0.089315663836603,  0.116740591889277,
          0.144684711494492,     0.172257960486318,  0.198541809966356,  0.222629812373093,  
          0.243668627640275,     0.260897374996557,  0.273683217172174,  0.281551264846161,
          0.284207182407807,     0.281551264846161,  0.273683217172174,  0.260897374996557, 
          0.243668627640275,     0.222629812373093,  0.198541809966356,  0.172257960486318,
          0.144684711494492,     0.116740591889277,  0.089315663836603,  0.063233540462927,
          0.039217873704933,     0.017864927605222, -0.000376523594801, -0.015217171954757,
         -0.026530700381420,    -0.034349008917934, -0.038849753959261, -0.040337219403073,   
         -0.039217873704933,    -0.035972210785445, -0.031124613750254, -0.025213013194532,
         -0.018760038149330,    -0.012247185791501, -0.006093280097580, -0.000638168720174,   
          0.003867755994290,     0.007268007346790,  0.009498596314057,  0.010581280537572,
          0.010612280152568,     0.009747498219627,  0.008185433303110,  0.006149037610289, 
          0.003867755994290,     0.001560884487000, -0.000576777741495, -0.002386227566719,
         -0.003752007629866,    -0.004606178216716, -0.004928553347545, -0.004743493616427,
         -0.004113762976042,    -0.003132122856295, -0.001911446068766, -0.000574180613695,
          0.000758020010898
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

double *zera_buffer(int tam){
    double *ptr = aloca_memoria_double(tam);
    for(int i=0;i<tam;i++){
        ptr[i] = 0.0;
    }
    return ptr;
}

double *rrc(double *vet, int k, int tam_vet, int N){
    double *h = aloca_memoria_double(N), alpha = 0.5, *buffer = zera_buffer(N);
    h = gera_h_rrc(h,N,k,alpha);

    int tam_conv = tam_vet + N -1;
    double *result = aloca_memoria_double(tam_conv);

    //convolução
    result = convolucao(vet,h,N,buffer,result,tam_conv,false);

    //imprime_vetor_to_double(result,tam_conv);printf("\n\nresult acima \n\n");

    return result;
}

// heterodinação TX ----------------------------------------------------
double *tx_heterodinacao(double *u, double *i, double *q, int length, double fc, double fs){
    double ui[length], uq[length];
    u = aloca_memoria_double(length);
    for(int it=0;it<length;it++){
        ui[it] = i[it] *cos(2*pi*fc*it/fs);
        uq[it] = q[it] *sin(2*pi*fc*it/fs);
        u[it] = ui[it] - uq[it];
    }
    //imprime_vetor_to_double(ui,length);
    //printf("\n\nq abaixo\n\n");
    //imprime_vetor_to_double(uq,length);
    return u;
}

// funções do costas loop ----------------------------------------------

void firpm(double *h, int filterLength, double *f, double *a) {
    //double sum;

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

double *att_z(double *z, int length, double value){
    for(int i=0;i<length-1;i++){
        z[i] = z[i+1];
    }
    z[length-1] = value;

    return z;
}

void fliplr(double *vet, int length){
    int inicio = 0;
    int fim = length - 1;
    
    while(inicio < fim){
        // Troque os elementos nas posições inicio e fim
        double aux = vet[inicio];
        vet[inicio] = vet[fim];
        vet[fim] = aux;
        
        inicio++;
        fim--;
    }
}

double matrix_multiplica(double *v, double *vet, int length){
    double result = 0;
    for (int i=0;i<length;i++){
        result += v[i] * vet[i];
    }
    return result;
}

void costas_loop(double *i, double *q, int length){
    double *r = i, f[]={0, 0.2, 0.3, 1}, a[]={1, 1, 0, 0};
    double h[length]; firpm(h,length,f,a); 
    int fl = 100, que = fl+1;
    double mu = 0.00001, f0 = 320000;

    double *th = zera_buffer(length);    th[0] = rand();
    double *z1 = zera_buffer(que);   double *z2 = zera_buffer(que);
    double *z3 = zera_buffer(que);   double *z4 = zera_buffer(que);

    for(int it=0;it<length;it++){
        double s = 2*r[it];
        z1 = att_z(z1,que,s*cos(2*pi*f0*it+th[it]));
        z2 = att_z(z2,que,s*cos(2*pi*f0*it+pi/4+th[it]));
        z3 = att_z(z3,que,s*cos(2*pi*f0*it+pi/2+th[it]));
        z4 = att_z(z4,que,s*cos(2*pi*f0*it+(3*pi/4)+th[it]));
        fliplr(h,length);
        double lpf1 = matrix_multiplica(h,z1,length);
        double lpf2 = matrix_multiplica(h,z2,length);
        double lpf3 = matrix_multiplica(h,z3,length);
        double lpf4 = matrix_multiplica(h,z4,length);

        th[it+1] = th[it]+mu*lpf1*lpf2*lpf3*lpf4;
    }

    for(int it=0;it<length;it++){
        i[it] = r[it]*sin(2*pi*it+th[it]);
        q[it] = q[it]*sin(2*pi*it+th[it]);
    }

}

// heterodinação RX -----------------------------------------------------

void rx_heterodinacao(double *u, double *i, double *q, int length,
                        double fc, double fs, int k, int N, I_and_Q *ptr){
    double ui[length], uq[length];
    double delta_f = 100;

    for(int it=0;it<length;it++){
        i[it] = u[it] *cos(2*pi*(fc+delta_f)*it/fs);
        q[it] = u[it] *(-sin(2*pi*(fc+delta_f)*it/fs));
    }
    
    //passagem pelo filtro
    i = rrc(i,k,length,N);
    q = rrc(q,k,length,N);
    ptr->i  = i;
    ptr->q  = q;

    plota_constelacao(i,q,(length+N-1)/2,"16-QAM");
  
    //costas_loop(i,q,length);

}

// funções de demodulação -----------------------------------------------
double *downsampler(double *vet_down, double *vet_up, int k , int tam_iq){ 
    vet_down = aloca_memoria_double(tam_iq);

    for(int it=0;it<tam_iq;it++){
        vet_down[it] = vet_up[it*k];
    }
    return vet_down;
}

void contencao(double *i, double *q, int tam){
    for(int it=0;it<tam;it++){
        i[it] *= 2;//5.8962*2;
        q[it] *= 2;//5.8962*2; 
    }
}

void demapper_16_qam(int *x_dmp, double *i, double *q, int tam){
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

void demapper_qpsk(int *x_dmp, double *i, double *q, int tam){
    int j = 0;
    for(int it=0;it<tam;it+=2){
        // valores correspondentes a q
        if(q[j]>0.0)
            x_dmp[it] = 0;
        else
            x_dmp[it] = 1;
        
        // valores correspondentes a i
        if(i[j]>0.0)
            x_dmp[it+1] = 0;
        else
            x_dmp[it+1] = 1;
        
        j++;  
    }
}

int main() {
    //x é o vetor de 0 e 1 correspondete ao sinal que será modulado
    int *x, tam;
    FILE *arq;

    //abre o arquivo contendo o sinal a ser modularizado
    arq = open_file_sinal("database/sinal.csv");
    tam = conta_elemento(arq); // conta a quantidadede elementos 
    x = aloca_memoria_int(x,tam); 
    atribui_vetor(arq,x,tam); // atribui os elementos ao vetor x
    fclose(arq); //fecha o arquivo que não será mais utilizado

    int tam_iq = tam/4; // [tam/4 == 16 QAM]  [tam/2 == QPSK]
    double *i = aloca_memoria_double(tam_iq);
    double *q = aloca_memoria_double(tam_iq);

    //-------------------------------------------------------------------------
    mapper_16_qam(x,i,q,tam); // escolha seu mapper (QAM || QPSK)
    show_constelacao_csv(i,q,tam_iq); // printa a constelação em (database/constelacao.csv)

    // Upsampler --------------------------------------------------------------
    int fs = 2560000; int symbolRate = 160000;
    int k = fs/symbolRate, tam_up = (k*tam_iq);
    
    double *i_up = aloca_memoria_double(tam_up);
    double *q_up = aloca_memoria_double(tam_up);

    upsampler(i,i_up,k,tam_iq);
    upsampler(q,q_up,k,tam_iq);

    // Filter modulação -------------------------------------------------------
    double *i_filtred, *q_filtred;
    int N = 97;

    i_filtred = rrc(i_up,k,tam_up,N);
    q_filtred = rrc(q_up,k,tam_up,N);

    // Tx ----------------------------------------------------------------------

    double *u,fc = 320000,fss =2560000;

    u = tx_heterodinacao(u,i_filtred,q_filtred,tam_up+N-1,fc,fss);

    //imprime_vetor_to_double(u,tam_up+N-1);

    // Rx ----------------------------------------------------------------------  
    I_and_Q *ptr = aloca_memoria_IandQ(ptr,1);
    rx_heterodinacao(u,i_filtred,q_filtred,tam_up+N-1,fc,fss,k,N,ptr);

    i_filtred = ptr->i;
    q_filtred = ptr->q;
    /*
    printf("\nposition: %d\n\n",((tam_up+(2*N)-2)/2)-10);
    imprime_vetor_to_double(&i_filtred[((tam_up+(2*N)-2)/2)-10],(tam_up+(2*N)-2)/2);

    printf("\n\ni filtrado completo , length: %d\n\n",tam_up+(2*N)-2);
    imprime_vetor_to_double(i_filtred,tam_up+(2*N)-2);
    */
    plota_constelacao(i_filtred,q_filtred,(tam_up+(2*N)-2),"16-QAM");
    //plota_constelacao(i_filtred,q_filtred,(tam_up+(2*N)-2)/2,"16-QAM");
    /*plota_constelacao(&i_filtred[((tam_up+(2*N)-2)/2)+1],&q_filtred[((tam_up+(2*N)-2)/2)+1],
                            (tam_up+(2*N)-2),"16-QAM");*/
    //printf("\nsaida do rrc i:%d\n",tam_up+(2*N)-2);
    //imprime_vetor_to_double(i_filtred,tam_up+(2*N)-2);

    // Downsampler -------------------------------------------------------------
    double *i_down, *q_down;

    i_down = downsampler(i_down,&i_filtred[N-1],k,tam_iq);   
    q_down = downsampler(q_down,&q_filtred[N-1],k,tam_iq);

    //plota_constelacao(i_down,q_down,tam_iq,"QPSK");
    contencao(i_down,q_down,tam_iq);
    //plota_constelacao(i_down,q_down,tam_iq,"16-QAM");

    // Demapper ---------------------------------------------------------------
    int *x_dmp = aloca_memoria_int(x_dmp,tam);

    demapper_16_qam(x_dmp,i_down,q_down,tam);
    escreve_vetor_to_CSV("database/demapper.csv",x_dmp,tam);
    
    return 0;
}