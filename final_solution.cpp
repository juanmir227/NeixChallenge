#include <iostream>
#include <cmath>


double standardNormalCDF(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}




int main() {
    double x = 1.5;
    double cdfValue = standardNormalCDF(x);

    std::cout << "Standard Normal CDF at x = " << x << ": " << cdfValue << std::endl;

    return 0;
}