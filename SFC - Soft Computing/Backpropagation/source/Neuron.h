//
// Created by Majkl on 2017-11-29.
//

#ifndef BACKPROP2_NEURON_H
#define BACKPROP2_NEURON_H

#include <vector>

class Neuron{
public:
    Neuron(int index,int outputs,double lr);
    void setOut(double out);
    double getOut();
    std::vector<double> myWeights;
    void calculateOutput(std::vector<Neuron> neurons);
    double activationFunction(double sum);
    double activationFunctionDerivation(double d);
    void calculateOutputLayerGradient(double target);
    void calculateHiddenLayerGradient(std::vector<Neuron> neurons);
    double gradientWeightsSum(std::vector<Neuron> neurons);
    void updateWeight(std::vector<Neuron> &neurons);
    double learnRate = 0.15;


private:
    int myIndex;
    double myOut;
    double myGradient;


};


#endif //BACKPROP2_NEURON_H
