import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy.stats import norm
from scipy.interpolate import interpolate
data = pd.read_csv('processedData.csv', sep = ';')


def d(sigma, S, K, r, t):
    d1 = 1 / (sigma * np.sqrt(t)) * ( np.log(S/K) + (r + sigma**2/2) * t)
    d2 = d1 - sigma * np.sqrt(t)
    return d1, d2

def call_price(sigma, S, K, r, t, d1, d2):
    C = norm.cdf(d1) * S - norm.cdf(d2) * K * np.exp(-r * t)
    return C


def f(sigma, S, K, r, t, C0):
    d1, d2 = d(sigma, S, K, r, t)
    C = call_price(sigma, S, K, r, t, d1, d2) - C0
    return C

S = 1166.5
K = 1033
t = 0.022515220700152206
r = 0.9
C0 = 145.4505
plt.figure(figsize = (16,10))
sigma = np.linspace(0.001, 1,1000)
plt.plot(sigma, f(sigma, S, K, r, t, C0), 'b')
plt.plot([0,1], [0,0], 'k')
plt.title('Modelo no presenta solución', fontsize = 18)
plt.xlabel('Valor de $\sigma$', fontsize = 14)
plt.ylabel('BlackScholes() - data', fontsize =14)
plt.grid(alpha = 0.5)
plt.tight_layout()
plt.savefig('graphs/noSolution.png')