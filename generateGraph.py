import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates

# Se cargan los archivos a utilizar: los resultados del programa computeVols.cpp y las fechas.
implied = pd.read_csv('impliedVols.csv')
realized = pd.read_csv('realizedVols.csv')
dates = pd.read_csv('dates.csv')
dates.created_at = pd.to_datetime(dates.created_at)

# Dado que para la volatilidad realizada se obtuvo un punto menos, se generan dos tiras temporales para mayor claridad.
impliedDates = dates
realizedDates = dates[1:]

# Se genera una figura que contiene los resultados obtenidos para las volatilidades. Esta contiene tres gráficos. Arriba los resultados obtenidos para ambas volatilidades superpuestas. Abajo ambas volatilidades por separado para mayor claridad.
fig = plt.figure(figsize = (16,10))
plt.subplot(2, 1, 1)
plt.plot(impliedDates, implied*100, color = 'blue', label = 'Volatilidad implícita')
plt.plot(realizedDates, realized*100, color = 'orange', alpha = 0.6, label = 'Volatilidad realizada')
plt.xlabel('Fecha', fontsize = 14)
plt.ylabel('Volatilidad (%)', fontsize = 14)
plt.legend(fontsize = 13)
plt.grid(alpha = 0.5)
plt.title('Comparación de volatilidad implícita y realizada', fontsize = 20)

plt.subplot(2, 2, 3)
plt.plot(impliedDates, implied*100, color='blue')
plt.title('Volatilidad implícita')
plt.xlabel('Fecha')
plt.ylabel('Volatilidad (%)')
plt.grid(alpha = 0.5)
    
plt.subplot(2, 2, 4)
plt.plot(realizedDates, realized*100, color='red')
plt.title('Volatilidad realizada')
plt.xlabel('Fecha')
plt.ylabel('Volatilidad (%)')
plt.grid(alpha = 0.5)

plt.tight_layout()

# #Guardo la figura en formato png para insertar en el README.
plt.savefig('volatilityComparison.png')
