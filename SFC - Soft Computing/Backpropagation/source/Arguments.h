//
// Created by Majkl on 2017-11-29.
//

#ifndef BACKPROP2_BACKPROP_H_H
#define BACKPROP2_BACKPROP_H_H
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////ARGUMETS
class Arguments{
public:
    Arguments(int argc,char** argv);
    void printHelp();
    std::string inputFile ="";
    std::string outputFile ="";
    bool inputSet;
    bool outputSet;
    double learningRate=0.15;
    double epsilon=0.01;
    double allowedTime=10;

    bool printFirstCycle=false;
    bool printLastCycle=false;
    bool printErrorOnly=false;

};






#endif //BACKPROP2_BACKPROP_H_H
