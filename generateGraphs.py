import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


# Cargo los archivos a utilizar, los resultados del programa computeVols.cpp y además la tira temporal obtenida al procesar
# los datos.
implied = pd.read_csv('impliedVols.csv')
realized = pd.read_csv('realizedVols.csv')
times = pd.read_csv('processedData.csv', sep = ';')
times['times'] = times.timeToMaturity.apply(lambda x: x*365)
times = times['times'].tolist()

# Dado que para la realizada tengo un punto menos genero dos tiras temporales.
impliedTimes = times
realizedTimes = times[1:]

# Genero una figura que contiene los resultados obtenidos para las volatilidades. Esta contiene tres gráficos.
# Arriba los resultados obtenidos para ambas volatilidades superpuestas. Abajo ambas volatilidades por separado para
# mayor claridad.
fig = plt.figure(figsize = (16,10))
plt.subplot(2, 1, 1)
plt.plot(impliedTimes, implied*100, color = 'blue', label = 'Volatilidad implícita')
plt.plot(realizedTimes, realized*100, color = 'orange', alpha = 0.6, label = 'Volatilidad realizada')
ax = plt.gca()
ax.invert_xaxis()
plt.xlabel('Time To Maturity (Días)')
plt.ylabel('Volatilidad (%)')
plt.legend(fontsize = 13)
plt.grid(alpha = 0.5)
plt.title('Comparación de volatilidad implícita y realizada', fontsize = 20)

plt.subplot(2, 2, 3)
plt.plot(impliedTimes, implied*100, color='blue')
plt.title('Volatilidad implícita')
plt.xlabel('Time To Maturity (Días)')
plt.ylabel('Volatilidad (%)')
plt.grid(alpha = 0.5)
ax = plt.gca()
ax.invert_xaxis()
    
plt.subplot(2, 2, 4)
plt.plot(realizedTimes, realized*100, color='red')
plt.title('Volatilidad realizada')
plt.xlabel('Time To Maturity (Días)')
plt.ylabel('Volatilidad (%)')
plt.grid(alpha = 0.5)
ax = plt.gca()
ax.invert_xaxis()

plt.tight_layout()

# #Guardo la figura en formato pdf para insertar en el README.
plt.savefig('volatilityComparison.png')