#include "bms.h"
#include "nit.h"
#include "sdt.h"
#include "pat.h"
#include "pmt.h"

//vectors for storing sdt,pat,pmt sections
vector<SDTstruct> SDT;
vector<PATstruct> PAT;
vector<PMTstruct> PMT;
//vector for storing all packets with certain PID
vector<channel> channels;

int main(int argc, char*argv[]){
    long buffSize;
    unsigned char *buff = getFile((char*)argv[1],&buffSize); //store file to buffer
    unsigned char *buffPointer=buff; //save pointer for later free()

    //process buffer of packets
    long packetNum=buffSize/188;
    for (long i = 0; i < packetNum; ++i) {
        processPacket(buff);
        if(i!=packetNum-1)
            buff+=188;
    }

    //sort channels according to pid
    sort(channels.begin(), channels.end());

    //make unique PAT packets
    makeUniquePAT();

    //make unique PMT packets
    makeUniquePMT();
    parsePMT();

    //SDT packets MERGING
    mergeSDTchunks();

    //parsing of STD merged sections
    parseSDTchunk();

    //erase program with pid 0 - null program
    eraseNullProgram();

    //print results to file
    printToFile(packetNum,(char*)argv[1]);

    //free buffer
    free(buffPointer);
    return 0;
}

//print results to file
void printToFile(int packetNum,char *file){
    FILE * out;
    string str(file);
    int pos=str.find(".");
    string name=str.substr(0,pos);
    name=name+".txt";
    out=fopen(name.c_str(),"w");

    printNIT(out);
    long streamBitrate=calculateStreamBitrate();
    for (int j = 0; j < SDT.size(); ++j) {
        long programPackets=getProgramPackets(SDT[j].pid);
        unsigned char pat[2];
        for (int i = 0; i < PAT.size(); ++i) {
            if(PAT[i].programPid==SDT[j].pid){
                pat[0]=PAT[j].pmtPid>>8;
                pat[1]=PAT[i].pmtPid&0b0000000011111111;
                programPackets+=addPMTpackets(PAT[i].pmtPid);
            }
        }
        long double rate=((((long double)programPackets/packetNum))*streamBitrate)/1000000;
        fprintf(out,"0x%.2x%.2x-%s-%s: %.2Lf Mbps\n",pat[0],pat[1],SDT[j].provider,SDT[j].service,rate);
        //printf("0x%.2x%.2x-%s-%s: %.2Lf Mbps\n",pat[0],pat[1],SDT[j].provider,SDT[j].service,rate);
    }
    fclose(out);
}

//add PMT packets to sum of all packets,
//for bitrate computing
int addPMTpackets(int pmtPid){
    int packets=0;
    for (int j = 0; j < channels.size(); ++j) {
        if(pmtPid==channels[j].pid){
            packets+=channels[j].packets.size();
        }
    }
    return packets;
}


//Sum the number of packets which belongs to one program(for later bitrate calculation)
//Audio + video + pmt and other packets
long getProgramPackets(int pid){
    long packets=0;
    vector<int> pids;

    for (int i = 0; i < PMT.size(); ++i) {
        if(PMT[i].programPid==pid){
            pids=PMT[i].pids;    
        }
    }


    for (int j = 0; j < channels.size(); ++j) {
        if(pidInVector(channels[j].pid,pids)){
            packets+=channels[j].packets.size();
        }
    }
    
    return packets;
}

//iterate vector for pid
bool pidInVector(int pid,vector<int> vec){
    for (int i = 0; i < vec.size(); ++i) {
        if(vec[i]==pid)
            return true;
    }
    return false;
}

//get PID of packet
int getPid(unsigned char* packet){
    char mask1=0b00011111;
    return packet[2]+(packet[1]&mask1)*256;
}

//packet processing function
//parsing packet according to its pid number
void processPacket(unsigned char* packet){
    int AFlenght=0;
    int pid=getPid(packet);
    unsigned char *tmp=packet;
    unsigned char * payload = (unsigned char*)malloc(184-AFlenght+1);
    payload[184-AFlenght]='\0';
    packet+=4+AFlenght;
    memcpy(payload,packet,184-AFlenght);
    packet=tmp;

    if(pid==16){         //NIT Table
        parseNIT(packet);
    }

    if(pid==0){          //PAT table
        parsePAT(packet);
    }

    if(pid==17){         //SDT table
        parseSDT(packet);
    }

    //SAVE PACKET TO VECTOR
    //every packet with same pid goes to one vector with that pid number
    if(!channelExists(pid)){ //add to vector
        channel tmp;
        tmp.pid=pid;
        unsigned char* newPacket=(unsigned char*)malloc(189);
        memcpy(newPacket,packet,188);
        newPacket[188]='\0';
        tmp.packets.push_back(newPacket);
        channels.push_back(tmp);
    }
    else{                    //create new vector with certain pid
        int index=-1;
        for(std::vector<channel>::size_type i = 0; i != channels.size(); i++){
            if(channels[i].pid==pid)
                index=i;
        }
        unsigned char* newPacket=(unsigned char*)malloc(189);
        memcpy(newPacket,packet,188);
        newPacket[188]='\0';
        channels[index].packets.push_back(newPacket);
    }
}

//Iterate vector and search for pid
bool channelExists(int pid){
    for(std::vector<channel>::size_type i = 0; i != channels.size(); i++){
        if(channels[i].pid==pid)
            return true;
    }
    return false;
}

//Read file to buffer
unsigned char* getFile(char* file,long *buffSize){
    FILE * pFile;
    long lSize;
    unsigned char * buffer;
    long result;

    //otvorenie suboru a citanie do buffra
    pFile = fopen ( file , "rb" );
    if (pFile==NULL){
        fprintf(stderr, "Nepodarilo sa otvorit subor %s\n",file);
        exit(1);
    }
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);
    buffer = (unsigned char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL){
        fputs ("Nedostatocna pamat",stderr);
        exit(1);
    }
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize){
        fprintf(stderr, "Chyba pri citani suboru\n");
        exit(1);
    }
    *buffSize=result;
    return buffer;
}

bool operator<(const channel& a, const channel& b){
    return a.pid < b.pid;
}
