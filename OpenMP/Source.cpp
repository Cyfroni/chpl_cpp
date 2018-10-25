#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <regex>
#include "ArtificialNeuralNetwork.h"

using namespace std;

vector<string> split(const string &, const string &);

int main(int argc, char **argv) { // argv = [file, cnn, threads]
    fstream file, cnn;
    string line;
    file.open("../" + string(argv[1]), ios::in);

    getline(file, line);
    vector<string> parameters = split(line, ",");
    int dataAmount = stoi(parameters[0]);
    int dataLength = stoi(parameters[1]);

    vector<vector<double>> data(dataAmount, vector<double>(dataLength - 1));
    vector<int> results(dataAmount);

    for (int i = 0; i < dataAmount; ++i) {
            getline(file, line);
            vector<string> input = split(line, ",");
            transform(input.begin(), input.end(), data[i].begin(), [](string &n) { return stod(n); });
    }

    cnn.open("../" + string(argv[2]), ios::in);
    getline(cnn, line);
    vector<string> yyy = split(line, ",");

    int xxx[] = {stoi(yyy[0]), stoi(yyy[1]), stoi(yyy[2]), stoi(yyy[3])};

    ArtificialNeuralNetwork ANN(sizeof(xxx) / sizeof(xxx[0]), xxx);

    for (int j = 0; j < 100; ++j) {
        int good = 0;
        for (int i = 0; i < dataAmount; ++i) {
            if (ANN.computeResult(&data[i][0]) == results[i]) {
                good++;
            }
            ANN.computeLoss(results[i]);
            ANN.computeGradient();
            ANN.learn();
        }
        cout << j << ":" << (double) good / dataAmount << endl;
    }


//    std::cout << "START\n";
//
//    srand((unsigned int) time(NULL));
//
//    std::string path("../");
//
//    /*ArtificialNeuralNetwork ANN("igtest/before/");
//    double tab[4] = { 1.0, 2.0, 3.0, 4.0 };
//    std::cout<<ANN.computeResult(tab)<<std::endl;
//    std::cout << ANN.computeLoss(0) << std::endl;
//    ANN.computeGradient();
//    ANN.learn();
//    ANN.writeAll("igtest/after/");*/
//
//    test_excel(path);
//
//    while (1);
    return 0;
}

vector<string> split(const string &str, const string &delim) {
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}
