#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

double PI() {
    return M_PI;
}

double N(double x) {
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}

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

double BlackScholes(double d1, double d2, double S, double K, double r, double t) {
    double option_price = 0;
    option_price = S*N(d1) - K*exp(-r*t)*N(d2);
    return option_price;
}

double Vega(double S, double d1, double t) {
    return S*F(d1)*sqrt(t);
}

double ImpliedVolatility(double epsilon, double abstol, double i, double max_iter, double S,double K,double r,double v,double t,double C0){

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
        double Vega = S * F(d1) * sqrt(t);
        v = -function_value / Vega + v;
        epsilon = std::abs(function_value);
    }


    std::cout << "Code required " << i << " iterations." << std::endl;
    return v;
}