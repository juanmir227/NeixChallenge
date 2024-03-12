import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy.stats import norm
from scipy.interpolate import interpolate

# Cargo los datos.
data = pd.read_csv('processedData.csv', sep = ';')

# Esta función calcula d1 y d2 de la fórmula de Black-Scholes.
def d(sigma, S, K, r, t):
    d1 = 1 / (sigma * np.sqrt(t)) * ( np.log(S/K) + (r + sigma**2/2) * t)
    d2 = d1 - sigma * np.sqrt(t)
    return d1, d2
# Esta función calcula el precio de la opción con Black-Scholes.
def call_price(sigma, S, K, r, t, d1, d2):
    C = norm.cdf(d1) * S - norm.cdf(d2) * K * np.exp(-r * t)
    return C

# Esta función llama a las funciones anteriormente definidas.
def f(sigma, S, K, r, t, C0):
    d1, d2 = d(sigma, S, K, r, t)
    C = call_price(sigma, S, K, r, t, d1, d2) - C0
    return C

# Se definen constantes utilizadas para el cálculo las cuales fueron tomadas de los datos.
S = 1166.5
K = 1033
t = 0.022515220700152206
r = 0.9
C0 = 145.4505

# Se realiza un gráfico de la fórmula de Black-Scholes en función del valor de la volatilidad Sigma. Y se observa que no existe una raíz para los datos propuestos.
plt.figure(figsize = (16,10))
sigma = np.linspace(0.001, 1,1000)
plt.plot(sigma, f(sigma, S, K, r, t, C0), 'b')
plt.plot([0,1], [0,0], 'k')
plt.title('Modelo no presenta solución', fontsize = 18)
plt.xlabel('Valor de $\sigma$', fontsize = 14)
plt.ylabel('BlackScholes() - data', fontsize =14)
plt.grid(alpha = 0.5)
plt.tight_layout()
# Se guarda el gráfico.
plt.savefig('noSolution.png')