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

struct Constantes {
    const double K = 1033;
    const double r = 0.9;
    const double abstol = 1e-4;
    const double epsilon = 1.0;
    const int max_iter = 1e3;
};

CSVData readCSV(const std::string& filename) {
    CSVData data;

    try {
        // Open the CSV file
        std::ifstream file(filename);

        // Check if the file is open
        if (!file.is_open()) {
            throw std::ios_base::failure("Error opening file.");
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
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        // Clear data if an exception occurs
        data = CSVData();
    }

    return data;
}

void saveToCSV(const std::vector<double>& vec, const std::string& filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    // Write header
    file << "Volatility" << std::endl;

    // Write data
    for (size_t i = 0; i < vec.size(); ++i) {
        file << vec[i] << std::endl;
    }

    file.close();
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

// Explicar que decidi separar las funciones para mayor claridad aunque pago con mas lineas de codigo.
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

double ImpliedVolatility(double epsilon, double abstol, double max_iter, double S,double K,double r,double v,double t,double C0){
    int i = 0;
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
        v = -function_value / Vega + v;
        epsilon = std::abs(function_value);
    }
    return v;
}

// Function to calculate realized volatility based on adjacent points
double RealizedVolatility(double underPrice,double timeToMaturity, double initUnderPrice, double initTimeToMaturity) {
    // Function to calculate logarithmic returns
    double logReturn = std::log(underPrice / initUnderPrice );
    double timeDifference = std::abs(timeToMaturity - initTimeToMaturity);
    double realizedVolatility = 0.0;
    // Ensure timeDifference is not zero to avoid division by zero
    if (timeDifference > 1e-6) {
        double weightedReturn = pow(logReturn,2) / timeDifference;
        realizedVolatility = std::sqrt((weightedReturn)/2);
    }
    return realizedVolatility;
}

// Decidi definir dos variables para guardarme los puntos anteriores en vez de un vector para no tener que cargarlo todas
// las veces.
int main() {
    // Defino variables constantes a utilizar y también agrego mi v inicial para comenzar el loop.
    //  La volatilidad inicial utilizada para el siguiente loop será la obtenida anteriormente.
    double v = 0.5;
    double initUnderPrice = 0.0;
    double initTimeToMaturity = 0.0;
    std::vector<double> impliedVols;
    std::vector<double> realizedVols;
    bool flag1 = false;
    bool flag2 = false;
    Constantes constantes;
    // Example usage
    CSVData data = readCSV("processedData.csv");
    // const double K = 1033;
    // const double r = 0.9;
    // const double abstol = 1e-4;
    // const double epsilon = 1.0;
    // const int max_iter = 1e3;

    size_t dataSize = data.price.size();
    // checkeo que tengan el mismo largo los vectores

    if ((data.underPrice.size() != dataSize || data.timeToMaturity.size() != dataSize) && (!flag1)) {
        std::cout << "Cuidado, los vectores cargados del csv no tienen el mismo largo." << std::endl;
        flag1 = true;
    }

    for (size_t i = 0; i < dataSize; ++i) {
    // Access elements using the common index
        double currentPrice = data.price[i]; //C0
        double currentUnderPrice = data.underPrice[i]; //S
        double currentTimeToMaturity = data.timeToMaturity[i]; //t
        // int j = 0;
        double impliedVol = ImpliedVolatility(constantes.epsilon, constantes.abstol, constantes.max_iter, currentUnderPrice, constantes.K, constantes.r, v, currentTimeToMaturity,currentPrice);
        if (i == 0){
            initUnderPrice = currentUnderPrice;
            initTimeToMaturity = currentTimeToMaturity;
        }
        else {
            double realizedVol= RealizedVolatility(currentUnderPrice, currentTimeToMaturity, initUnderPrice, initTimeToMaturity);
            realizedVols.push_back(realizedVol);
            initUnderPrice = currentUnderPrice;
            initTimeToMaturity = currentTimeToMaturity;
        }

        if ((std::isnan(impliedVol) || (impliedVol < 0.0)) && (!flag2)) {
            std::cout << "Los resultados de volatilidad implicita contienen valores nan o negativos. Recalibrar inputs podría solucionarlo." << std::endl;
            flag2 = true;
        }

        impliedVols.push_back(impliedVol);
    }

    // std::cout << "Implied Volatility:" << std::endl;
    // for (const auto& val : impliedVols) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;

    //     std::cout << "Realized Volatility:" << std::endl;
    // for (const auto& val : realizedVols) {
    //     std::cout << val << " ";
    // }
    std::cout << std::endl;
    saveToCSV(impliedVols, "impliedVols.csv");
    saveToCSV(realizedVols, "realizedVols.csv");


    return 0;

}