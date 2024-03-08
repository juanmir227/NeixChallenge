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



// Function to calculate logarithmic returns
std::vector<double> calculateLogReturns(const CSVData& data) {
    std::vector<double> logReturns;

    for (size_t i = 1; i < data.underPrice.size(); ++i) {
        double logReturn = std::log(data.underPrice[i] / data.underPrice[i - 1]);
        logReturns.push_back(logReturn);
    }

    return logReturns;
}

// Function to calculate realized volatility based on adjacent points
double calculateRealizedVolatility(const std::vector<CSVData>& data) {
    std::vector<double> logReturns = calculateLogReturns(data);
    double sumWeightedReturns = 0.0;

    for (size_t i = 1; i < logReturns.size(); ++i) {
        double timeDifference = data[i].timeToMaturity - data[i - 1].timeToMaturity;

        // Ensure timeDifference is not zero to avoid division by zero
        if (std::abs(timeDifference) > 1e-6) {
            double weightedReturn = logReturns[i - 1] / timeDifference;
            sumWeightedReturns += weightedReturn;
        }
    }

    // Calculate realized volatility as the square root of the sum of weighted returns
    return std::sqrt(std::abs(sumWeightedReturns));
}

int main() {
    // Example vector of data points (replace with your actual data)
    std::vector<CSVData> data = {{1209.5, 0.11575342465753424},
                                   {1210.5, 0.1157496194824962},
                                   {1211.75, 0.11574771689497718},
                                   {1210.5, 0.11574654443714822},
                                   {1206.5, 0.11574525952335573}};

    // Calculate realized volatility based on adjacent points
    double realizedVolatility = calculateRealizedVolatility(data);

    // Print the result
    std::cout << "Realized Volatility: " << realizedVolatility << std::endl;

    return 0;
}
