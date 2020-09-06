/* 
 * File:   bms1B.cpp
 * projekt: BMS1 AMPLITUDE SHIFT KEYING - DEMODULATION
 * login:  xhalin01
 */

#include <cstdlib>
#include <math.h>
#include <string>
#include <stdio.h>
#include <limits.h>
#include <iostream>
#include "sndfile.hh"

static const double AMPLITUDE = 0x7f000000;
using namespace std;

int findMaxAmplitude(int * tmpPeriod,int periodLength);
string bitsFromAmplitude(int maxAmplitude);

int main(int argc, char** argv) {
    
    //**********************READ FILE TO BUFFER**************//
    SndfileHandle inputFile;
    int sampleRate;
    int *buffer;
    
    inputFile = SndfileHandle(argv[1]);
    sampleRate = inputFile.samplerate();                     //get sample rate of file
    buffer = new int[sampleRate];                            //reserve memmory for signal
    inputFile.read(buffer, sampleRate);                      //read file to buffer
    //*********************GET PERIOD LENGTH*****************//
    int i=0;
    int periodLength=0;
    while(buffer[i]==0){                                     //read lenght of first two synchronization bits(00),
        periodLength++;                                      //if buffer[i] is not zero then the second period starts(11) and cycle ends,
        i++;                                                 //which gives us the length of one period
    }

    if(buffer[periodLength*2-1]==0){                         //even-numbered period correction(could start with 0 value in buffer, so previous 
        periodLength--;                                      //period detection is not correct, we need to look at end of second period (with measured period value)
    }                                                        //and check if we are still in second(not zero value) or third period(zero value in buffer provided
                                                             //synchronization bits))

    //*********************GET LENGHT OF SIGNAL**************//
    int lastSignal=0;
    for (int i = 0; i < sampleRate; ++i){                    //iterate trough whole buffer
        if(buffer[i]!=0){                                    //find last occurence of not zero sample
            lastSignal=i;
        }
    }
    int signalLength=lastSignal+periodLength+1;              //add one ending period(00) and 1 as buffer index starts from 0
    //********************DEMODULATE SIGNAL TO DATA**********//
    string bitsToWrite="";
    int currentPeriod=0;
    int * tmpPeriod = new int[periodLength];
    
    for (int i = 0; i < signalLength/periodLength; ++i){     //iterate trough each period
        for (int j = 0; j < periodLength; ++j){
            tmpPeriod[j]=buffer[j+(i*periodLength)];         //fill temp buffer with period samples
        }
        currentPeriod++;
        int maxAmplitude=findMaxAmplitude(tmpPeriod,periodLength); //find max amplitude in current period
        string demodulatedBits=bitsFromAmplitude(maxAmplitude);    //get data bits according to amplitude 
        bitsToWrite+=demodulatedBits;                        //add decoded bits to string, which will be written to file
    }

    //****************WRITE DEMODULATED DATA TO FILE*********//
    bitsToWrite=bitsToWrite.substr(8,bitsToWrite.length());  //remove first 8 synchronization bits
    FILE *fileHandle;
    string fileName=string(argv[1]).substr(0,string(argv[1]).length()-3)+"txt"; //make file name from program argument
    fileHandle = fopen(fileName.c_str(),"w");                //create file handle
    if (fileHandle == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(fileHandle, "%s\n", bitsToWrite.c_str());        //write decoded bits to file
    fclose(fileHandle);

    //***********************FREE MEMMORY********************//
    delete [] buffer;
    return EXIT_SUCCESS;
}

//function to find maximum amplitude in one period
int findMaxAmplitude(int * tmpPeriod,int periodLength){
    int max=INT_MIN;
    for (int i = 0; i < periodLength; ++i){
        if(tmpPeriod[i]>max){
            max=tmpPeriod[i];
        }
    }
    return max;
}

//function to get data bits acording to amplitude
string bitsFromAmplitude(int maxAmplitude){
    if(maxAmplitude==0){
        return "00";
    }
    else if(maxAmplitude>0 && maxAmplitude<=AMPLITUDE/3*1){
        return "01";
    }
    else if(maxAmplitude>AMPLITUDE/3*1 && maxAmplitude<=AMPLITUDE/3*2){
        return "10";
    }
    return "11";
}