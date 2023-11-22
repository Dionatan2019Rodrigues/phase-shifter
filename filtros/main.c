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

//funções auxiliares -------------------------------------------------
int *aloca_memoria(int *ptr , int tam){
    ptr = (int *) malloc(tam*sizeof(int));
    if(ptr == NULL)
        printf("Erro de alocação:\n");
    return ptr;
}

void plota_constelacao(double *i, double *q, int length){
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

double *aloca_memoria_double(int tam){
    double *ptr = (double *) malloc(tam*sizeof(double));
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

void imprime_vetor(double *vet, int tam){
    for(int it=0;it<tam;it++){
        printf("%.2f\t",vet[it]);
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

void mapper_reserva(int *sequencia, double *I, double *Q, int tam) {
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
            default:
                break;
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

void gera_h_rrc(double *h, int N, int k, double alpha){
    
    for(int i=0;i<N;i++){
        h[i] = (4*alpha)/(pi*k) *  

            ( cos(((i-((N-1)/2))/k)*pi*(1+alpha)) 
                + (pi*(1-alpha)/4*alpha) * sin(((i-((N-1)/2))/k)*pi*(1-alpha))
            
              / 1-(((i-((N-1)/2))/k)*4*alpha)
            );
    }
}

double *zera_buffer(int tam){
    double *ptr = aloca_memoria_double(tam);
    for(int i=0;i<tam;i++){
        ptr[i] = 0.0;
    }
    return ptr;
}

double *rrc(double *vet, int k, int tam_vet){
    int N=6;
    double *h = aloca_memoria_double(N), alpha = 0.5, *buffer = zera_buffer(N);
    gera_h_rrc(h,N,k,alpha);
    //imprime_vetor(h,N);
    int tam_conv = tam_vet + 2*N -1;
    double *result = aloca_memoria_double(tam_conv);

    //convolução
    result = convolucao(vet,h,N,buffer,result,tam_conv,false);

    return result;
}

double *tx_heterodinacao(double *u, double *i, double *q, int length, double fc, double fs){
    double ui[length], uq[length];
    u = aloca_memoria_double(length);
    for(int it=0;it<length;it++){
        ui[it] = i[it] *cos(2*pi*fc*it/fs);
        uq[it] = q[it] *sin(2*pi*fc*it/fs);
        u[it] = ui[it] - uq[it];
    }
    return u;
}

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

void rx_heterodinacao(double *u, double *i, double *q, int length, double fc, double fs, int k){
    double ui[length], uq[length];
    double delta_f = 0;

    plota_constelacao(i,q,length);

    for(int it=0;it<length;it++){
        i[it] = u[it] *cos(2*pi*(fc+delta_f)*it/fs);
        q[it] = u[it] *(-sin(2*pi*(fc+delta_f)*it/fs));
    }

    plota_constelacao(i,q,length);

    //passagem pelo filtro
    i = rrc(i,k,length);
    q = rrc(q,k,length);

    plota_constelacao(i,q,length);

    //costas loop   
    //costas_loop(i,q,length);

}

// funções de demodulação --------------------------------------------
double *downsampler(double *vet_down, double *vet_up, int k , int tam_iq){ 
    vet_down = aloca_memoria_double(tam_iq);

    for(int it=0;it<tam_iq;it++){
        vet_down[it] = vet_up[it*k];
    }
    return vet_down;
}

void demapper(int *x_dmp, double *i, double *q, int tam){
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

void plot_constelacao(double *I, double *Q, int tam){
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

void contencao(double *i, double *q, int tam){
    for(int it=0;it<tam;it++){
        i[it] /= 5.8962;
        q[it] /= 5.8962; 
    }
}

int main() {
    //x é o vetor de 0 e 1 correspondete ao sinal que será modulado
    int tam = 10000;
    int x[tam];
    FILE *arq;

    //abre o arquivo contendo o sinal a ser modularizado
    //arq = open_file_sinal("database/sinal-renan.csv");
    //tam = conta_elemento(arq); // conta a quantidadede elementos 
    //x = aloca_memoria(x,tam); 
    //atribui_vetor(arq,x,tam); // atribui os elementos ao vetor x
    //fclose(arq); //fecha o arquivo que não será mais utilizado
 
    for(int it=0;it<tam;it++)
        x[it] = rand() %2;

    int tam_iq = tam/4;
    double *i = aloca_memoria_double(tam_iq);
    double *q = aloca_memoria_double(tam_iq);

    //-------------------------------------------------------------------------
    mapper_reserva(x,i,q,tam);
    plot_constelacao(i,q,tam_iq); 

    // Upsampler --------------------------------------------------------------
    int fs = 2560000; int symbolRate = 160000;
    int k = fs/symbolRate, tam_up = (k*tam_iq);

    double *i_up = aloca_memoria_double(tam_up);
    double *q_up = aloca_memoria_double(tam_up);

    upsampler(i,i_up,k,tam_iq);
    upsampler(q,q_up,k,tam_iq);

    //plota_constelacao(i_up,q_up,tam_up);

    // Filter modulação -------------------------------------------------------
    double *i_filtred, *q_filtred;

    //i_filtred = filtro_passa_baixa(i_up,tam_up);
    //q_filtred = filtro_passa_baixa(q_up,tam_up);
    //i_filtred = gaussiano(i_up,tam_up);
    //q_filtred = gaussiano(q_up,tam_up);
    i_filtred = rrc(i_up,k,tam_up);
    q_filtred = rrc(q_up,k,tam_up);

    //plota_constelacao(i_filtred,q_filtred,tam_up);
    
    //imprime_vetor(i_filtred,tam_up);
    //printf("saida: \n");
    // Tx ----------------------------------------------------------------------
    double *u,fc = 320000,fss =2560000;

    u = tx_heterodinacao(u,i_filtred,q_filtred,tam_up,fc,fss);

    //plota_constelacao(i,q,tam_iq);
    //plota_constelacao(i_up,q_up,tam_up);

    // Rx ----------------------------------------------------------------------  

    rx_heterodinacao(u,i_filtred,q_filtred,tam_up,fc,fss,k);

    //imprime_vetor(i_filtred,tam_up);

    // Filter demodulação ------------------------------------------------------  
    double *i_demo, *q_demo;
    
    //i_demo = filtro_passa_baixa(i_filtred,tam_up);
    //q_demo = filtro_passa_baixa(q_filtred,tam_up);
    //i_demo = gaussiano(i_filtred,tam_up);
    //q_demo = gaussiano(q_filtred,tam_up);
    i_demo = rrc(i_filtred,k,tam_up);
    q_demo = rrc(q_filtred,k,tam_up);   
     
    // Downsampler -------------------------------------------------------------
    double *i_down, *q_down;

    i_down = downsampler(i_down,&i_demo[k/2],k,tam_iq);   
    q_down = downsampler(q_down,&q_demo[k/2],k,tam_iq);

    contencao(i_down,q_down,tam_iq);

    // Demapper ---------------------------------------------------------------
    int *x_dmp = aloca_memoria(x_dmp,tam);

    demapper(x_dmp,i_down,q_down,tam);
    escreve_vetor_to_CSV("database/demapper.csv",x_dmp,tam);

    //mapper_reserva(x_dmp,i,q,tam);

    //imprime_vetor(i,tam_iq);
    //printf("\n\n");
    //imprime_vetor(q,tam_iq);

    //plota_constelacao(i,q,tam);

    return 0;
}