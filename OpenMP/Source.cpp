#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string.h>
#include <random>
#include <algorithm>
#include <chrono>
#include <omp.h>
#include <ctime>

using namespace std;

int threads = 1;

void print(const vector<float> &m, int n_rows, int n_columns) {

    for (int i = 0; i < n_rows; ++i) {
        for (int j = 0; j < n_columns; ++j) {
            cout << m[i * n_columns + j] << " ";
        }
        cout << '\n';
    }
}

int argmax(const vector<float> &m) {

    return distance(m.begin(), max_element(m.begin(), m.end()));
}

vector<float> relu(const vector<float> &z) {
    int size = z.size();
    vector<float> output;
    for (int i = 0; i < size; ++i) {
        if (z[i] < 0) {
            output.push_back(0.0);
        } else output.push_back(z[i]);
    }
    return output;
}

vector<float> reluPrime(const vector<float> &z) {
    int size = z.size();
    vector<float> output;
    for (int i = 0; i < size; ++i) {
        if (z[i] <= 0) {
            output.push_back(0.0);
        } else output.push_back(1.0);
    }
    return output;
}

static vector<float> random_vector(const int size) {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> distribution(0.0, 0.05);
    static default_random_engine generator;

    vector<float> data(size);
    generate(data.begin(), data.end(), [&]() { return distribution(generator); });
    return data;
}

vector<float> softmax(const vector<float> &z, const int dim) {

    const int zsize = static_cast<int>(z.size());
    vector<float> out;

    for (unsigned i = 0; i < zsize; i += dim) {
        vector<float> foo;
        for (unsigned j = 0; j < dim; ++j) {
            foo.push_back(z[i + j]);
        }

        float max_foo = *max_element(foo.begin(), foo.end());

        for (unsigned j = 0; j < dim; ++j) {
            foo[j] = exp(foo[j]); // - max_foo);
        }

        float sum_of_elems = 0.0;
        for (unsigned j = 0; j < dim; ++j) {
            sum_of_elems = sum_of_elems + foo[j];
        }

        for (unsigned j = 0; j < dim; ++j) {
            out.push_back(foo[j] / sum_of_elems);
        }
    }
    return out;
}

vector<float> sigmoid_d(const vector<float> &m1) {

    const unsigned long VECTOR_SIZE = m1.size();
    vector<float> output(VECTOR_SIZE);


    for (unsigned i = 0; i < VECTOR_SIZE; ++i) {
        output[i] = m1[i] * (1 - m1[i]);
    }

    return output;
}

vector<float> sigmoid(const vector<float> &m1) {

    const unsigned long VECTOR_SIZE = m1.size();
    vector<float> output(VECTOR_SIZE);


    for (unsigned i = 0; i < VECTOR_SIZE; ++i) {
        output[i] = 1 / (1 + exp(-m1[i]));
    }

    return output;
}

vector<float> operator+(const vector<float> &m1, const vector<float> &m2) {

    const unsigned long VECTOR_SIZE = m1.size();
    vector<float> sum(VECTOR_SIZE);

    for (unsigned i = 0; i < VECTOR_SIZE; ++i) {
        sum[i] = m1[i] + m2[i];
    };

    return sum;
}

vector<float> operator-(const vector<float> &m1, const vector<float> &m2) {

    const unsigned long VECTOR_SIZE = m1.size();
    vector<float> difference(VECTOR_SIZE);

    for (unsigned i = 0; i < VECTOR_SIZE; ++i) {
        difference[i] = m1[i] - m2[i];
    };

    return difference;
}

vector<float> operator*(const vector<float> &m1, const vector<float> &m2) {

    const unsigned long VECTOR_SIZE = m1.size();
    vector<float> product(VECTOR_SIZE);

    for (unsigned i = 0; i < VECTOR_SIZE; ++i) {
        product[i] = m1[i] * m2[i];
    };

    return product;
}

vector<float> operator*(const float m1, const vector<float> &m2) {

    const unsigned long VECTOR_SIZE = m2.size();
    vector<float> product(VECTOR_SIZE);

    for (unsigned i = 0; i < VECTOR_SIZE; ++i) {
        product[i] = m1 * m2[i];
    };

    return product;
}

vector<float> operator/(const vector<float> &m2, const float m1) {

    const unsigned long VECTOR_SIZE = m2.size();
    vector<float> product(VECTOR_SIZE);

    for (unsigned i = 0; i < VECTOR_SIZE; ++i) {
        product[i] = m2[i] / m1;
    };

    return product;
}

vector<float> transpose(float *m, const int C, const int R) {

    vector<float> mT(C * R);

    for (unsigned n = 0; n < C * R; n++) {
        unsigned i = n / C;
        unsigned j = n % C;
        mT[n] = m[R * j + i];
    }

    return mT;
}

vector<float>
dot(const vector<float> &m1, const vector<float> &m2, const int m1_rows, const int m1_columns, const int m2_columns) {

    vector<float> output(m1_rows * m2_columns);


    #pragma omp parallel num_threads(threads)
    {
        #pragma omp for schedule(guided)
        for (int row = 0; row < m1_rows; ++row) {
            for (int col = 0; col < m2_columns; ++col) {
                output[row * m2_columns + col] = 0.f;
                for (int k = 0; k < m1_columns; ++k) {
                    output[row * m2_columns + col] += m1[row * m1_columns + k] * m2[k * m2_columns + col];
                }
            }
        }
    }

    return output;
}

vector <string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector <string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

template< class RandomIt >
void random_column_shuffle( RandomIt first, RandomIt last, int columns, int column)
{
    typename std::iterator_traits<RandomIt>::difference_type i, n;
    n = (last - first)/columns;
    for (i = n-1; i > 0; --i) {
        using std::swap;
        swap(first[i * columns + column], first[(std::rand() % (i+1)) * columns + column]);
    }
}

int main(int argc, const char *argv[]) { // argv = [file, batch, threads]

    string line;
    vector <string> line_v;
    int dataAmount, dataLength, categories;

    threads = atoi(argv[3]);
    int trainData = 375;

    //cout << "<OpenMP>\n\n";
    //cout << "Loading data ...\n";
    vector<float> X_train;
    vector<float> y_train;
    ifstream file_in("../all.data", fstream::in);
    if (file_in.is_open()) {
        file_in >> line;
        line_v = split(line, ',');
        dataAmount = stoi(line_v[0]);
        dataLength = stoi(line_v[1]) - 1;
        categories = stoi(line_v[2]);
        while (file_in >> line)) {
            line_v = split(line, ',');

            if (strcmp(line_v[0].c_str(), "M")){
              y_train.push_back(0.);
              y_train.push_back(1.);
            } else {
              y_train.push_back(1.);
              y_train.push_back(0.);
            }

            int size = static_cast<int>(line_v.size());
            for (unsigned i = 1; i < size; ++i) {
                X_train.push_back(strtof((line_v[i]).c_str(), 0));
            }

        }

        file_in.close();
    } else {
        cout << "Unable to open file" << '\n';
    }

    ifstream file(string(argv[1]));
    file >> line;
    line_v = split(line, ',');

    vector<int> dnn{ dataAmount, dataLength, stoi(line_v[0]), stoi(line_v[1]), categories };

    for (int i=0; i<dnn[1]; i++){
      float sum = 0, std = 0;
      for(int j = i; j < X_train.size(); j += dnn[1]){
        sum += X_train[j];
        std += X_train[j] * X_train[j];
      }
      float mean = sum / dnn[0];
      std = sqrt(std / dnn[0] - mean * mean);
      for(int j = i; j < X_train.size(); j += dnn[1]){
        X_train[j] = (X_train[j] - mean) / std;
      }
    }

    vector<float> _b_X(X_train.begin() + dnn[1]*trainData, X_train.end());
    vector<float> _b_y(y_train.begin() + dnn[4]*trainData, y_train.end());

    int xsize = static_cast<int>(X_train.size());
    int ysize = static_cast<int>(y_train.size());

    // Some hyperparameters for the NN
    int BATCH_SIZE = atoi(argv[2]);
    float lr = .001 / BATCH_SIZE;

    // Random initialization of the weights
    vector<float> W1 = random_vector(dnn[1] * dnn[2]);
    vector<float> W2 = random_vector(dnn[2] * dnn[3]);
    vector<float> W3 = random_vector(dnn[3] * dnn[4]);

    const auto t1 = chrono::high_resolution_clock::now();
    //cout << "Training the model ...\n";
    for (unsigned i = 0; i < 10000; ++i) {

        // Building batches of input variables (X) and labels (y)
        int randindx = rand() % (trainData - BATCH_SIZE);
        vector<float> b_X;
        vector<float> b_y;
        for (unsigned j = randindx * dnn[1]; j < (randindx + BATCH_SIZE) * dnn[1]; ++j) {
            b_X.push_back(X_train[j]);
        }
        for (unsigned k = randindx * dnn[4]; k < (randindx + BATCH_SIZE) * dnn[4]; ++k) {
            b_y.push_back(y_train[k]);
        }

        auto seed = unsigned ( time(0) );

        for (int j=0; j<dnn[4]; j++){
          srand ( seed );
          random_column_shuffle ( b_X.begin(), b_X.end(), dnn[4], j );
          srand ( seed );
          random_column_shuffle ( b_y.begin(), b_y.end(), dnn[4], j );
        }


        // Feed forward
        vector<float> a1 = relu(dot(b_X, W1, BATCH_SIZE, dnn[1], dnn[2]));
        vector<float> a2 = relu(dot(a1, W2, BATCH_SIZE, dnn[2], dnn[3]));
        vector<float> yhat = softmax(dot(a2, W3, BATCH_SIZE, dnn[3], dnn[4]), dnn[4]);

        // Back propagation
        vector<float> dyhat = (yhat - b_y);
        vector<float> dW3 = dot(transpose(&a2[0], BATCH_SIZE, dnn[3]), dyhat, dnn[3], BATCH_SIZE, dnn[4]);
        vector<float> dz2 = dot(dyhat, transpose(&W3[0], dnn[3], dnn[4]), BATCH_SIZE, dnn[4], dnn[3]) * reluPrime(a2);
        vector<float> dW2 = dot(transpose(&a1[0], BATCH_SIZE, dnn[2]), dz2, dnn[2], BATCH_SIZE, dnn[3]);
        vector<float> dz1 = dot(dz2, transpose(&W2[0], dnn[2], dnn[3]), BATCH_SIZE, dnn[3], dnn[2]) * reluPrime(a1);
        vector<float> dW1 = dot(transpose(&b_X[0], BATCH_SIZE, dnn[1]), dz1, dnn[1], BATCH_SIZE, dnn[2]);

        // Updating the parameters
        W3 = W3 - lr * dW3;
        W2 = W2 - lr * dW2;
        W1 = W1 - lr * dW1;


        if ((i + 1) % 10001 == 0) {
            cout << "------------------------------Epoch " << i + 1
                 << "------------------------------" << "\n";
            vector<float> _a1 = relu(dot(_b_X, W1, dnn[0] - trainData, dnn[1], dnn[2]));
            vector<float> _a2 = relu(dot(_a1, W2, dnn[0] - trainData, dnn[2], dnn[3]));
            vector<float> _yhat = softmax(dot(_a2, W3, dnn[0] - trainData, dnn[3], dnn[4]), dnn[4]);
            int randinx2 = (rand() % (dnn[0] - trainData - 11)) * dnn[4];
            vector<float> _yhat10(_yhat.begin() + randinx2, _yhat.begin() + randinx2 + 10 * dnn[4]);
            vector<float> _b_y10(_b_y.begin() + randinx2, _b_y.begin() + randinx2 + 10 * dnn[4]);

            print(_yhat, 5, dnn[4]);
            cout << "<..>\n";
            print(_yhat10, 10, dnn[4]);
            cout << "------------------------------\n";
            print(_b_y, 5, dnn[4]);
            cout << "<..>\n";
            print(_b_y10, 10, dnn[4]);
            vector<float> loss_m = yhat - b_y;
            float loss = 0.0;
            for (unsigned k = 0; k < BATCH_SIZE * dnn[4]; ++k) {
                loss += loss_m[k] * loss_m[k];
            }
            vector<float> _loss_m = _yhat - _b_y;
            float _loss = 0.0;
            for (unsigned k = 0; k < (dnn[0] - trainData) * dnn[4]; ++k) {
                _loss += _loss_m[k] * _loss_m[k];
            }

            const auto t2 = chrono::high_resolution_clock::now();
            chrono::duration<double, std::milli> fp_ms = t2 - t1;

            cout << "                              Loss " << loss / BATCH_SIZE << "\n";
            cout << "                             _Loss " << _loss / (dnn[0] - trainData) << "\n";
            cout << "                              Time " << fp_ms.count() / 1000.0 << " s\n";
        }
    }
    const auto t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> fp_ms = t2 - t1;
    cout << fp_ms.count() << endl;
    return 0;
}
