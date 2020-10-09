//
// Created by Majkl on 2017-11-29.
//
#include <iostream>
#include <time.h>
#include <ctime>
#include "Network.h"
#include <stdio.h>
#include <stdlib.h>

Network::Network(Input myInput) {
    in=myInput.input;
    target=myInput.output;
    printLastCycle=myInput.printLastCycle;
    printFirstCycle=myInput.printFirstCycle;
    printErrorOnly=myInput.printErrorOnly;
    allowedTime=myInput.allowedTime;
    epsilon=myInput.epsilon;

    if(myInput.outputFile==""){
        outputFile=stdout;
    }
    else{
        outputFile=fopen(myInput.outputFile.c_str(),"w");
        if(outputFile==NULL){
            printf("error opening output file\n");
            exit(2);
        }
    }

    for (int i = 0; i < myInput.topology.size(); ++i) {
        myLayers.push_back(std::vector<Neuron>());
        int outputs;
        if (i + 1 == myInput.topology.size()) {
            outputs = 0;
        }
        else {
            outputs=myInput.topology[i+1];
        }

        for (int j = 0; j <= myInput.topology[i]; ++j) {
            myLayers.back().push_back(Neuron(j,outputs,myInput.learningRate));
        }

        myLayers.back()[myInput.topology[i]].setOut(1);
    }
}

void Network::run() {

    double E;
    double Ep;

    clock_t begin = clock();
    clock_t  end;
    double time;

    int y=1;
    do{
        E=0;
        for (int i = 0; i < in.size()-1; ++i) {
            //////calculate output
            for (int j = 0; j < myLayers.size(); ++j) {
                if(j==0){                                       //first input
                    for (int k = 0; k <myLayers[j].size()-1; ++k) {
                        myLayers[j][k].setOut(in[i][k]);
                    }
                }
                else{
                    for (int k = 0; k <myLayers[j].size()-1; ++k) {
                        myLayers[j][k].calculateOutput(myLayers[j-1]);
                    }
                }
            }

            ///////get results
            out.push_back(std::vector<double> ());
            for (int l = 0; l < myLayers.back().size()-1; ++l) {
                double d=myLayers.back()[l].getOut();
                out.back().push_back(d);
                //fprintf(outputFile,"input: %lf,%lf,result:%lf, target:%lf\n",in[i][0],in[i][1],d,target[i][0]);
            }

            //////error
            Ep=0;
            double delta=0;
            for (int m = 0; m < myLayers.back().size()-1; ++m) {
                delta=target[i][m]-myLayers.back()[m].getOut();
                Ep+=(delta * delta)*0.5;
                //printf("Ep=%lf,target=%lf,getOut=%lf\n",Ep,target[i][m],myLayers.back()[m].getOut());
                ////////output layer gradient
                myLayers.back()[m].calculateOutputLayerGradient(target[i][m]);
            }

            E+=Ep;
            ///////hidden layer gradients
            for (int n = myLayers.size()-2; n > 0; --n) {
                for (int j = 0; j < myLayers[n].size(); ++j) {
                    myLayers[n][j].calculateHiddenLayerGradient(myLayers[n+1]);
                }
            }

            ///////update weights
            for (int n = myLayers.size()-1; n > 0; --n) {
                for (int j = 0; j < myLayers[n].size()-1; ++j) {
                    std::vector<Neuron> &prev = myLayers[n-1];
                    myLayers[n][j].updateWeight(prev);
                }
            }
        }
        //////////print result

        if(!printErrorOnly){
            if(printFirstCycle && y==1){
                printResult();
            }
            else if(printLastCycle && ((E<epsilon) or (int)time > allowedTime)){
                printResult();
            }
            else if(!printFirstCycle && !printLastCycle){
                printResult();
            }
        }

        end = clock();
        time = double(end - begin) / CLOCKS_PER_SEC;
        printf("Cycle %d. E=%.4lf\n",y++,E);

    }while((E>epsilon) and (int)time <= allowedTime);

}

void Network::printResult() {
    for (int i = 0; i < in.size()-1; ++i) {
        double error=0;
        fprintf(outputFile,"input: ");
        for (int x = 0; x < in[i].size(); ++x) {
            fprintf(outputFile,"%.3lf ",in[i][x]);
        }
        //fprintf(outputFile,"\n");

        fprintf(outputFile,"target: ");
        for (int x = 0; x < target[i].size(); ++x) {
            fprintf(outputFile,"%.3lf ",target[i][x]);
        }
        //fprintf(outputFile,"\n");

        fprintf(outputFile,"result: ");
        for (int x = 0; x < out[i].size(); ++x) {
            fprintf(outputFile,"%.3lf ",out[i][x]);
            error+=target[i][x]-out[i][x];
        }
        //fprintf(outputFile,"\n");

        if(error==-0){
            error=0;
        }
        fprintf(outputFile,"error: ");
        fprintf(outputFile,"%.3lf ",error);

        fprintf(outputFile,"\n");

        //fprintf(outputFile,"%d input: %lf,%lf,result:%lf, target:%lf\n",i,in[i][0],in[i][1],out[i][0],target[i][0]);
    }

}
