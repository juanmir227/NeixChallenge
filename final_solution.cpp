#include <iostream>
#include <cmath>



double PI() {
    return M_PI;
}

// Agrego una función para el cálculo de la CDF que voy a necesitar para calcular d1 y d2 de Black-Scholes.
double N(double x) {
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}

// Función para el cálculo de la normal
double F(double x) {
    double factor = 1.0/sqrt(2.0*PI());
    double e = exp(-pow(x, 2)/2.0);
    return factor*e;
}

double D1(double S, double K, double r, double v, double t) {
    double top = log(S / K) + (r + 0.5 * pow(v, 2)) * t;
    double bottom = v * sqrt(t);
    return top / bottom;
}


double D2(double d1, double v, double t) {
    return d1 - v*sqrt(t);
}


// double BlackScholes(double S, double K, double r, double v, double t) {
double BlackScholes(double d1, double d2, double S, double K, double r, double t) {
    // double d1 = D1(S, K, r, v, t);
    // double d2 = D2(d1, v, t);
    double option_price = 0;
    option_price = S*N(d1) - K*exp(-r*t)*N(d2);
    return option_price;
}

double Vega(double S, double d1, double t) {
    return S*F(d1)*sqrt(t);
}


int main() {
    double S = 1210.5;
    double K = 1033;
    double C0 = 301.84900000000005;
    double r = 0.9;
    double t = 0.1157496194824962;
    double v = 0.3; // mi propuesta de vatilidad
    double abstol = 1e-6;
    double epsilon = 1.0;
    int i = 0;
    int max_iter = 1e3;


    while (epsilon > abstol) {
        if (i > max_iter) {
            std::cout << "El programa ha alcanzado el máximo de iteraciones "<< std::endl;
            break;
        }

        i++;
        double orig = v;
        double d1 = D1(S, K, r, v, t);
        double d2 = D2(d1, v, t);
        double function_value = BlackScholes(d1, d2, S, K, r, t) - C0;
        double vega = S * F(d1) * sqrt(t);
        v = -function_value / vega + v;
        epsilon = std::abs(function_value);
    }

    std::cout << "Implied volatility = " << v << std::endl;
    std::cout << "Code required " << i << " iterations." << std::endl;

    return 0;


    
    // double result1 = D1(S, K, r, v, t);
    // double result2 = D2(result1, v, t);
    // double vega = Vega(S,result1,t);
    // double optionprice = BlackScholes(S, K, r,v, t);
}