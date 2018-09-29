#include "ArtificialNeuralNetwork.h"
#include <iostream>
#include <random>
#include <fstream>

using namespace std;

//Adam
static double beta1 = 0.9;
static double beta2 = 0.999;
static double eps = 1e-8;
static double t = 1;

//Hyperparameters
static double lambda = 0.0005;
static double ratio = 0.001;

//
static bool obias = false;

//
static InitializationFunction initialization = Sqrt;
static UpdateFunction update = Vanilla;
static LossFunction loss = SVM;

//
int i_limit, j_limit, k_limit;
int _layer, prev_layer;

void setRatio(double rat) {
    ratio = rat;
}

double getRatio() {
    return ratio;
}

void setLambda(double lam) {
    lambda = lam;
}

double getLambda() {
    return lambda;
}

void setInit(InitializationFunction init) {
    initialization = init;
}

void setUpd(UpdateFunction up) {
    update = up;
}

void setLoss(LossFunction lo) {
    loss = lo;
}

void endANN() {
    cout << "\nInitialization method(" << initialization << "): ";
    if (initialization == Random) cout << "random <-1,1>";
    else if (initialization == Sqrt) cout << "sqrt(2/n)";
    else if (initialization == Xavier) cout << "Xavier";
    cout << "\nUpdate method(" << update << "): ";
    if (update == Vanilla) cout << "Vanilla";
    else if (update == Adam) cout << "Adam";
    cout << "\nLoss method(" << loss << "): ";
    if (loss == SVM) cout << "SVM";
    else if (loss == Softmax) cout << "Softmax";
    cout << "\nRatio: " << ratio
         << "\nLambda: " << lambda << endl;
}

ArtificialNeuralNetwork::ArtificialNeuralNetwork(const int lA, int *lC) {
    create(lA, lC);
}

ArtificialNeuralNetwork::ArtificialNeuralNetwork(string path) {
    if (!readAll(path)) cout << "Blad wczytywania\n";
}

void ArtificialNeuralNetwork::create(const int lA, int *lC) {
    lAmount = lA;
    lCardinality = lC;
    mCardinality = 0;
    for (int i = 0; i < lAmount; i++)
        if (mCardinality < lCardinality[i]) mCardinality = lCardinality[i];
    Neurons = new Neuron *[mCardinality * lAmount];

    double init_range;
    int nextLC;
    int prevLC;
    string type;
    double *w;
    default_random_engine generator;
    normal_distribution<double> distribution(0.0, 1.0);
    for (int i = 0; i < lAmount; i++) {
        _layer = i * mCardinality;
        j_limit = lCardinality[i];
        nextLC = (i < lAmount - 1 ? lCardinality[i + 1] : 0);
        prevLC = (i > 0 ? lCardinality[i - 1] : 0);
        for (int j = 0; j < j_limit; j++) {
            w = nullptr;
            if (i < lAmount - 1) {
                w = new double[nextLC];

                if (initialization == Random) {
                    for (int k = 0; k < nextLC; k++)
                        w[k] = distribution(generator);
                } else if (initialization == Sqrt) {
                    for (int k = 0; k < nextLC; k++)
                        w[k] = distribution(generator) * sqrt(2.0 / (i > 0 ? prevLC : 1));
                } else if (initialization == Xavier) {
                    init_range = 8.0 * sqrt(6.0 / (double) (prevLC + nextLC));
                    for (int k = 0; k < nextLC; k++)
                        w[k] = (double) rand() / RAND_MAX * init_range - init_range / 2.0;
                }

                if (i == 0) type = "Input";
                else type = "Hidden";
            } else
                type = "Output";

            *(Neurons + _layer + j) = new Neuron(nextLC, w, type);
        }
    }
}

void ArtificialNeuralNetwork::writeAll(string path) {
    fstream plik;
    plik.open(path + "info.txt", ios::out);

    plik << "Ilosc warstw:\n"
         << lAmount
         << "\nLicznosci warstw:\n";
    for (int i = 0; i < lAmount; i++)
        plik << lCardinality[i] << " ";
    plik.close();

    for (int i = 0; i < lAmount; i++) {
        plik.open(path + to_string(i) + ".txt", ios::out);
        j_limit = lCardinality[i];
        _layer = i * mCardinality;
        for (int j = 0; j < j_limit; j++) {
            plik << "#" << to_string(j) + ": ";
            (*Neurons[_layer + j]).writeToFile(&plik);
            plik << "\n\n";
        }
        plik.close();
    }
}

bool ArtificialNeuralNetwork::readAll(string path) {
    fstream plik;
    string x;
    plik.open(path + "info.txt", ios::in);

    getline(plik, x);
    plik >> lAmount;
    getline(plik, x);
    getline(plik, x);
    lCardinality = new int[lAmount];
    for (int i = 0; i < lAmount; i++)
        plik >> lCardinality[i];
    plik.close();

    mCardinality = 0;
    for (int i = 0; i < lAmount; i++)
        if (mCardinality < lCardinality[i]) mCardinality = lCardinality[i];
    Neurons = new Neuron *[mCardinality * lAmount];

    for (int i = 0; i < lAmount; i++) {
        j_limit = lCardinality[i];
        _layer = i * mCardinality;
        for (int j = 0; j < j_limit; j++) {
            Neuron *x = new Neuron();
            *(Neurons + _layer + j) = x;
        }
    }


    for (int i = 0; i < lAmount; i++) {
        plik.open(path + to_string(i) + ".txt", ios::in);
        int l = lCardinality[i];
        for (int j = 0; j < l; j++) {
            (*Neurons[i * mCardinality + j]).readFromFile(&plik);
            getline(plik, x);
        }
        plik.close();
    }
    return true;
}

int ArtificialNeuralNetwork::computeResult(double *data) {
    i_limit = lCardinality[0];
    for (int i = 0; i < i_limit; i++) {
        (*Neurons[i]).value = data[i];
    }
    for (int i = 1; i < lAmount; i++) {
        j_limit = lCardinality[i];
        _layer = i * mCardinality;
        prev_layer = (i - 1) * mCardinality;
        for (int j = 0; j < j_limit; j++) {
            (*Neurons[_layer + j]).value = (*Neurons[_layer + j]).bias;
            for (int k = 0; k < lCardinality[i - 1]; k++) {
                (*Neurons[_layer + j]).value +=
                        (*Neurons[prev_layer + k]).value * (*Neurons[prev_layer + k]).weights[j];
            }
            if (i < lAmount - 1) (*Neurons[_layer + j]).activate();
        }
    }
    int result = 0;
    i_limit = lCardinality[lAmount - 1]; // last
    _layer = (lAmount - 1) * mCardinality; // last
    for (int i = 1; i < i_limit; i++)
        if ((*Neurons[_layer + i]).value == (*Neurons[_layer + 0]).value) result++;
    if (result == i_limit - 1)
        return -1;

    result = 0;
    for (int i = 1; i < i_limit; i++)
        if ((*Neurons[_layer + i]).value > (*Neurons[_layer + result]).value) result = i;
    return result;
}

double ArtificialNeuralNetwork::computeLoss(int result) {
    double amount = 0;
    i_limit = lAmount - 1;
    for (int i = 0; i < i_limit; i++) {
        j_limit = lCardinality[i + 1];    //next layer
        k_limit = lCardinality[i];        //act layer
        _layer = i * mCardinality;
        for (int j = 0; j < j_limit; j++)
            for (int k = 0; k < k_limit; k++)
                amount += 0.5 * lambda * pow((*Neurons[_layer + k]).weights[j], 2);    //L2 Regularization
    }


    i_limit = lCardinality[lAmount - 1];
    _layer = (lAmount - 1) * mCardinality; //last
    if (loss == SVM) {
        double iSVM;
        (*Neurons[(lAmount - 1) * mCardinality + result]).gradient = 0.0;
        for (int i = 0; i < i_limit; i++) {
            if (i == result) continue;
            iSVM = (*Neurons[_layer + i]).value - (*Neurons[_layer + result]).value + 1;
            if (iSVM <= 0.0) (*Neurons[_layer + i]).gradient = 0.0;
            else {
                (*Neurons[_layer + i]).gradient = 1.0;
                (*Neurons[_layer + result]).gradient -= 1.0;
                amount += iSVM;
            }
        }
    } else if (loss == Softmax) {
        vector<double> vec;
        double sum = 0.0;
        double softmax;
        for (int i = 0; i < i_limit; i++) {
            vec.push_back((*Neurons[_layer + i]).value);
            vec[i] = exp(vec[i]);
            sum += vec[i];
        }
        amount += -log10(vec[result] / sum);
        for (int i = 0; i < i_limit; i++) {
            if (i == result)
                (*Neurons[_layer + i]).gradient = -(sum - vec[i]) / (sum * log(10));
            else
                (*Neurons[_layer + i]).gradient = vec[i] / (sum * log(10));
        }
    }
    return amount;
}

void ArtificialNeuralNetwork::computeGradient() {
    i_limit = lAmount - 2;
    for (int i = 0; i < i_limit; i++) {
        j_limit = lCardinality[i];
        _layer = i * mCardinality;
        for (int j = 0; j < j_limit; j++)
            (*Neurons[_layer + j]).gradient = 0.0;
    }
    for (int i = lAmount - 1; i > 0; i--) {
        j_limit = lCardinality[i];
        k_limit = lCardinality[i - 1];
        _layer = i * mCardinality;
        prev_layer = (i - 1) * mCardinality;
        for (int j = 0; j < j_limit; j++) {
            if (i < lAmount - 1) (*Neurons[_layer + j]).reActivate();
            double gradient = (*Neurons[_layer + j]).gradient;
            for (int k = 0; k < k_limit; k++)
                (*Neurons[prev_layer + k]).gradient += gradient * (*Neurons[prev_layer + k]).weights[j];
        }
    }
}

void ArtificialNeuralNetwork::learn() {
    double m;
    double v;
    double weight;
    double gradient;
    double value;
    for (int i = lAmount - (obias == true ? 0 : 1); i > 0; i--) {
        j_limit = lCardinality[i - 1];
        k_limit = lCardinality[i];
        _layer = i * mCardinality;
        prev_layer = (i - 1) * mCardinality;
        for (int j = 0; j < j_limit; j++) {
            value = (*Neurons[prev_layer + j]).value;
            if (i < lAmount)
                for (int k = 0; k < k_limit; k++) {
                    weight = (*Neurons[prev_layer + j]).weights[k];
                    gradient = (*Neurons[_layer + k]).gradient;

                    if (update == Vanilla)
                        (*Neurons[prev_layer + j]).weights[k] -= ratio * gradient * value;

                    else if (update == Adam) {
                        m = (*Neurons[prev_layer + j]).m[k] =
                                beta1 * (*Neurons[prev_layer + j]).m[k] + (1 - beta1) * gradient * value;
                        v = (*Neurons[prev_layer + j]).v[k] =
                                beta2 * (*Neurons[prev_layer + j]).v[k] + (1 - beta2) * pow(gradient * value, 2.0);
                        (*Neurons[prev_layer + j]).weights[k] -=
                                ratio * (m / (1 - pow(beta1, t)) / (sqrt(v / (1 - pow(beta2, t))) + eps));
                        t++;
                    }
                    (*Neurons[prev_layer + j]).weights[k] -= weight * lambda; //L2 Regularization
                }
            if (i > 0) {
                if (update == Vanilla)
                    (*Neurons[prev_layer + j]).bias -= ratio * (*Neurons[prev_layer + j]).gradient;
                else if (update == Adam) {
                    (*Neurons[prev_layer + j]).bias -= ratio * (*Neurons[prev_layer + j]).gradient;
                }
            }
        }
    }
}

void ArtificialNeuralNetwork::reset() {
    t = 1;
    i_limit = lAmount - 1;
    for (int i = 0; i < i_limit; i++) {
        j_limit = lCardinality[i];
        _layer = i * mCardinality;
        for (int j = 0; j < j_limit; j++)
            (*Neurons[_layer + j]).reset();
    }
}