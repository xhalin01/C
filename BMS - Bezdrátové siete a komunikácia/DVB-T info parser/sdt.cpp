//
// Created by user on 10/11/2019.
//
#include "sdt.h"

//variables for SDT processing - merging of SDT sections

vector<unsigned char*> sdt;
unsigned char* SDTchunk;
bool stop=false;
bool start=true;
int totalSDTlenght=0;
int actualSDTlenght=0;
int remainingSDTlenght=0;

//merge SDT packets
void mergeSDTchunks(){
    int sdtLenght=0;
    for (int k = 0; k < sdt.size(); ++k) {
        sdtLenght+=strlen((const char*)sdt[k]);
    }
    SDTchunk=(unsigned char*)malloc(sdtLenght);
    int index=0;
    for (int l = 0; l < sdt.size(); ++l) {
        memcpy(SDTchunk+index,sdt[l],strlen((const char*)sdt[l]));
        index+=strlen((const char*)sdt[l]);
    }
}

//Parsing of SDT packets
void parseSDT(unsigned char*packet){
    int payloadStart=(packet[1]&0b01000000)>>6;
    if(!stop){
        if(payloadStart==false && start==true){
            //skip packet
        }
        else if(payloadStart==true && start==true){
            //add to SDT without header, get the lenght;
            totalSDTlenght=packet[7]+((packet[6]&0b00001111)*256)-4-4;
            int len=188-16;
            unsigned char*temp=(unsigned char*)malloc(len+1);
            memcpy(temp,packet+16,len);
            temp[len]='\0';
            start= false;
            actualSDTlenght+=len;
            remainingSDTlenght=totalSDTlenght-actualSDTlenght;
            sdt.push_back(temp);
        }
        else if(payloadStart== true && start==false){
            stop= true;
            //stop storing SDT packets, all sections saved
        }
        else if(payloadStart== false && start== false){
            //get according amount of chunk
            //add to SDT
            int len=0;
            if(remainingSDTlenght>184){
                len=184;
            }
            else{
                len=totalSDTlenght-actualSDTlenght;
            }
            unsigned char*temp=(unsigned char*)malloc(len+1);

            memcpy(temp,packet+4,len);
            temp[len]='\0';
            sdt.push_back(temp);
            actualSDTlenght+=len;
            remainingSDTlenght-=len;
        }
    }
}

//Parsing of SDT sections
void parseSDTchunk(){
    int remain=totalSDTlenght-4; //-4 crc bits
    while(remain>1){
        SDTstruct tmp;
        tmp.pid=SDTchunk[1]+(SDTchunk[0]*256);

        int len=((SDTchunk[3]&0b00001111)*256)+SDTchunk[4];

        unsigned char* chunk=(unsigned char*)malloc(len);
        memcpy(chunk,SDTchunk+5,len);
        parseNameDescriptor(tmp.pid,chunk);

        SDTchunk+=5;  // shift head!!!
        SDTchunk+=len; // shift descriptors!!!
        remain-=len+5;
    }
}

//Parsing provider and service descriptors in SDT packets
void parseNameDescriptor(int pid, unsigned char *chunk){
    SDTstruct tmp;
    tmp.pid=pid;
    int service_provider_name_length=chunk[3];

    unsigned char* provider=(unsigned char*)malloc(service_provider_name_length+1);
    memcpy(provider,chunk+4,service_provider_name_length);
    provider[service_provider_name_length]='\0';
    tmp.provider=provider;
    int service_name_length=chunk[4+service_provider_name_length];

    unsigned char* service=(unsigned char*)malloc(service_name_length+1);
    memcpy(service,chunk+5+service_provider_name_length,service_name_length);
    service[service_name_length]='\0';

    tmp.service=service;
    SDT.push_back(tmp);
}