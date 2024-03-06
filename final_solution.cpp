#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct CSVData {
    std::vector<double> price;
    std::vector<double> underPrice;
    std::vector<double> timeToMaturity;
};

CSVData readCSV(const std::string& filename) {
    CSVData data;

    // Open the CSV file
    std::ifstream file(filename);

    // Check if the file is open
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return data; // Empty data if file cannot be opened
    }

    std::string line;
    // Read the header line (assuming the first line is a header)
    std::getline(file, line);

    // Read the data lines
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        // Read each column using stringstream
        std::getline(ss, token, ';');
        data.price.push_back(std::stod(token));

        std::getline(ss, token, ';');
        data.underPrice.push_back(std::stod(token));

        std::getline(ss, token, ';');
        data.timeToMaturity.push_back(std::stod(token));
    }

    // Close the file
    file.close();

    return data;
}

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

int main() {

    // std::ifstream file("your_file.csv");
    // if (!file.is_open()) {
    //     std::cerr << "Error opening file." << std::endl;
    //     return 1;
    // }

    // std::vector<double> price;
    // std::vector<double> underPrice;
    // std::vector<double> timeToMaturity;



    // double S = 1210.5;
    // double K = 1033;
    // double C0 = 301.84900000000005;
    // double r = 0.9;
    // double t = 0.1157496194824962;
    // double v = 0.5; // mi propuesta de vatilidad
    // double abstol = 1e-6;
    // double epsilon = 1.0;
    // int i = 0;
    // int max_iter = 1e3;
    // v = ImpliedVolatility(epsilon,abstol, i, max_iter, S, K, r, v, t, C0);
    // std::cout << "Implied volatility = " << v << std::endl;

    // Example usage
    CSVData data = readCSV("processed_data.csv");

    // Print the vectors for verification
    std::cout << "Price Vector:" << std::endl;
    for (const auto& val : data.price) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "UnderPrice Vector:" << std::endl;
    for (const auto& val : data.underPrice) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    std::cout << "TimeToMaturity Vector:" << std::endl;
    for (const auto& val : data.timeToMaturity) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;

}