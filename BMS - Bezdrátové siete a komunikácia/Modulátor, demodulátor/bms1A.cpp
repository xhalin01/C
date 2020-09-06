/* 
 * File:   bms1A.cpp
 * projekt: BMS1 AMPLITUDE SHIFT KEYING - MODULATION
 * login:  xhalin01
 */

#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <cassert>
#include "sndfile.hh"

#define SAMPLE_RATE 32000
#define CHANELS 1
#define FORMAT  (SF_FORMAT_WAV | SF_FORMAT_PCM_24)
#define AMPLITUDE  (1.0 * 0x7F000000)
#define carrierFreqvency  (1000.0)
using namespace std;

string getInputFromFile(char* file);
double getAmplitude(string tmp);

int main(int argc, char** argv) {

    //***************INPUT FROM FILE********************//
    string input=getInputFromFile(argv[1]);             //get input from file                                  
    int inputLength=input.length();                     //get input length

    //***************CARRIER SIGNAL*********************//
    int *carrierSignal = new int[SAMPLE_RATE];
    for (int i = 0; i < SAMPLE_RATE; i++){
        carrierSignal [i] = AMPLITUDE * sin(carrierFreqvency/SAMPLE_RATE* 2  * i * M_PI); //make carrier signal - 
    }                                                   //with freq 1000Hz and sample rate 32000

    //**************DATA SIGNAL*************************//
    double *dataSignal = new double[SAMPLE_RATE];
    int dataFreqvency=SAMPLE_RATE/35;                   //data signal freqvency = 914.2857Hz ~ 1828bit/s
    int dataPeriod=SAMPLE_RATE/dataFreqvency;           //data signal period = 35 samples
    for (int i = 0; i < SAMPLE_RATE; ++i){
        string tmp = string() + input[ i/dataPeriod*2 ] + input[ i/dataPeriod*2+1 ]; //take 2 bits from input for modulation
        dataSignal[i]=getAmplitude(tmp);                //make data signal for 2 bits, with max amplitude 1
    }

    //*************MODULATED SIGNAL*********************//
    int *modulatedSignal= new int[SAMPLE_RATE];          
    for (int i = 0; i < SAMPLE_RATE; ++i){                     
        modulatedSignal[i] = dataSignal[i] * carrierSignal[i]; //create modulated signal by multiplying carrier and data signal
        //printf("modulated[%d]=%d\n",i,modulatedSignal[i]);
    }

    //************WRITE RESULT TO FILE******************//
    SndfileHandle outputFile;
    string fileName=string(argv[1]).substr(0,string(argv[1]).length()-3)+"wav"; //create output file name
    outputFile=SndfileHandle(fileName, SFM_WRITE, FORMAT, CHANELS, SAMPLE_RATE); //create wav header
    outputFile.write(modulatedSignal, dataPeriod*inputLength/2); //write modulated signal to file divided by 2 because we're encoding -
                                                        // 2 bits into one period

    //***********FREE MEMORY****************************//
    delete [] modulatedSignal;    
    delete [] dataSignal;
    delete [] carrierSignal;
    return EXIT_SUCCESS;
}

//Function to read input from file
string getInputFromFile(char* file){
    FILE *fileHandle;
    char c;
    string input="00110011";
    fileHandle = fopen(file, "r");
    while((c=fgetc(fileHandle))!=EOF){
        if(c!='\n')
            input+=c;
    }
    return input;
}

//Sets aplitude for input bits
double getAmplitude(string tmp){
    if(tmp=="00"){  //if bits to encode are 00 then amplitude is 0.0*3 etc
        return 0;
    }
    else if(tmp=="01"){
        return 1.0/3;
    }
    else if(tmp=="10"){
        return 2.0/3;
    }
    return 3.0/3;
}