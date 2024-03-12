#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// Genero structs para mayor orden.
// Con vectores a utilizar que se encuentran dentro del CSV.
struct CSVData {
    std::vector<double> price;
    std::vector<double> underPrice;
    std::vector<double> timeToMaturity;
};
// Con constantes que no cambiarán durante el proceso.
struct Constantes {
    const double K = 1033;
    const double r = 0.9;
    const double abstol = 1e-4;
    const double epsilon = 1.0;
    const int max_iter = 1e3;
};
// Creo una función que toma un string, genera una instancia del struct CSVData y llena sus vectores con el
// contenido del Csv.
CSVData readCSV(const std::string& filename) {
    CSVData data;

    try {

        std::ifstream file(filename);

        // Checkeo si efectivamente se abrió el archivo correctamente. Donde en caso de fallar, se detendrá el proceso.
        if (!file.is_open()) {
            throw std::ios_base::failure("Error opening file.");
        }

        std::string line;
        // Guardo el header
        std::getline(file, line);

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;

            // Guardo los datos en los vectores.
            std::getline(ss, token, ';');
            data.price.push_back(std::stod(token));

            std::getline(ss, token, ';');
            data.underPrice.push_back(std::stod(token));

            std::getline(ss, token, ';');
            data.timeToMaturity.push_back(std::stod(token));
        }

        // Cierro el archivo
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        // Limpio data si ocurre una excepción.
        data = CSVData();
    }

    return data;
}

// Esta función toma un vector y un string y guarda el contenido del vector en un archivo Csv.
void saveToCSV(const std::vector<double>& vec, const std::string& filename) {
    std::ofstream file(filename);
    // Chekeo que se pudo abrir efectivamente. En caso de fallar se printea un error.
    if (!file.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    
    file << "Volatility" << std::endl;

    
    for (size_t i = 0; i < vec.size(); ++i) {
        file << vec[i] << std::endl;
    }

    file.close();
}
// Función que calcula la constante Pi.
double PI() {
    return M_PI;
}

// Esta función calcula la CDF que voy a necesitar para calcular d1 y d2 de Black-Scholes.
double N(double x) {
    return 0.5 * (1.0 + erf(x / sqrt(2.0)));
}

// Esta función calcula la normal que voy a necesitar para la función Vega, o derivada de Black-Scholes.
double F(double x) {
    double factor = 1.0/sqrt(2.0*PI());
    double e = exp(-pow(x, 2)/2.0);
    return factor*e;
}

// Esta función calcula d1, el argumento de la primer parte de la fórmula de Black-Scholes.
double D1(double S, double K, double r, double v, double t) {
    double top = log(S / K) + (r + 0.5 * pow(v, 2)) * t;
    double bottom = v * sqrt(t);
    return top / bottom;
}
// Esta función calcula d2, el argumento de la segunda parte de al fórmula de Black-Scholes.
double D2(double d1, double v, double t) {
    return d1 - v*sqrt(t);
}

// Esta función obtiene el valor del precio de la opción usando la fórmula de Black-Scholes.
double BlackScholes(double d1, double d2, double S, double K, double r, double t) {
    double option_price = 0;
    option_price = S*N(d1) - K*exp(-r*t)*N(d2);
    return option_price;
}

// Esta función realiza el cálculo del valor de la función Vega la cual es la derivada en función de sigma de la fórmula de Black-Scholes.
double Vega(double S, double d1, double t) {
    return S*F(d1)*sqrt(t);
}

// Esta función calcula la volatilidad implícita para cada uno de los puntos de mis datos.
double ImpliedVolatility(double epsilon, double abstol, double max_iter, double S,double K,double r,double v,double t,double C0){

    int i = 0;
    while (epsilon > abstol) {
        // En caso de superar las 1000 iteraciones sin converger, se romperá el loop. 
        if (i > max_iter) {
            std::cout << "El programa ha alcanzado el máximo de iteraciones "<< std::endl;
            break;
        }
        i++;
        // uso un método de root finding para obtener el valor de la volatilidad.
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

// Esta función calcula la volatilidad realizada tomando el punto actual y el anterior para calcularla.
double RealizedVolatility(double underPrice,double timeToMaturity, double initUnderPrice, double initTimeToMaturity) {
    // Calculo retornos logarítmicos.
    double logReturn = std::log(underPrice / initUnderPrice );
    double timeDifference = std::abs(timeToMaturity - initTimeToMaturity);
    double realizedVolatility = 0.0;
    // Me aseguro de no estar dividiendo por 0.
    if (timeDifference > 1e-6) {
        double weightedReturn = pow(logReturn,2) / timeDifference;
        realizedVolatility = std::sqrt((weightedReturn)/2);
    }
    return realizedVolatility;
}

int main() {
    // Defino variables y vectores a utilizar y también agrego mi estimación de volatillidad inicial para comenzar el loop.
    double v = 0.5;
    // Defino dos variables que se utilizarán al calcular la volatilidad realizada para guardar el punto anterior.
    double initUnderPrice = 0.0;
    double initTimeToMaturity = 0.0;
    std::vector<double> impliedVols;
    std::vector<double> realizedVols;
    // Genero flags para marcar warnings.
    bool flag1 = false;
    bool flag2 = false;

    // Inicializo instancias de los structs.
    Constantes constantes;
    CSVData data = readCSV("processedData.csv");

    // Genero variable con el largo del vector price que debería ser igual a los demás.
    size_t dataSize = data.price.size();

    // Checkeo que los vectores tengan el mismo largo. En caso de que no lo sean activo la flag y printeo un warning.
    if ((data.underPrice.size() != dataSize || data.timeToMaturity.size() != dataSize) && (!flag1)) {
        std::cout << "Cuidado, los vectores cargados del csv no tienen el mismo largo." << std::endl;
        flag1 = true;
    }

    for (size_t i = 0; i < dataSize; ++i) {
        // Defino los datos utilizados en esta vuelta del loop a partir de los vectores del CSV.
        double currentPrice = data.price[i]; 
        double currentUnderPrice = data.underPrice[i];
        double currentTimeToMaturity = data.timeToMaturity[i];
        // calculo volatilidad implicita para estos datos.
        double impliedVol = ImpliedVolatility(constantes.epsilon, constantes.abstol, constantes.max_iter, currentUnderPrice, constantes.K, constantes.r, v, currentTimeToMaturity,currentPrice);
        // Dado que la volatilidad realizada la calculo a partir del segundo punto, distingo i = 0 de los demás.
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
        // En caso de tener resultados nan o negativos, activo la segunda flag y printeo un warning.
        if ((std::isnan(impliedVol) || (impliedVol < 0.0)) && (!flag2)) {
            std::cout << "Los resultados de volatilidad implicita contienen valores nan o negativos. Recalibrar inputs podría solucionarlo." << std::endl;
            flag2 = true;
        }
        // Guardo el valor de la volatilidad calculada en el vector.
        impliedVols.push_back(impliedVol);
    }

    // Guardo en archivos csv los valores obtenidos para las volatilidades implícita y realizada.
    saveToCSV(impliedVols, "impliedVols.csv");
    saveToCSV(realizedVols, "realizedVols.csv");
    
    std::cout << "Las volatilidades fueron calculadas exitosamente." << std::endl;

    return 0;
}