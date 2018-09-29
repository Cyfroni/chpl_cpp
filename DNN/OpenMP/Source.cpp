#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <fstream>
#include <iomanip>
#include "ArtificialNeuralNetwork.hpp"

using namespace std;

//excel parameters
/*const int excel_lines = 110527;
const int all_test_lines = 70000;
const int input = 8;
const string file("KaggleV2-May-2016.csv");
vector<int> lines_to_ignore = { 1, 2, 4, 5, 7 };
const int target = -1; // last*/

const int excel_lines = 1309;
const int all_test_lines = 900;
const int input = 6;
const string file("titanic_dataset.csv");
vector<int> lines_to_ignore = {3, 8};
const int target = 1;

//
const int size = input + lines_to_ignore.size() + 1;
int xxx[] = {input, 100, 100, 100, 2};

void test_excel(string);

double test_patients(ArtificialNeuralNetwork *, vector<double> *, const int, const int, const int, int *, const int = 0,
                     bool = false, bool = true, bool = false);

int main() {
    std::cout << "START\n";

    srand((unsigned int) time(NULL));

    std::string path("Excel/");

    /*ArtificialNeuralNetwork ANN("igtest/before/");
    double tab[4] = { 1.0, 2.0, 3.0, 4.0 };
    std::cout<<ANN.computeResult(tab)<<std::endl;
    std::cout << ANN.computeLoss(0) << std::endl;
    ANN.computeGradient();
    ANN.learn();
    ANN.writeAll("igtest/after/");*/

    test_excel(path);

    while (1);
    return 0;
}

void test_excel(string path) {
    int test_lines;
    int iterations;
    int verify_lines;
    bool evenly;
    int decay;

    fstream excel;
    string line, line2, result_line;
    vector<double> tab[input + 1];
    string data[input];
    double mean[input] = {0};
    double std[input] = {0};
    int shuffled_lines[excel_lines];


    excel.open(file, ios::in);
    getline(excel, line);
    for (int i = 0; i < excel_lines; i++) {
        shuffled_lines[i] = i;

        getline(excel, line);
        replace(line.begin(), line.end(), ' ', '#');
        replace(line.begin(), line.end(), ',', ' ');

        istringstream iss(line);

        int k = 0;
        for (int j = 1; j <= size; j++) {
            iss >> line;
            if (line[0] == '"') {
                line2 = line;
                while (line2[line2.length() - 1] != '"') {
                    iss >> line2;
                    line += line2;
                }

            }
            if (j == target || (j == size && target == -1)) result_line = line;
            else if (std::find(lines_to_ignore.begin(), lines_to_ignore.end(), j) == lines_to_ignore.end()) {
                data[k++] = line;
            }
        }

        setInit(Xavier);
        setUpd(Vanilla);
        setActiv(ReLU);
        setLoss(SVM);
        if (file == "KaggleV2-May-2016.csv") {
            tab[0].push_back(data[0] == "F" ? 1.0 : 0.0);        //Gender
            tab[1].push_back(stod(data[1]));                    //Age
            tab[2].push_back(stod(data[2]));                    //Scholarship
            tab[3].push_back(stod(data[3]));                    //Hipertension
            tab[4].push_back(stod(data[4]));                    //Diabetes
            tab[5].push_back(stod(data[5]));                    //Alcoholism
            tab[6].push_back(stod(data[6]));                    //Handcap
            tab[7].push_back(stod(data[7]));                    //SMS_received
            tab[8].push_back(result_line == "No" ? 0.0 : 1.0);    //No-show

            setLambda(1e-6);
            setRatio(1e-3);

            test_lines = 20;
            iterations = 100;
            verify_lines = 1000;
            evenly = true;
            decay = 0;
        } else if (file == "titanic_dataset.csv") {
            tab[0].push_back(stod(data[0]));                    //pclass
            tab[1].push_back(data[0] == "female" ? 1.0 : 0.0);    //sex
            tab[2].push_back(stod(data[2]));                    //age
            tab[3].push_back(stod(data[3]));                    //sibsp
            tab[4].push_back(stod(data[4]));                    //parch
            tab[5].push_back(stod(data[5]));                    //fare
            tab[6].push_back(stod(result_line));                //survived

            setLambda(1e-10);
            setRatio(7e-4);

            test_lines = all_test_lines;
            iterations = 50;
            verify_lines = excel_lines;
            evenly = true;
            decay = 0;
        } else {
            cout << "UNKNOWN FILE\n";
            return;
        }

        for (int j = 0; j < input; j++) {
            mean[j] += tab[j][i];
            std[j] += tab[j][i] * tab[j][i];
        }
    }
    excel.close();

    for (int i = 0; i < input; i++) {
        mean[i] /= excel_lines;
        std[i] = sqrt(std[i] / (excel_lines) - mean[i] * mean[i]);
    }

    for (int i = 0; i < excel_lines; i++) {
        for (int j = 0; j < input; j++) {
            tab[j][i] -= mean[j];
            tab[j][i] /= (std[j] == 0.0 ? 1.0 : std[j]);
        }
    }

    double before, after, yy;
    bool full_learn = false;
    bool full_test = false;
    string info = "00";
    ArtificialNeuralNetwork ANN(sizeof(xxx) / sizeof(xxx[0]), xxx);

    while (1) {
        if (verify_lines > excel_lines - all_test_lines) verify_lines = excel_lines - all_test_lines;
        if (test_lines > all_test_lines) test_lines = all_test_lines;

        if (info[0] == '1') ANN.create(sizeof(xxx) / sizeof(xxx[0]), xxx);
        if (info[1] == '1') random_shuffle(&shuffled_lines[0], &shuffled_lines[all_test_lines]);
        else if (info[1] == '2') random_shuffle(&shuffled_lines[all_test_lines], &shuffled_lines[excel_lines]);
        else if (info[1] == '3') random_shuffle(&shuffled_lines[0], &shuffled_lines[excel_lines]);


        if (full_test) {
            before = test_patients(&ANN, tab, all_test_lines, excel_lines - all_test_lines, 0, shuffled_lines);
            if (full_learn) {
                test_patients(&ANN, tab, 0, all_test_lines, iterations, shuffled_lines, decay, true, true, evenly);
                after = test_patients(&ANN, tab, all_test_lines, excel_lines - all_test_lines, 0, shuffled_lines);

                cout << "<<<<< " << before << ", " << after << " >>>>> ( " << after - before << " )\n\n";
                full_learn = false;
            }
            full_test = false;
        } else {
            ANN.writeAll(path + "backup/");
            before = test_patients(&ANN, tab, test_lines, verify_lines, 0, shuffled_lines);
            test_patients(&ANN, tab, 0, test_lines, iterations, shuffled_lines, decay, true, true, evenly);
            after = test_patients(&ANN, tab, test_lines, verify_lines, 0, shuffled_lines);

            cout << "<<<<< " << before << ", " << after << " >>>>> ( " << after - before << " )\n\n";
            endN();
            endANN();
        }
        while (1) {
            cout
                    << "NEW SCHUFF ACTIV INIT UPDATE LOSS / EDIT(-) / BACKUP(b) / SAVE(s/[0-9]s) / LOAD(l/[0-9]l) / FULL(f/lf) / RESET(r) / DIVIDE((r,l, )/)\n";
            getline(cin, info);
            if (info.length() > 0) {
                if (info[info.length() - 1] == 'b') {
                    ANN.readAll(path + "backup/");
                } else if (info[info.length() - 1] == 's') {
                    if (info.length() == 2)
                        ANN.writeAll(path + "saved/" + info[info.length() - 2] + "/");
                    else ANN.writeAll(path);
                } else if (info[info.length() - 1] == 'l') {
                    if (info.length() == 2 && info[info.length() - 2] == 'b') ANN.readAll(path + "best/");
                    else if (info.length() == 2) ANN.readAll(path + "saved/" + info[info.length() - 2] + "/");
                    else ANN.readAll();
                } else if (info[info.length() - 1] == 'f') {
                    full_test = true;
                    if (info.length() > 1 && info[info.length() - 2] == 'l') full_learn = true;
                    break;
                } else if (info[info.length() - 1] == '-') {
                    cout << "test_lines iterations verify_lines ratio lambda decay evenly\n"
                         << test_lines << " " << iterations << " " << verify_lines << " " << getRatio() << " "
                         << getLambda() << " " << decay << " " << evenly << "\n";
                    getline(cin, info);
                    istringstream iss(info);
                    if (!iss.eof()) iss >> test_lines;
                    if (!iss.eof()) iss >> iterations;
                    if (!iss.eof()) iss >> verify_lines;
                    if (!iss.eof()) {
                        iss >> yy;
                        setRatio(yy);
                    }
                    if (!iss.eof()) {
                        iss >> yy;
                        setLambda(yy);
                    }
                    if (!iss.eof()) iss >> decay;
                    if (!iss.eof()) iss >> evenly;
                } else if (info[info.length() - 1] == '/') {
                    if (info.length() > 1) {
                        if (info[info.length() - 2] == 'r') setRatio(getRatio() / 2.0);
                        else if (info[info.length() - 2] == 'l') setLambda(getLambda() / 2.0);
                    } else {
                        setRatio(getRatio() / 2.0);
                        setLambda(getLambda() / 2.0);
                    }
                } else if (info[info.length() - 1] == 'r') {
                    ANN.reset();
                } else break;
            } else break;
        }
        if (info.length() > 6 || info.length() < 2) info = "00";
        else {
            if (info.length() > 2) setActiv((ActivationFunction)(info[2] - '0'));
            if (info.length() > 3) setInit((InitializationFunction)(info[3] - '0'));
            if (info.length() > 4) setUpd((UpdateFunction)(info[4] - '0'));
            if (info.length() > 5) setLoss((LossFunction)(info[5] - '0'));
        }
    }
}

double test_patients(ArtificialNeuralNetwork *A, vector<double> *vec, const int test_begin, const int test_lines,
                     const int iterations, int *shuffled_lines, const int decay, bool learning, bool writing,
                     bool evenly) {
    long double loss;
    int correct;
    int answers[3][2]; // [0,1,-1][correct/all] - czegolny przypadek (dla 2 klas wyjsciowych)

    double best = 0.0;
    double worst = 100.0;
    double x;
    double progress = 0.0;

    double patient[input];
    int result;

    if (learning) cout << "\nLEARNING.. lines: " << test_lines << " (can be less because of evenly flag)\n\n";
    else cout << "\nVERIFYING.. lines: " << test_lines << "\n\n";

    int rtl = test_lines;
    double pratio = getRatio();
    double plambda = getLambda();
    for (int i = 0; i <= iterations; i++) {
        correct = 0;
        loss = 0.0;
        answers[0][0] = answers[0][1] = answers[1][1] = answers[1][0] = answers[2][1] = 0;
        int out;
        int ig = 0;
        for (int j = test_begin; j < test_begin + test_lines; j++) {
            if (evenly && j + ig == all_test_lines) {
                rtl = j - test_begin + 1;
                break;
            }

            for (int p = 0; p < input; p++)
                patient[p] = vec[p][shuffled_lines[j + ig]];
            result = (int) vec[input][shuffled_lines[j + ig]];
            answers[result][1]++;
            if (evenly && answers[result][1] > answers[(result == 0 ? 1 : 0)][1] + 1) {
                answers[result][1]--;
                ig++;
                j--;
                continue;
            }
            out = (*A).computeResult(patient);
            if (result == out) {
                correct++;
                answers[result][0]++;
            } else if (out == -1)
                answers[2][1]++;
            loss += (*A).computeLoss(result);

            if (learning) {
                (*A).computeGradient();
                (*A).learn();
            }
        }

        if (decay != 0 && (i + 1) % decay == 0) {
            setRatio(getRatio() / 2.0);
            setLambda(getLambda() / 2.0);
        }

        x = 100.0 * correct / rtl;

        if (writing) {

            cout << i << "# " << (double) x << "% poprawnych (avg loss = ";
            cout << setprecision(10);
            cout << (x == 100.0 ? 0.0 : (double) (loss / (rtl)));
            cout << ") 0: " << answers[0][0] << "/" << answers[0][1];
            if (answers[0][1])cout << " (" << 100.0 * answers[0][0] / answers[0][1] << "%)";
            cout << " 1: " << answers[1][0] << "/" << answers[1][1];
            if (answers[1][1])cout << " (" << 100.0 * answers[1][0] / answers[1][1] << "%)";
            cout << " -1: " << answers[2][1] << "/" << rtl - correct;
            if (rtl - correct)cout << " (" << 100.0 * answers[2][1] / (rtl - correct) << "%)";
            cout << endl;
        }
        if (i == 0)
            progress = (x == 100.0 ? 0.0 : loss / (rtl));
        if (best < x) {
            best = x;
            //if (learning) (*A).writeAll("Excel/best/");
        }
        if (worst > x) worst = x;


    }
    setRatio(pratio);
    setLambda(plambda);

    if (learning) cout << "loss < " << progress << ", " << (x == 100.0 ? 0.0 : loss / (rtl)) << " > : ";
    cout << "fit < " << worst << ", " << best << " >%\n";

    return best;
}