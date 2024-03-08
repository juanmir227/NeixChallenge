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
        double d1 = D1(S, K, r, v, t);
        double d2 = D2(d1, v, t);
        double estimation = BlackScholes(d1, d2, S, K, r, t);
        double function_value = estimation - C0;
        double Vega = S * F(d1) * sqrt(t);
        // std::cout << "F(d1):" << F(d1) << std::endl;
        // std::cout << "vega:" << Vega << std::endl;
        v = -function_value / Vega + v;
        epsilon = std::abs(function_value);
        // std::cout << "Epsilon value:" << epsilon << std::endl;
        // std::cout << "implied volatility estimation" << v <<std::endl;
    }


    // std::cout << "Code required " << i << " iterations." << std::endl;
    return v;
}


int main() {
    // Defino variables constantes a utilizar y también agrego mi v inicial para comenzar el loop.
    //  La volatilidad inicial utilizada para el siguiente loop será la obtenida anteriormente.
    const double K = 1033;
    const double r = 0.9;
    const double abstol = 1e-4;
    const double epsilon = 1.0;
    double v = 0.5;
    const int max_iter = 1e3;
    std::vector<double> impliedVol;

    // double S = 1209.5;
    // double K = 1033;
    // double C0 = 297.653;
    // double r = 0.9;
    // double t = 0.11575342465753424;
    // double v = 0.5; // mi propuesta de vatilidad
    // double abstol = 1e-4;
    // double epsilon = 1.0;
    // int i = 0;
    // int max_iter = 1e3;
    // double d1 = D1(S, K, r, v, t);
    // double d2 = D2(d1, v, t);
    // double bs = BlackScholes(d1, d2, S, K, r, t);
    // std::cout << "d1 value = " << d1 << std::endl;
    // std::cout << "d2 value = " << d2 << std::endl;
    // std::cout << "Black Scholes value" << bs << std::endl;
    // v = ImpliedVolatility(epsilon,abstol, i, max_iter, S, K, r, v, t, C0);
    // std::cout << "Implied volatility = " << v << std::endl;

    // Example usage
    CSVData data = readCSV("processed_data.csv");

    size_t dataSize = data.price.size();
    // checkeo que tengan el mismo largo los vectores
    if (data.underPrice.size() != dataSize || data.timeToMaturity.size() != dataSize) {
        std::cerr << "Error: Vectors must have the same size." << std::endl;
        return 1;
    }

    for (size_t i = 0; i < dataSize; ++i) {
    // Access elements using the common index
        double currentPrice = data.price[i]; //C0
        double currentUnderPrice = data.underPrice[i]; //S
        double currentTimeToMaturity = data.timeToMaturity[i]; //t
        if (i == 0){
            std::cout << "price:" << currentPrice << std::endl;
            std::cout << "underprice:" << currentUnderPrice << std::endl;
            std::cout << "ttm:" << currentTimeToMaturity << std::endl;
        }
        int j = 0;
        double vol = ImpliedVolatility(epsilon, abstol, j, max_iter, currentUnderPrice, K, r, v, currentTimeToMaturity,currentPrice);




        impliedVol.push_back(vol);
    }

    std::cout << "Implied Volatility:" << std::endl;
    for (const auto& val : impliedVol) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;

}













    // Print the vectors for verification
    // std::cout << "Price Vector:" << std::endl;
    // for (const auto& val : data.price) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;

    // std::cout << "UnderPrice Vector:" << std::endl;
    // for (const auto& val : data.underPrice) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;

    // std::cout << "TimeToMaturity Vector:" << std::endl;
    // for (const auto& val : data.timeToMaturity) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;
