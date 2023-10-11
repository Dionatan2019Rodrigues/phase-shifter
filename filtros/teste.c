#include <stdio.h>
#include <stdlib.h>
#include <gnuplot_i.h>

int main() {
    // Tamanho da constelação
    int tamanho_constelacao = 64;

    // Vetor de pontos IQ (exemplo)
    double pontos_I[tamanho_constelacao];
    double pontos_Q[tamanho_constelacao];

    // Preencha os vetores de pontos IQ com seus valores reais
    // Neste exemplo, você pode gerar pontos aleatórios para fins de ilustração
    for (int i = 0; i < tamanho_constelacao; i++) {
        pontos_I[i] = rand() / (double)RAND_MAX;  // Valores I aleatórios entre 0 e 1
        pontos_Q[i] = rand() / (double)RAND_MAX;  // Valores Q aleatórios entre 0 e 1
    }

    // Inicialize o gnuplot
    gnuplot_ctrl *h;
    h = gnuplot_init();

    // Configurar opções de plotagem
    gnuplot_setstyle(h, "points");
    gnuplot_cmd(h, "set title 'Constelação IQ'");
    gnuplot_cmd(h, "set xlabel 'I'");
    gnuplot_cmd(h, "set ylabel 'Q'");

    // Plotar a constelação IQ
    gnuplot_plot_xy(h, pontos_I, pontos_Q, tamanho_constelacao, "Constelação");

    // Aguardar entrada do usuário antes de fechar o gráfico
    printf("Pressione Enter para fechar o gráfico...\n");
    getchar();

    // Fechar o gnuplot
    gnuplot_close(h);

    return 0;
}
