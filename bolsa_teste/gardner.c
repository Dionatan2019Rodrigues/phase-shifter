#include <stdio.h>
#include <math.h>

#define SAMPLES_PER_SYMBOL 2   // Número de amostras por símbolo
#define SYMBOL_PERIOD 10       // Período do símbolo em amostras
#define NUM_SAMPLES 100        // Número total de amostras

int main() {
    float samples[NUM_SAMPLES];  // Array para armazenar as amostras do sinal recebido

    // Simulando amostras do sinal recebido (esse trecho pode ser substituído pelos dados reais do sinal)
    for (int i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = sin(2 * M_PI * i / SYMBOL_PERIOD) + 0.1 * sin(4 * M_PI * i / SYMBOL_PERIOD);
    }

    float accum = 0.0;  // Variável para acumular as diferenças entre as amostras consecutivas
    int minIndex = -1;  // Índice onde o mínimo local é detectado

    // Implementação do algoritmo de Gardner
    for (int i = 0; i < NUM_SAMPLES - SAMPLES_PER_SYMBOL; i++) {
        float product = samples[i] * samples[i + SYMBOL_PERIOD];  // Multiplicação das amostras
        accum += product;

        // Verificação do mínimo local
        if (i % SYMBOL_PERIOD == 0) {
            if (minIndex == -1 || accum < accumSamples[minIndex]) {
                minIndex = i;
            }
        }
    }

    // Ajuste de temporização com base no mínimo local detectado
    int timingAdjustment = minIndex - (SYMBOL_PERIOD * SAMPLES_PER_SYMBOL / 2);
    printf("Timing adjustment: %d samples\n", timingAdjustment);

    return 0;
}
