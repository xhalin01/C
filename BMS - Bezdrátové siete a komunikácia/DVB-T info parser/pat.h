//
// Created by user on 10/11/2019.
//

#ifndef LEL_PAT_H
#define LEL_PAT_H

using namespace std;

#include <vector>
#include <algorithm>

//storing of PAT info
//program pid - pid of one program
//pmt pid - pid of packet where pmt info is stored
struct PATstruct{
    int programPid;
    int pmtPid;
};

//PAT processing funcions
extern vector<PATstruct> PAT;
void makeUniquePAT();
void parsePAT(unsigned char* packet);


#endif //LEL_PAT_H
