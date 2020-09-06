#ifndef LEL_SDT_H
#define LEL_SDT_H

#include <vector>
#include <cstring>
#include <cstdlib>

using namespace std;

//storing of SDT info
//service and provider name of according program with its pid
struct SDTstruct{
    int pid;
    unsigned char* service;
    unsigned char* provider;

};


//SDT processing funcions
void mergeSDTchunks();
void parseSDTchunk();
void parseSDT(unsigned char*packet);
void parseNameDescriptor(int pid, unsigned char *chunk);

extern vector<SDTstruct> SDT;

#endif //LEL_SDT_H
