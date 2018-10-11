#ifndef _N_H
#define _N_H

#include <string>

enum ActivationFunction {
    linear, ReLU, PReLU, ELU, Tanh, Sigmoid
};

void setActiv(ActivationFunction);

void endN();

class Neuron {
public:

    double bias;                //b
    double value;                //x
    int nextLayerCardinality;    //Liczno�� nast�pnej warstwy
    double gradient;
    double *weights;            //w
    std::string type;            //Input, Hidden, Output
    double *m;                    //Parametr potrzebny do algorytmu Adam
    double *v;                    //Parametr potrzebny do algorytmu Adam

    Neuron(int nLC, double *w,
           std::string t);    //Tworzy neuron o Licznosci nast�pnej warstwy = "nLC", wagach "w" i typie "t"
    Neuron();                                    //Domy�lny konstruktow
    void reset();                                //Resetuje parametry algorytmu Adam
    void activate();                            //Funkcja aktywacji
    void reActivate();                            //Cofni�ta aktywacja (faza Backpropagation)
    void writeToFile(std::fstream *plik);        //Zapisanie do pliku
    void readFromFile(std::fstream *plik);        //Wczytanie z pliku
};

#endif