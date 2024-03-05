#include <iostream>
#include <cmath>



double PI()
{
    return M_PI;
}

// Agrego una función para el cálculo de la CDF que voy a necesitar para calcular d1 y d2 de Black-Scholes.
double N(double x) {
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}


double F(double x) {
    double factor = 1.0/sqrt(2.0*PI());
    double e = exp(-pow(x, 2)/2.0);
    return factor*e;
}

double D1(double S, double K, double r, double q, double v, double t) {
    double top = log(S / K) + (r - q + 0.5 * pow(v, 2)) * t;
    double bottom = v * sqrt(t);
    return top / bottom;
}

double BlackScholes(double S, double K, double r, double q, double v, double t, int optype)
{
    double d1 = D1(S, K, r, q, v, t);
    double d2 = D2(d1, v, t);
    double option_price = 0;
    if(optype == 0){
        option_price = S*exp(-q*t)*N(d1) - K*exp(-r*t)*N(d2);
    } else {
        option_price = K*exp(-r*t)*N(-d2) - S*exp(-q*t)*N(-d1);
    }
    return option_price;
}


double D2(double d1, double v, double t) {
    return d1 - v*sqrt(t);
}

// double Vega(double S, double K, double r, double q, double v, double t)
// {
//     double d1 = D1(S, K, r, q, v, t);
//     return (1.0/100.0)*S*N(d1)*sqrt(t);
// }
// change in the function

double Vega(double S, double d1, double t) {
    return (1.0/100.0)*S*N(d1)*sqrt(t);
}


int main() {
    double S = 297.653;
    double K = 1033;
    double r = 0.9;
    double q = 0.0147;
    // double v = 0.11; debo proveer valor inicial
    double t = 0.11575342465753424;
    double result1 = D1(S, K, r, q, 0.11, t);
    double result2 = D2(result1, 0.11, t);
    
    std::cout << "D1 Result: " << result1 << std::endl;
    std::cout << "D2 Result: " << result2 << std::endl;
    return 0;
}