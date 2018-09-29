#ifndef _ANN_HPP
#define _ANN_HPP

#include <string>
#include "Neuron.hpp"

enum InitializationFunction {
    Random, Sqrt, Xavier
};

enum UpdateFunction {
    Vanilla, Adam
};

enum LossFunction {
    SVM, Softmax
};

void setRatio(double);

double getRatio();

void setLambda(double);

double getLambda();

void setInit(InitializationFunction);

void setUpd(UpdateFunction);

void setLoss(LossFunction);

void endANN();

class ArtificialNeuralNetwork {

    int lAmount;        //Ilo�� warstw
    int *lCardinality;    //Licznosc poszczeg�lnych warst
    Neuron **Neurons;    //Sie�
    int mCardinality;    //Ilosc neuron�w w najliczniejszej warstwie

public:

    ArtificialNeuralNetwork(const int lA, int *lC);    //Tworzy sie� o "lA" warstwach o licznosciach "lC"
    ArtificialNeuralNetwork(std::string path);        //Wczytanie sieci z pliku ze scie�ki "path"
    void create(const int lA, int *lC);                //Tworzy sie� o "lA" warstwach o licznosciach "lC"
    void writeAll(std::string = "");                //Zapisanie sieci do pliku ze scie�ki "path"
    bool readAll(std::string = "");                    //Wczytanie sieci z pliku ze scie�ki "path"
    int computeResult(double *data);                //Przyporz�dkowanie dane "data" do klasy
    double computeLoss(int result);                    //Liczy strate w stosunku do klasy "result"
    void computeGradient();                            //Oblicza gradienty dla wszystkich neuron�w
    void learn();                                    //Aktualizuje wagi i biasy w zale�no�ci od gradient�w
    void reset();                                    //Resetuje parametry potrzebne do algorytmu Adam
};

#endif