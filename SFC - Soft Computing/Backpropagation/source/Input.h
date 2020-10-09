//
// Created by Majkl on 2017-11-29.
//

#ifndef BACKPROP2_INPUT_H_H
#define BACKPROP2_INPUT_H_H
#include <iostream>
#include "Arguments.h"
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <stdio.h>

class Input{
public:
    Input(Arguments args);
    std::string outputFile;
    double learningRate;
    double allowedTime;
    double epsilon;

    bool printFirstCycle;
    bool printLastCycle;
    bool printErrorOnly;


    std::vector<int> topology;
    std::vector<std::vector<double> > input;
    std::vector<std::vector<double> > output;

};

#endif //BACKPROP2_INPUT_H_H
