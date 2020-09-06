#include "pmt.h"
#include "pat.h"
#include "bms.h"

//unique the PMT vector
void makeUniquePMT(){
    auto comp2 = [] ( const PMTstruct& lhs, const PMTstruct& rhs ) {return lhs.programPid == rhs.programPid;};
    auto pred2 = []( const PMTstruct& lhs, const PMTstruct& rhs ) {return lhs.programPid < rhs.programPid;};
    std::sort(PMT.begin(),PMT.end(),pred2);
    auto last2 = std::unique(PMT.begin(), PMT.end(),comp2);
    PMT.erase(last2, PMT.end());
}

//Parsing of PMT packets, creating PMT vector
void parsePMT(){
    for (int i = 0; i < PAT.size(); ++i) {
        for (int j = 0; j < channels.size(); ++j) {
            if(PAT[i].pmtPid==channels[j].pid){
                for (int k = 0; k < channels[j].packets.size(); ++k) {
                    PMTstruct tmp;
                    tmp.programPid = PAT[i].programPid;
                    unsigned char* packet=(unsigned char *)malloc(188);
                    memcpy(packet,channels[j].packets[k],188);
                    tmp.pids= parsePMTpacket(packet);
                    //tmp.pids.push_back(channels[j].pid);
                    //add PMT struct from one PMT table to vector of program map tables
                    PMT.push_back(tmp);
                }
            }
        }
    }
}

//Parse PMT packet for program map section
vector<int> parsePMTpacket(unsigned char* packet){
    int section_length=packet[7];
    int program_info=packet[11+5];
    int loop_lenght=section_length-9-program_info-4;

    //move in packet to loop with PIDs
    packet+=16+1+program_info;

    unsigned char* loop=(unsigned char*)malloc(loop_lenght);
    memcpy(loop,packet,loop_lenght);
    //parse loop for elemetary PIDs
    vector<int> v=parseLoop(loop,loop_lenght);

    return v;
}

//Parse descriptors loop for pid
vector<int> parseLoop(unsigned char* loop,int len){
    int index=0;
    vector<int> elemetaryPIDs;
    int elemPid;
    int infoLen;
    while (index<len){
        elemPid=loop[2]+((loop[1]&0b00011111)*256);

        elemetaryPIDs.push_back(elemPid);
        infoLen=loop[4];
        loop+=infoLen+5;
        index+=infoLen+5;
    }
    return elemetaryPIDs;
}

//erassing program with pid 0
void eraseNullProgram(){
    for (int f = 0; f < PAT.size(); ++f) {
        if(PAT[f].programPid==0){
            PAT.erase(PAT.begin()+f);
        }
    }
}