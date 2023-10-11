# Definir o terminal de saída (formato da imagem)
set terminal pngcairo enhanced size 800,600
set output 'resultado.png'

# Definir o título do gráfico e os rótulos dos eixos
set title 'Constelação 16-QAM'
set xlabel 'I'
set ylabel 'Q'

# Configurar estilo de pontos e tamanho
set style data points
set pointtype 7
set pointsize 2

# Plotar os dados do arquivo CSV
splot 'database/constelacao.csv' using 1:2 with points title 'Constelação 16-QAM'
