//
// Created by user on 10/11/2019.
//

#ifndef LEL_NIT_H
#define LEL_NIT_H
#include <cstdio>
#include <cstring>
#include <cstdlib>
using namespace std;


//stuct for storing NIT info
struct NIT{
    unsigned char* network_name;
    int network_ID;
    int bandwidth;
    int constelation;
    int code_rate;
    int guard_interval;
};

//NIT processing functions

void printNIT(FILE*out);
void parseNIT(unsigned char* packet);
void parseNetDescriptors(unsigned char* descriptors);
void parseTransportDescriptors(unsigned char* descriptors);
long calculateStreamBitrate();


#endif //LEL_NIT_H
