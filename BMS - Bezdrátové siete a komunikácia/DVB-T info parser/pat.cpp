#include "pat.h"

//unique the PAT vector
void makeUniquePAT(){
    auto comp = [] ( const PATstruct& lhs, const PATstruct& rhs ) {return lhs.programPid == rhs.programPid;};
    auto pred = []( const PATstruct& lhs, const PATstruct& rhs ) {return lhs.programPid < rhs.programPid;};
    std::sort(PAT.begin(),PAT.end(),pred);
    auto last = std::unique(PAT.begin(), PAT.end(),comp);
    PAT.erase(last, PAT.end());
}

//Pairing PAT packets with according PIDs for PMT sections
void parsePAT(unsigned char* packet){
    int length=packet[7];
    int pidsLenght=(length-9)/4;
    packet+=13;
    for (int i = 0; i < pidsLenght; ++i) {
        PATstruct pat;
        pat.programPid=packet[1]+packet[0]*256;
        pat.pmtPid=packet[3]+((packet[2]&0b00011111)*256);

        PAT.push_back(pat);
        if(i!=pidsLenght-1);
        packet+=4;
    }
}