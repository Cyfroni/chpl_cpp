//
//  main.cpp
//  mlperceptron
//
//  Created by Sergei Bugrov on 7/1/17.
//  Copyright © 2017 Sergei Bugrov. All rights reserved.
//

#include <iostream>
#include <vector>
#include <math.h>
#include <chrono>
#include <omp.h>
#include <algorithm>
#include <random>

#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <regex>

using namespace std;

int threads;

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

vector<float> sigmoid_d (const vector <float>& m1) {

    /*  Returns the value of the sigmoid function derivative f'(x) = f(x)(1 - f(x)),
        where f(x) is sigmoid function.
        Input: m1, a vector.
        Output: x(1 - x) for every element of the input matrix m1.
    */

    const unsigned long VECTOR_SIZE = m1.size();
    vector <float> output (VECTOR_SIZE);


    for( unsigned i = 0; i < VECTOR_SIZE; ++i ) {
        output[ i ] = m1[ i ] * (1 - m1[ i ]);
    }

    return output;
}

vector <float> sigmoid (const vector <float>& m1) {

    /*  Returns the value of the sigmoid function f(x) = 1/(1 + e^-x).
        Input: m1, a vector.
        Output: 1/(1 + e^-x) for every element of the input matrix m1.
    */

    const unsigned long VECTOR_SIZE = m1.size();
    vector <float> output (VECTOR_SIZE);


    for( unsigned i = 0; i < VECTOR_SIZE; ++i ) {
        output[ i ] = 1 / (1 + exp(-m1[ i ]));
    }

    return output;
}

vector <float> operator+(const vector <float>& m1, const vector <float>& m2){

    /*  Returns the elementwise sum of two vectors.
        Inputs:
            m1: a vector
            m2: a vector
        Output: a vector, sum of the vectors m1 and m2.
    */

    const unsigned long VECTOR_SIZE = m1.size();
    vector <float> sum (VECTOR_SIZE);

    for (unsigned i = 0; i < VECTOR_SIZE; ++i){
        sum[i] = m1[i] + m2[i];
    };

    return sum;
}

vector <float> operator-(const vector <float>& m1, const vector <float>& m2){

    /*  Returns the difference between two vectors.
        Inputs:
            m1: vector
            m2: vector
        Output: vector, m1 - m2, difference between two vectors m1 and m2.
    */

    const unsigned long VECTOR_SIZE = m1.size();
    vector <float> difference (VECTOR_SIZE);

    for (unsigned i = 0; i < VECTOR_SIZE; ++i){
        difference[i] = m1[i] - m2[i];
    };

    return difference;
}

vector <float> operator*(const vector <float>& m1, const vector <float>& m2){

    /*  Returns the product of two vectors (elementwise multiplication).
        Inputs:
            m1: vector
            m2: vector
        Output: vector, m1 * m2, product of two vectors m1 and m2
    */

    const unsigned long VECTOR_SIZE = m1.size();
    vector <float> product (VECTOR_SIZE);

    for (unsigned i = 0; i < VECTOR_SIZE; ++i){
        product[i] = m1[i] * m2[i];
    };

    return product;
}

vector <float> transpose (float *m, const int C, const int R) {

    /*  Returns a transpose matrix of input matrix.
        Inputs:
            m: vector, input matrix
            C: int, number of columns in the input matrix
            R: int, number of rows in the input matrix
        Output: vector, transpose matrix mT of input matrix m
    */

    vector <float> mT (C*R);

    for(unsigned n = 0; n < C*R; n++) {
        unsigned i = n/C;
        unsigned j = n%C;
        mT[n] = m[R*j + i];
    }

    return mT;
}

vector <float> dot (const vector <float>& m1, const vector <float>& m2, const int m1_rows, const int m1_columns, const int m2_columns) {

    /*  Returns the product of two matrices: m1 x m2.
        Inputs:
            m1: vector, left matrix of size m1_rows x m1_columns
            m2: vector, right matrix of size m1_columns x m2_columns (the number of rows in the right matrix
                must be equal to the number of the columns in the left one)
            m1_rows: int, number of rows in the left matrix m1
            m1_columns: int, number of columns in the left matrix m1
            m2_columns: int, number of columns in the right matrix m2
        Output: vector, m1 * m2, product of two vectors m1 and m2, a matrix of size m1_rows x m2_columns
    */

    vector <float> output (m1_rows*m2_columns);

    //#pragma omp parallel num_threads(threads)
    {
      //  #pragma omp for schedule(guided)
        for( int row = 0; row < m1_rows; ++row ) {
            for( int col = 0; col < m2_columns; ++col ) {
                output[ row * m2_columns + col ] = 0.f;
                for( int k = 0; k < m1_columns; ++k ) {
                    output[ row * m2_columns + col ] += m1[ row * m1_columns + k ] * m2[ k * m2_columns + col ];
                }
            }
        }
    }

    return output;
}

template <class T>
void print ( const vector <T>& m, int n_rows = 1) {

    /*  "Couts" the input vector as n_rows x n_columns matrix.
        Inputs:
            m: vector, matrix of size n_rows x n_columns
            n_rows: int, number of rows in the left matrix m1
            n_columns: int, number of columns in the left matrix m1
    */

    int n_columns = m.size() / n_rows;

    for( int i = 0; i < n_rows; ++i ) {
        for( int j = 0; j < n_columns; ++j ) {
            cout << m[ i * n_columns + j ] << " ";
        }
        cout << endl;
    }
    cout << endl;

}

vector <float> softmax (const vector <float>& z, const int dim) {

    const int zsize = static_cast<int>(z.size());
    vector <float> out;

    for (unsigned i = 0; i != zsize; i += dim) {
        vector <float> foo;
        for (unsigned j = 0; j != dim; ++j) {
            foo.push_back(z[i + j]);
        }

        float max_foo = *max_element(foo.begin(), foo.end());

        for (unsigned j = 0; j != dim; ++j) {
            foo[j] = exp(foo[j] - max_foo);
        }

        float sum_of_elems = 0.0;
        for (unsigned j = 0; j != dim; ++j) {
            sum_of_elems = sum_of_elems + foo[j];
        }

        for (unsigned j = 0; j != dim; ++j) {
            out.push_back(foo[j]/sum_of_elems);
        }
    }
    return out;
}

static vector<float> random_vector(const int size)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> distribution(0.0, 0.05);
    static default_random_engine generator;

    vector<float> data(size);
    generate(data.begin(), data.end(), [&]() { return distribution(generator); });
//    print(data);
    return data;
}

float DNN(vector<float>& input, vector<float>& y, vector<int>& cnnSize) {

    int epoch = 50;
    int dataAmount = y.size();
    int categories = cnnSize.back();

    vector<vector<float>> W;

    for ( int size : cnnSize){
        W.push_back(random_vector(size));
    }

    auto activate = sigmoid;
    auto deactivate = sigmoid_d;
    auto normalize = softmax;

    const auto t1 = chrono::high_resolution_clock::now();
    for (unsigned i = 1; i <= epoch; ++i) {
        vector<vector<float>> X = {input};

        for (int j=0; j<cnnSize.size() - 1; ++j) {
            int a_size = cnnSize[j];
            int b_size = cnnSize[j+1];

            vector<float> &a = X.back();
            vector<float> &w = W[j];
            vector<float> &&b = activate(dot(a, w, dataAmount, a_size, b_size));
            X.push_back(b);
//            print(b, b_size);
        }

        vector<float> &a = X.back();
        vector<float> &w = W.back();

        vector<float> pred = normalize(dot(a, w, dataAmount, cnnSize.rbegin()[1], categories), categories);

//        print(dot(a, w, dataAmount, cnnSize.rbegin()[1], categories), categories);
        print(pred, categories);

//        vector<float> &l = pred;
        vector<float> dl = pred - y;
//
        vector<float> *d = &dl;

        for (int j=cnnSize.size() - 2; j>=0; --j) {
            int a_size = cnnSize[j];
            int b_size = cnnSize[j+1];
            //int c_size = cnnSize[j+1];

            vector<float> &a = X[j+1];
            vector<float> &w = W[j+1];

            vector<float> &&dx = j == cnnSize.size() - 2 ? dl :
                dot(*d, transpose( &w[0], b_size, cnnSize[j+1] ), dataAmount, cnnSize[j+1], b_size) * deactivate(a);

            vector<float> dw = dot(transpose( &a[0], dataAmount, a_size ), *d, a_size, dataAmount, b_size);
            w = w + dw;
            d = new vector<float>(dx);
        }

        if (i == epoch){
            //cout<<W.size();
            cout<<"##\n";
//            for (auto w : W){
//                print(w);
//            }
            cout<<"@@\n";
//            print(pred);
        };

    }
    const auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> fp_ms = t2 - t1;

    return fp_ms.count() / 1000.0;
}

int main(int argc, char **argv) { // argv = [file, cnn, threads]
    fstream file, cnn;
    string line;

    threads = stoi(argv[3]);

    file.open("../" + string(argv[1]), ios::in);

    getline(file, line);
    vector<string> parameters = split(line, ",");
    int dataAmount = stoi(parameters[0]);
    int dataLength = stoi(parameters[1]);

    vector<float> data(dataAmount * (dataLength - 1));
    vector<float> results(dataAmount);

    for (int i = 0; i < dataAmount; ++i) {
            getline(file, line);
            vector<string> input = split(line, ",");
            transform(input.begin(), input.begin() + 2, results.begin() + i, [](string &n) { return stof(n); });
            transform(input.begin() + 1, input.end(), data.begin() + i * (dataLength - 1) , [](string &n) { return stof(n); });
    }

    file.close();
    cnn.open("../" + string(argv[2]), ios::in);


    getline(cnn, line);
    vector<string> cnnParams = split(line, ",");

    vector<int> cnnSize(cnnParams.size() + 1);
    cnnSize[0] = dataLength - 1;
    transform(cnnParams.begin(), cnnParams.end(), cnnSize.begin() + 1, [](string &n) { return stoi(n); });

    cnn.close();

//    print(data, dataAmount);
//    print(results, dataAmount);
//    print(cnnSize);
    float time = DNN(data,results,cnnSize);

    cout<<endl<<"time: "<<time<<endl;

    return 0;
}
