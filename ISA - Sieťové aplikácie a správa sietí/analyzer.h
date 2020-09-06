#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
using namespace std;


//pomocne struktury
struct portData
{
	int port;
	int data1;
	int data2;
};

struct address
{
	unsigned char * addr;
	int data1;
	int data2;
};

struct ethHeader
{
	unsigned char * source;
	unsigned char * destination;
	unsigned char * type;
};

struct ipv4Header
{
	unsigned char * data;
	unsigned short totalLength;
	unsigned char * source;
	unsigned char * destination;
	unsigned char * protocol;
};

struct ipv6Header
{
	unsigned char * data;
	unsigned short * source;
    unsigned short * destination;
	unsigned char * protocol;
};

struct udpHeader
{
	unsigned char * source;
	unsigned char * destination;
	unsigned char * length;
};

struct tcpHeader
{
	unsigned char * source;
	unsigned char * destination;
	int length;
};


unsigned char * getGlobalHeader(unsigned char * buffer);
unsigned char * getNextHeader(unsigned char * buffer);
unsigned char * getPacket(unsigned char * buffer,int size);

ethHeader getEthHeader(unsigned char * buffer);
ipv6Header getIpv6Header(unsigned char * buffer);
ipv4Header getIpv4Header(unsigned char * buffer);
udpHeader getUdpHeader(unsigned char * buffer);
tcpHeader getTcpHeader(unsigned char * buffer);

int getSizeOfPacket(unsigned char * buffer);

//kontroluju ci sa hodnota adresy v hlavicke zhoduje s hodnotou vo vstupe
bool checkMacAdress(ethHeader head,int * macArg,int sflag,int dflag);
bool checkIpv4Adress(ipv4Header head,int * ipv4Arg,int sflag,int dflag);
bool checkIpv6Adress(ipv6Header head,int * ipv6Arg,int sflag,int dflag);
bool checkTcpPort(tcpHeader head,int port,int sflag,int dflag);
bool checkUdpPort(udpHeader head,int port,int sflag,int dflag);

//iteruju vektor a hladaju v nom poziciu struktury
int iterateMacVector(vector<address> vec,unsigned char * buffer);
int iterateIpv4Vector(vector<address> vec,unsigned char * buffer);
int iterateDataVector(vector<portData> vec,int port);

//vyberaju strukturu s najvacsimi hodnotami z danych vektorov
int pickNextAdress(vector<address> v);
int pickNextPortData(vector<portData> v);

//pomocne funkcie na allokaciu
unsigned char * allocCharBuffer(unsigned char * buffer,int n);
unsigned short * allocShortBuffer(unsigned short * buffer,int n);