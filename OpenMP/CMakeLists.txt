cmake_minimum_required(VERSION 3.11)

project(chpl_cpp)
set(CMAKE_CXX_STANDARD 11)

add_executable(chpl_cpp Source.cpp Neuron.cpp ArtificialNeuralNetwork.cpp)