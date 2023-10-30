#include <stdio.h>

int main() {
    // Valores I e Q da constelação 16-QAM
    double I[] = {1.0, -1.0, 3.0, -3.0, 1.0, -1.0, 3.0, -3.0, 1.0, -1.0, 3.0, -3.0, 1.0, -1.0, 3.0, -3.0};
    double Q[] = {1.0, 1.0, 1.0, 1.0, 3.0, 3.0, 3.0, 3.0, -1.0, -1.0, -1.0, -1.0, -3.0, -3.0, -3.0, -3.0};

    // Número de símbolos na constelação 16-QAM
    int numSimbolos = sizeof(I) / sizeof(I[0]);

    // Inicialize o arquivo de saída para o GNUplot
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");

    if (gnuplotPipe == NULL) {
        printf("Erro ao iniciar o GNUplot.\n");
        return 1;
    }

    // Configurar o gráfico
    fprintf(gnuplotPipe, "set title 'Constelação 16-QAM (I vs. Q)'\n");
    fprintf(gnuplotPipe, "set xlabel 'I'\n");
    fprintf(gnuplotPipe, "set ylabel 'Q'\n");

    // Plotar os símbolos da constelação
    fprintf(gnuplotPipe, "plot '-' using 1:2 title '16-QAM' with points pointtype 7\n");

    for (int i = 0; i < numSimbolos; i++) {
        fprintf(gnuplotPipe, "%lf %lf\n", I[i], Q[i]);
    }

    fprintf(gnuplotPipe, "e\n");
    fclose(gnuplotPipe);

    return 0;
}
