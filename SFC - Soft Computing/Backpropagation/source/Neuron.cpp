//
// Created by Majkl on 2017-11-29.
//

#include "Neuron.h"
#include <cmath>
#include <cstdlib>
#include <time.h>

void Neuron::calculateOutput(std::vector<Neuron> neurons){
    double sum=0;
    for (int i = 0; i < neurons.size(); ++i) {

        sum+=neurons[i].myWeights[myIndex]*neurons[i].getOut();

    }

    myOut=activationFunction(sum);

}

double Neuron::getOut() {
    return myOut;
}

void Neuron::setOut(double out){
    myOut = out;
}

Neuron::Neuron(int index, int outputs,double lr) {
    myIndex=index;
    learnRate=lr;
    srand(time(NULL));
    for (int i = 0; i < outputs; ++i) {
        myWeights.push_back(rand() / double(RAND_MAX)-0.5);
    }
}

double Neuron::activationFunction(double sum) {
    return tanh(sum);
}

double Neuron::activationFunctionDerivation(double d) {
    return 1.0 - (d * d);
}

void Neuron::calculateOutputLayerGradient(double target) {
    myGradient = (target - myOut) * activationFunctionDerivation(myOut);
}

void Neuron::calculateHiddenLayerGradient(std::vector<Neuron> neurons) {
    myGradient = gradientWeightsSum(neurons) * activationFunctionDerivation(myOut);
}

double Neuron::gradientWeightsSum(std::vector<Neuron> neurons) {
    double summary=0;

    for (int i = 0; i < neurons.size()-1; ++i) {
        summary+=neurons[i].myGradient*myWeights[i];
    }
    return summary;
}

void Neuron::updateWeight(std::vector<Neuron> &neurons) {
    for (unsigned n = 0; n < neurons.size(); ++n) {
        Neuron &neuron = neurons[n];
        double newDeltaWeight = neuron.learnRate * neuron.getOut() * (myGradient);
        neuron.myWeights[myIndex]=newDeltaWeight+neuron.myWeights[myIndex];
    }
}

