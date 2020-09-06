#ifndef LEL_PMT_H
#define LEL_PMT_H

#include <vector>
#include <algorithm>

using namespace std;

//struct for storing PMT info
//programPid - pid of program
//pids - vector of pids which belongs to one program
struct PMTstruct{
    vector<int> pids;
    int programPid;
};


//PMT processing functions
extern vector<PMTstruct> PMT;
void makeUniquePMT();
void parsePMT();
int iteratePMT(int pid);
vector<int> parsePMTpacket(unsigned char* buff);
vector<int> parseLoop(unsigned char* loop,int len);
void eraseNullProgram();

#endif //LEL_PMT_H
