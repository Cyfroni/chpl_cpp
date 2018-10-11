#include "Neuron.h"
#include <fstream>
#include <math.h>
#include <iostream>

using namespace std;


static ActivationFunction activation = ReLU;
static double alfa = 0.01;

void setActiv(ActivationFunction activ) {
    activation = activ;
}

void endN() {
    cout << "Activation method(" << activation << "): ";
    if (activation == linear) cout << "no activation (linear)";
    else if (activation == ReLU) cout << "ReLU ";
    else if (activation == PReLU) cout << "PReLU (" << alfa << ")";
    else if (activation == ELU) cout << "ELU (" << alfa << ")";
    else if (activation == Tanh) cout << "tanh";
    else if (activation == Sigmoid) cout << "sigmoid ";
}

Neuron::Neuron(int nLC, double *w, string t) {
    Neuron();
    nextLayerCardinality = nLC;
    weights = w;
    type = t;
    m = new double[nextLayerCardinality];
    v = new double[nextLayerCardinality];
    for (int i = 0; i < nextLayerCardinality; i++) {
        m[i] = 0.0;
        v[i] = 0.0;
    }
}

Neuron::Neuron() {
    bias = 0.0;
    value = 0.0;
    nextLayerCardinality = 0;
    gradient = 0.0;
    weights = nullptr;
    type = "";
    m = nullptr;
    v = nullptr;
}

void Neuron::reset() {
    value = 0.0;
    gradient = 0.0;
    for (int i = 0; i < nextLayerCardinality; i++) {
        m[i] = 0.0;
        v[i] = 0.0;
    }
}

void Neuron::activate() {
    if (activation == linear);                                                                //no activation
    else if (activation == ReLU) value = (value < 0 ? 0.0 : value);                        //ReLU
    else if (activation == PReLU) value = (value < 0 ? alfa * value : value);                //PReLU
    else if (activation == ELU) value = (value < 0 ? alfa * (exp(value) - 1) : value);    //ELU
    else if (activation == Tanh) value = tanh(value);                                    //tanh
    else if (activation == Sigmoid) value = 1 / (1 + exp(-value));                            //sigmoid
}

void Neuron::reActivate() {
    if (activation == linear);                                                                //no activation
    else if (activation == ReLU) gradient *= (value <= 0 ? 0 : 1);                        //ReLU
    else if (activation == PReLU) gradient *= (value <= 0 ? alfa : 1);                    //PReLU
    else if (activation == ELU) gradient *= (value <= 0 ? alfa * exp(value) : 1);        //ELU
    else if (activation == Tanh) gradient *= 1 / pow(cosh(value), 2);                    //tanh
    else if (activation == Sigmoid) gradient *= (exp(value) / pow(exp(value) + 1, 2));        //sigmoid
}

void Neuron::writeToFile(fstream *plik) {
    *plik << type;
    if (type.compare("Input")) {
        *plik << "\nBias:\n"
              << bias;
    }
    *plik << "\nGradient:\n"
          << gradient
          << "\nWarto��:\n"
          << value;
    if (type.compare("Output")) {
        *plik << "\nIlo�� Wag:\n"
              << nextLayerCardinality
              << "\nWagi:\n";
        for (int i = 0; i < nextLayerCardinality; i++)
            *plik << weights[i] << " ";
    }


}

void Neuron::readFromFile(fstream *plik) {
    string x;
    getline(*plik, x); // #x: ?????\n
    type = x.substr(x.find(" ") + 1);
    if (!type.compare("Input")) {
        bias = 0;
    } else {
        getline(*plik, x); // "bias\n"
        *plik >> bias;
        getline(*plik, x); // "\n"

    }
    getline(*plik, x); // "gradient\n"
    getline(*plik, x); // gradient \n
    getline(*plik, x); //"value\n"
    getline(*plik, x); // value \n
    if (!type.compare("Output")) {
        weights = nullptr;
    } else {
        getline(*plik, x); // "ilosc wag\n"
        *plik >> nextLayerCardinality;
        getline(*plik, x); // "\n"
        getline(*plik, x); // "wagi\n"
        weights = new double[nextLayerCardinality];
        m = new double[nextLayerCardinality];
        v = new double[nextLayerCardinality];
        for (int i = 0; i < nextLayerCardinality; i++) {
            *plik >> weights[i];
            m[i] = 0.0;
            v[i] = 0.0;
        }
        getline(*plik, x); // "\n"
    }
}