//
// Created by Majkl on 2017-11-29.
//

#ifndef BACKPROP2_NETWORK_H
#define BACKPROP2_NETWORK_H

#include "Neuron.h"
#include "Input.h"
#include <vector>
#include <stdio.h>

class Network{
public:
    Network(Input myInput);
    void run();
    FILE * outputFile;
    void printResult();
    bool printFirstCycle;
    bool printLastCycle;
    bool printErrorOnly;

    double epsilon;
    double allowedTime;

private:
    std::vector<std::vector<Neuron> > myLayers;
    std::vector<std::vector<double> > in;
    std::vector<std::vector<double> > target;
    std::vector<std::vector<double> > out;

};

#endif //BACKPROP2_NETWORK_H
