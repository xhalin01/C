#ifndef LEL_BMS_H
#define LEL_BMS_H

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <math.h>
#include <string>

using namespace std;

//storing of all packets which belongs to one pid
struct channel{
    vector <unsigned char *> packets;
    int pid;
};
extern vector<channel> channels;

//packets procesing functions
int getPid(unsigned char* packet);
unsigned char* getFile(char* file,long* buffSize);
void processPacket(unsigned char* packet);
bool channelExists(int pid);
long getProgramPackets(int pid);
bool pidInVector(int pid,vector<int> vec);
void printToFile(int packetNum,char *file);
int addPMTpackets(int pmtPid);

#endif //LEL_BMS_H
