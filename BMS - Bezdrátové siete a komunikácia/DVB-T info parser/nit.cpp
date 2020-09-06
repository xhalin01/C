#include "nit.h"
NIT NITtable;

//Parsing the NIT table packets
void parseNIT(unsigned char* packet){
    NITtable.network_ID=packet[9]+(packet[8]*256);
    int network_descriptors_length=packet[9+5]+(packet[8+5]&0b00001111)*256;
    unsigned char* netDescriptors=(unsigned char*)malloc(network_descriptors_length+1);
    memcpy(netDescriptors,packet+15,network_descriptors_length);
    parseNetDescriptors(netDescriptors);
    //transport descriptors
    int transport_descriptors_length=packet[22+network_descriptors_length]+(packet[21+network_descriptors_length]&0b00001111)*256;
    unsigned char* transportDescriptors=(unsigned char*)malloc(transport_descriptors_length+1);
    memcpy(transportDescriptors,packet+15+network_descriptors_length+8,transport_descriptors_length); //8 loop bytes

    parseTransportDescriptors(transportDescriptors);
}

//Printing the NIT table acording to NIT packets
void printNIT(FILE * f){
    fprintf(f,"Network name: %s\n",NITtable.network_name);
    fprintf(f,"Network ID: %d\n",NITtable.network_ID);
    fprintf(f,"Bandwidth: %d",8-NITtable.bandwidth); fprintf(f," Mhz\n");

    fprintf(f,"Constellation: ");
    if(NITtable.constelation==0)  fprintf(f,"QPSK");
    else if(NITtable.constelation==1)  fprintf(f,"16-QAM");
    else if(NITtable.constelation==2)  fprintf(f,"64-QAM");
    fprintf(f,"\n");

    fprintf(f,"Guard interval: ");
    if(NITtable.guard_interval==0)  fprintf(f,"1/32");
    else if(NITtable.guard_interval==1)  fprintf(f,"1/16");
    else if(NITtable.guard_interval==2)  fprintf(f,"1/8");
    else if(NITtable.guard_interval==3)  fprintf(f,"1/4");
    fprintf(f,"\n");

    fprintf(f,"Code rate: ");
    if(NITtable.code_rate==0)  fprintf(f,"1/2");
    if(NITtable.code_rate==1)  fprintf(f,"2/3");
    if(NITtable.code_rate==2)  fprintf(f,"3/4");
    if(NITtable.code_rate==3)  fprintf(f,"5/6");
    if(NITtable.code_rate==4)  fprintf(f,"7/8");
    fprintf(f,"\n\n");
}

//Parsing the net descriptors in NIT table
void parseNetDescriptors(unsigned char* descriptors){
    bool nameFound= false;
    while (!nameFound){
        if(descriptors[0]==64){
            int name_lenght=descriptors[1];
            NITtable.network_name=(unsigned char*)malloc(name_lenght+1);
            descriptors+=2;
            memcpy(NITtable.network_name,descriptors,name_lenght);
            NITtable.network_name[name_lenght]='\0';
            nameFound=true;
        }
        else{
            descriptors+=descriptors[1];
            descriptors+=2; //tag and lenght bytes
        }
    }
}

//Parsing the transport descriptors in NIT table
void parseTransportDescriptors(unsigned char* descriptors) {
    bool tableFound= false;
    while (!tableFound){
        if(descriptors[0]==90){
            int bandwidth=(descriptors[6]&0b11100000)>>5;
            NITtable.bandwidth=bandwidth;
            int constelation=(descriptors[7]&0b11000000)>>6;
            NITtable.constelation=constelation;
            int codeRate=(descriptors[7]&0b00000111);
            NITtable.code_rate=codeRate;
            int guardInterval=(descriptors[8]&0b00011000)>>3;
            NITtable.guard_interval=guardInterval;
            tableFound= true;
        }
        else{
            descriptors+=descriptors[1];
            descriptors+=2; //tag and lenght bytes
        }
    }
}

//Calculating of stream bitrate
long calculateStreamBitrate(){
    long double A=54000000*((long double)188/204);

    long double bandwidth;
    switch(NITtable.bandwidth){
        case 0:
            bandwidth=1;
            break;
        case 1:
            bandwidth=(long double)7/8;
            break;
        case 2:
            bandwidth=(long double)6/8;
            break;
        case 3:
            bandwidth=(long double)5/8;
            break;
        default:
            bandwidth=1;
            break;
    }
    long double B=A*bandwidth;

    long double modulation;
    switch (NITtable.constelation){
        case 0:
            modulation=(long double)1/4;
            break;
        case 1:
            modulation=(long double)1/2;
            break;
        case 2:
            modulation=(long double)3/4;
            break;
    }

    long double C=modulation*B;

    double codeRate;
    switch(NITtable.code_rate){
        case 0:
            codeRate=(long double)1/2;
            break;
        case 1:
            codeRate=(long double)2/3;
            break;
        case 2:
            codeRate=(long double)3/4;
            break;
        case 3:
            codeRate=(long double)5/6;
            break;
        case 4:
            codeRate=(long double)7/8;
            break;
    }

    long double D=C*codeRate;

    long double guardInterval;
    switch(NITtable.guard_interval){
        case 0:
            guardInterval=(long double)32/33;
            break;
        case 1:
            guardInterval=(long double)16/17;
            break;
        case 2:
            guardInterval=(long double)8/9;
            break;
        case 3:
            guardInterval=(long double)4/5;
            break;
    }

    long E=(long)D*(long double)guardInterval;
    return E;
}