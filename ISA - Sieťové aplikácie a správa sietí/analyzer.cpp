#include "analyzer.h"
using namespace std;

int main (int argc, char * argv[]) 
{
    int j=0;
    int iflag=0,fflag=0,vflag=0,sflag=0,dflag=0;
    char * file;
    char * filter;
    char * value;
    

    //premenne pre nacitanie vstupu
    int macArg[6];
    int ipv4Arg[4];
    int ipv6Arg[8];
    int ipv6Arg2[6];
    int port=-1;

    
    int count=0;
    int result1;
    unsigned int result2;
    unsigned int length;
    bool top10=false;

    
    while(argv[j]!=NULL)                   //spracovanie argumentov
    {
    	if((strcmp(argv[j],"-i")==0))
    	{
    		iflag=1;
    		j++;
    		file=strdup(argv[j]);
    	}
    	if((strcmp(argv[j],"-f"))==0)
    	{
    		fflag=1;
    		j++;
    		filter=strdup(argv[j]);
    	}
    	if((strcmp(argv[j],"-v"))==0)
    	{
    		vflag=1;
    		j++;
    		value=strdup(argv[j]);
    	}
    	if((strcmp(argv[j],"-s"))==0)
    	{
    		sflag=1;
    	}
    	if((strcmp(argv[j],"-d"))==0)
    	{
    		dflag=1;
    	}
    	j++;
    }

    if(!(iflag) || !(fflag) || !(vflag) || !(sflag || dflag))
    {
    	fprintf(stderr, "Zle zadane argumenty! (-i subor,-f filter,-v hodnota, -s/-d) \n");
    	return 1;
    }

    if(strcmp(filter,"ipv4")==0)             //ked bol zadany ipv4 filter pokracuj na formatovane citanie
    {
    	if (strcmp(value,"top10")==0)
    	{
    		top10=true;
    	}
    	else
    	{
    		for (int i = 0; i < 4; ++i)
    		{
    			ipv4Arg[i]=0;
    		}
    		if(sscanf (value,"%d.%d.%d.%d",&ipv4Arg[0],&ipv4Arg[1],&ipv4Arg[2],&ipv4Arg[3])!=4) //formatovane citanie argv do pola integerov
    		{
    			fprintf(stderr, "Wrong ipv4 adress\n");
    			return 1;
    		}
    	}
    }
    if(strcmp(filter,"mac")==0)
    {
    	
    	if (strcmp(value,"top10")==0)
    	{
    		top10=true;
    	}
    	else
    	{
    		for (int i = 0; i < 6; ++i)
    		{
    			macArg[i]=0;
    		}
    		if(sscanf (value,"%x:%x:%x:%x:%x:%x",&macArg[0],&macArg[1],&macArg[2],&macArg[3],&macArg[4],&macArg[5])!=6) //formatovane citanie vstupu
    		{
    			fprintf(stderr, "Wrong mac adress\n");
    			return 1;
    		}
    	}
    }
    if(strcmp(filter,"tcp")==0)
    {
    	if (strcmp(value,"top10")==0)
    	{
    		top10=true;
    	}
    	else
    	{
    		port=atoi(value);
    	}
    }

    if(strcmp(filter,"udp")==0)
    {
    	if (strcmp(value,"top10")==0)
    	{
    		top10=true;
    	}
    	else
    	{
    		port=atoi(value);
    	}
    }

   
    if(strcmp(filter,"ipv6")==0)
    {
    	if (strcmp(value,"top10")==0)
    	{
    		top10=true;
    	}
    	else
    	{	
    		for (int i = 0; i < 8; ++i)
    		{
    			ipv6Arg[i]=0;
    		}
    		for (int i = 0; i < 6; ++i)
    		{
    			ipv6Arg[i]=0;
    		}
    		 //formatovane citanie vstupu
    		result1=sscanf(value,"%x:%x:%x:%x:%x:%x:%x:%x",&ipv6Arg[0],&ipv6Arg[1],&ipv6Arg[2],&ipv6Arg[3],&ipv6Arg[4],&ipv6Arg[5],&ipv6Arg[6],&ipv6Arg[7]);
    		//ak nenacital 8 cisel,tak citaj druhu polovicu adresy za "::"
    		if(result1!=8)
    		{
    			length=strlen(value);
    			int index=0;

    			for (unsigned int i = 0; i < strlen(value); ++i)
    			{
    				if(value[i]==':')
    				{
    					count++;
    				}
    				if(count==result1)
    				{
    					index=i;
    					break;
    				}
    			}

    			if(result1>8|| result1==0)
    			{
    				fprintf(stderr, "error in ipv6 parsing\n");
    				return 1;
    			}
    			else if(value[index+1]!=':')
    			{
    				fprintf(stderr, "error in ipv6 parsing\n");
    				return 1;
    			}
    			else
    			{
    				result2=sscanf((&(value[index]))+2,"%x:%x:%x:%x:%x:%x:%x:%x",&ipv6Arg2[0],&ipv6Arg2[1],&ipv6Arg2[2],&ipv6Arg2[3],&ipv6Arg2[4],&ipv6Arg2[5],&ipv6Arg2[6],&ipv6Arg2[7]);
    			}
    			
    			if(result2>6 || result2==0 || result1+result2>7)
    			{
    				fprintf(stderr, "error in ipv6 parsing\n");
    				return 1;
    			}

    			for(unsigned int i=0;i<result2;i++)
    			{
    				ipv6Arg[8-result2+i]=ipv6Arg2[i]; //spoj dve nacitane casti adresy
    			}    	
    		}
    	}
    		
    }	

    FILE * pFile;
	long lSize;
	unsigned char * buffer;
	long result;

	//otvorenie suboru a citanie do buffra
	pFile = fopen ( file , "rb" );
	if (pFile==NULL)
	{
		fprintf(stderr, "Nepodarilo sa otvorit subor\n");
		return 1;
	}

	fseek (pFile , 0 , SEEK_END);
	lSize = ftell (pFile);
	rewind (pFile);

	buffer = (unsigned char*) malloc (sizeof(char)*lSize);
	if (buffer == NULL)
	{
		fputs ("Nedostatocna pamat",stderr);
		return 1;
	}


	result = fread (buffer,1,lSize,pFile);
	if (result != lSize)
	{
		fprintf(stderr, "Chyba pri citani suboru\n");
		return 1;
	}	
	
	unsigned char * header;
	unsigned char * packet;
	int packetSize;
	
	ethHeader ethHead;
	ipv4Header ipv4Head;
	ipv6Header ipv6Head;
	tcpHeader tcpHead;
	udpHeader udpHead;

	int padding;
	int i=0;
	
	//velkosti hlaviciek
	int L2=0;
	int L3=0;
	int L4=0;
	int L5=0;

	//vysledne hodnoty
	int value1=0;
	int value2=0;

	//velkosti hlaviciek ukladane do vektorov
	int vectorL2=0;
	int vectorL3=0;
	int vectorL4=0;
	int vectorL5=0;

	//vektory pre ulozenie struktur
	vector<address> v1;
	vector<portData> portData1;

	//pomocne premenne pre pocitanie podla konkretneho filtra
	int indx=-1;
	bool ipv6Bool=false;
	bool ipv4Bool=false;
	bool udpBool=false;
	bool tcpBool=true;

	buffer+=24; //Posuneme sa o global Header
	while(buffer[0]!='\0')
	{
  		//vynulovanie vsetkych pomocnych premennych
  		ipv6Bool=false;
	    ipv4Bool=false;
	    udpBool=false;
	    tcpBool=false;
  		padding=0;
  		i++;
  		L2=0;
  		L3=0;
	    L4=0;
	    L5=0;

	    vectorL2=0;
	    vectorL3=0;
	    vectorL4=0;
	    vectorL5=0;


  		header=getNextHeader(buffer);         //ziskanie pcap hlavicky
  		buffer+=16;
  		

  		packetSize=getSizeOfPacket(header);  //ziskanie velkosti paketu
  		packet=getPacket(buffer,packetSize); //ziskanie obsahu paketu
  		ethHead=getEthHeader(packet);        //ziskanie ehernetovej hlavicky

  		vectorL2=packetSize;                 //pocitanie dat pre vector
  		vectorL3=packetSize-14;				 //pocitanie dat pre vector
  		
  		if(checkMacAdress(ethHead,macArg,sflag,dflag)) //ked mac adressa v pakete vyhovuje vstupu tak pocitaj velkost
		{
			L2=packetSize;
			L3=packetSize-14;
		}
  		packet+=14;
  		
  		if(ethHead.type[0]==134 && ethHead.type[1]==221)  //86dd hexadecimal ipv6 identification - ipv6 paket
		{
			
			ipv6Bool=true;
			ipv6Head=getIpv6Header(packet);
			if(checkIpv6Adress(ipv6Head,ipv6Arg,sflag,dflag))  //ked ipv6 adressa v pakete vyhovuje vstupu tak pocitaj velkost
			{
				L2=packetSize;
				L4=packetSize-40-14;
			}
			vectorL4=packetSize-40-14;
			packet+=40;
			if(ipv6Head.protocol[0]==6 && strcmp(filter,"tcp")==0)        //tpc protocol 
			{
				tcpBool=true;
				tcpHead=getTcpHeader(packet);
				if(checkTcpPort(tcpHead,port,sflag,dflag))				  //ked tcp port vyhovuje vstupu pocitaj data
				{
					L2=packetSize;
					L5=packetSize-40-14-tcpHead.length;
				}
				vectorL5=packetSize-40-14-tcpHead.length;
			}
			if(ipv6Head.protocol[0]==17 && strcmp(filter,"udp")==0)       //udp protocol 
			{
				udpBool=true;
				udpHead=getUdpHeader(packet);
				if(checkUdpPort(udpHead,port,sflag,dflag))                //ked udp port vyhovuje vstupu pocitaj data
				{
					L2=packetSize;
					L5=packetSize-40-14-8;
				}
				vectorL5=packetSize-40-14-8;
			}

		}

		else if(ethHead.type[0]==8 && ethHead.type[1]==0)      //0800 hexadecimal ipv4 identification - ipv4 paket
		{
			
			ipv4Bool=true;
			ipv4Head=getIpv4Header(packet);
			padding=packetSize-14-ipv4Head.totalLength;
			if(padding>0 && (checkMacAdress(ethHead,macArg,sflag,dflag)))
			{
				L3=packetSize-14-padding;
			}
			if(padding>0)
			{
				vectorL3-=padding;
			}
			if(checkIpv4Adress(ipv4Head,ipv4Arg,sflag,dflag))                        //ked adresa v pakete vyhovuje vstupu pocitaj data
			{
				L2=packetSize;
				L4=packetSize-20-14;
				
				if(padding>0)
				{
					L4-=padding;
				}
			}
			vectorL4=packetSize-20-14;
			if(padding>0)
			{
				vectorL4-=padding;
			}	
			packet+=20;
			if(ipv4Head.protocol[0]==6 && strcmp(filter,"tcp")==0)                   //tpc protocol
			{
				tcpBool=true;
				tcpHead=getTcpHeader(packet);
				if(checkTcpPort(tcpHead,port,sflag,dflag))
				{
					L2=packetSize;
					L5=packetSize-20-14-tcpHead.length;
					if(padding>0)
					{	
						L5-=padding;
						vectorL5-=padding;
					}
				}
				vectorL5=packetSize-20-14-tcpHead.length;
				if(padding>0)
				{	
					vectorL5-=padding;
				}
			}
			if(ipv4Head.protocol[0]==17 && strcmp(filter,"udp")==0)                  //udp protocol
			{
				udpBool=true;
				udpHead=getUdpHeader(packet);
				if(checkUdpPort(udpHead,port,sflag,dflag))
				{
					L2=packetSize;
					L5=packetSize-20-14-8;
					if(padding>0)
					{
						vectorL5-=padding;
					}
				}
				vectorL5=packetSize-20-14-8;
			}
		}
		else if(ethHead.type[0]==8 && ethHead.type[1]==6)   //0806 ARP RARP
		{
			if(checkMacAdress(ethHead,macArg,sflag,dflag))  //ak vyhovovala mac adressa paketu vstupu odcitaj ARP data z druhej hodnoty 
			{
				L3-=18;
			}
			vectorL3-=18;
		}

  		buffer+=packetSize;
  		if(top10)             //ak bol zadany filter top10 vytvaraj vektory struktur a ukladaj hodnoty
  		{
  			if(strcmp(filter,"mac")==0) //top 10 pre mac
			{
				if(sflag)										//ak bol zadany parameter -s
				{
					if(iterateMacVector(v1,ethHead.source)>=0)  //ak vektor existuje pripis nove data
					{
						indx=iterateMacVector(v1,ethHead.source);
						v1[indx].data1+=vectorL2;
						v1[indx].data2+=vectorL3;
					}
					else									    //ak neexistuje vytvor ho a pripis aktualne data
					{
						address a;
						a.addr=allocCharBuffer(ethHead.source,7);
						a.data1=vectorL2;
						a.data2=vectorL3;
						v1.push_back(a);
					}
				}
				if(dflag)										//ak bol zadany parameter -d
				{
					if(iterateMacVector(v1,ethHead.destination)>=0)
					{
						indx=iterateMacVector(v1,ethHead.destination);
						v1[indx].data1+=vectorL2;
						v1[indx].data2+=vectorL3;
					}
					else
					{
						address a;
						a.addr=allocCharBuffer(ethHead.destination,7);
						a.data1=vectorL2;
						a.data2=vectorL3;
						v1.push_back(a);
					}
				}
				
			}
			if(strcmp(filter,"ipv4")==0 && ipv4Bool) //top 10 pre ipv4
			{
				if(sflag)
				{

					if(iterateIpv4Vector(v1,ipv4Head.source)>=0)
					{
						indx=iterateIpv4Vector(v1,ipv4Head.source);
						v1[indx].data1+=vectorL2;
						v1[indx].data2+=vectorL4;
					}
					else
					{
						address a;
						a.addr=allocCharBuffer(ipv4Head.source,5);
						
						
						a.data1=vectorL2;
						a.data2=vectorL4;
						v1.push_back(a);
					}
				}
				if(dflag)
				{
					if(iterateIpv4Vector(v1,ipv4Head.destination)>=0)
					{
						indx=iterateIpv4Vector(v1,ipv4Head.destination);
						v1[indx].data1+=vectorL2;
						v1[indx].data2+=vectorL4;
					}
					else
					{
						address a;
						a.addr=allocCharBuffer(ipv4Head.destination,5);
						a.data1=vectorL2;
						a.data2=vectorL4;
						v1.push_back(a);
					}
				}
			}
			if(strcmp(filter,"udp")==0 && udpBool) //top 10 pre udp
			{
				if(sflag)
				{
					
					short int temp;
					temp=(unsigned short)udpHead.source[0]<<8 | udpHead.source[1];					
					if(iterateDataVector(portData1,temp)>=0)
					{
						indx=iterateDataVector(portData1,temp);
						portData1[indx].data1+=vectorL2;
						portData1[indx].data2+=vectorL5;
					}
					else
					{
						portData a;
						a.port=temp;
						a.data1=vectorL2;
						a.data2=vectorL5;
						portData1.push_back(a);
					}	
				}
				if(dflag)
				{
					short int temp1;
					temp1=(unsigned short)udpHead.destination[0]<<8 | udpHead.destination[1];
					if(iterateDataVector(portData1,temp1)>=0)
					{
						indx=iterateDataVector(portData1,temp1);
						portData1[indx].data1+=vectorL2;
						portData1[indx].data2+=vectorL4;
					}
					else
					{
						portData a;
						a.port=temp1;
						a.data1=vectorL2;
						a.data2=vectorL4;
						portData1.push_back(a);
						
					}
				}
			}
			if(strcmp(filter,"tcp")==0 && tcpBool) 		//top 10 pre tcp
			{
				if(sflag)
				{
					short int temp;
					temp=(unsigned short)tcpHead.source[0]<<8 | tcpHead.source[1];					
					if(iterateDataVector(portData1,temp)>=0)
					{
						indx=iterateDataVector(portData1,temp);
						portData1[indx].data1+=vectorL2;
						portData1[indx].data2+=vectorL5;
					}
					else
					{
						portData a;
						a.port=temp;
						a.data1=vectorL2;
						a.data2=vectorL5;
						portData1.push_back(a);
					}
				}
				if(dflag)
				{
					short int temp;
					temp=(unsigned short)tcpHead.destination[0]<<8 | tcpHead.destination[1];					
					if(iterateDataVector(portData1,temp)>=0)
					{
						indx=iterateDataVector(portData1,temp);
						portData1[indx].data1+=vectorL2;
						portData1[indx].data2+=vectorL5;
					}
					else
					{
						portData a;
						a.port=temp;
						a.data1=vectorL2;
						a.data2=vectorL5;
						portData1.push_back(a);	
					}
				}
			}
  		}
  		else  //ak nebol zadany filter top,pripocitavaj podla 
  		{
  			value1+=L2;
  			if(strcmp(filter,"mac")==0)
			{
  				value2+=L3;
			}
			if(strcmp(filter,"ipv4")==0 || strcmp(filter,"ipv6")==0)
			{
				value2+=L4;
			}
			if(strcmp(filter,"udp")==0 || strcmp(filter,"tcp")==0)
			{
				value2+=L5;
			}
		}
  			
	}

	if(top10)    //vypis hodnot pre filter top10
	{
    	if(strcmp(filter,"mac")==0)
    	{
			int i;
			for (int x = 0; x < 10; ++x)
			{
				if((i=pickNextAdress(v1))!=-1)
				{
					for (int j = 0; j < 6; ++j)
    				{
    					if(j!=5)
    						printf("%02x:",v1[i].addr[j]);
    					else
    						printf("%02x",v1[i].addr[j]);
    				}
    				printf(" %d %d\n",v1[i].data1,v1[i].data2);
    				v1.erase(v1.begin() + i);
				}
			}
    	}
    	if(strcmp(filter,"ipv4")==0)
    	{
    		int i;
			for (int x = 0; x < 10; ++x)
			{
				if((i=pickNextAdress(v1))!=-1)
				{
					for (int j = 0; j < 4; ++j)
    				{
    					if(j!=3)
    						printf("%d.",v1[i].addr[j]);
    					else
    						printf("%d",v1[i].addr[j]);
    				}
    				printf(" %d %d\n",v1[i].data1,v1[i].data2);
    				v1.erase(v1.begin() + i);
				}
			}
   	    }
   	    if(strcmp(filter,"udp")==0 || strcmp(filter,"tcp")==0) 
   	    {
   	    	int i;
			for (int x = 0; x < 10; ++x)
			{
				if((i=pickNextPortData(portData1))!=-1)
				{
					printf("%d ",portData1[i].port);
    				printf("%d %d\n",portData1[i].data1,portData1[i].data2);
    				portData1.erase(portData1.begin() + i);
				}
			}
   	    }
	}
	else //vypis hodnot pre ostatne filtre
	{
		printf("%d %d\n",value1,value2);
	}
    
    fclose (pFile);
    return 0;

}

//získa globalnu hlavicku z buffera
unsigned char * getGlobalHeader(unsigned char * buffer)
{
	unsigned char *globalHeader=(unsigned char*)malloc(24);
  	memcpy (globalHeader,buffer,24);
  	return globalHeader;
}

//získa dalsiu hlavicku paketu z buffra
unsigned char * getNextHeader(unsigned char * buffer)
{
	unsigned char *header=(unsigned char*)malloc(16);
  	memcpy (header,buffer,16);
  	return header;
}

//získa paket z buffra
unsigned char * getPacket(unsigned char * buffer,int size)
{
	unsigned char *packet=(unsigned char*)malloc(size);
	memcpy(packet,buffer,size);
	return packet;
}


//allokuje miesto pre unsigned char buffer a prida null terminator na koniec
unsigned char * allocCharBuffer(unsigned char * buffer,int n)
{
	unsigned char * temp=(unsigned char*)malloc(n);
	memcpy(temp,buffer,(n-1));
	temp[n-1]='\0';
	return temp;
}

//allokuje miesto pre unsigned short buffer a prida null terminator na koniec
unsigned short * allocShortBuffer(unsigned short * buffer,int n)
{
	unsigned short * temp=(unsigned short*)malloc(n);
	memcpy(temp,buffer,(n-1));
	temp[n-1]='\0';
	return temp;
}

//získa eth hlavicku a ulozi ju do struktury
ethHeader getEthHeader(unsigned char * buffer)
{
	ethHeader head;

	head.destination=allocCharBuffer(buffer,7);
	buffer+=6;
	
	head.source=allocCharBuffer(buffer,7);
	buffer+=6;
	
	head.type=allocCharBuffer(buffer,3);
	buffer+=2;

	return head;
}

//získa ipv6 hlavicku a ulozi ju do struktury
ipv6Header getIpv6Header(unsigned char * buffer)
{
	ipv6Header head;
	head.data=allocCharBuffer(buffer,9);
	buffer+=6;
	head.protocol=allocCharBuffer(buffer,2);
	buffer+=2;
	head.source=(unsigned short*)malloc(9);
	for (int i = 0; i < 8; ++i)
	{
		head.source[i] = ((unsigned short)(*(buffer+i*2)))<<8 | (*(buffer+i*2+1));  
	}
	head.source[8]='\0';

	unsigned short s1[8];
	for (int i = 0; i < 8; ++i)
	{
		s1[i]=head.source[i];
	}
	buffer+=16;

	head.destination=(unsigned short*)malloc(9);
	for (int i = 0; i < 8; ++i)
	{
		head.destination[i] =((unsigned short)(*(buffer+i*2)))<<8 | (*(buffer+i*2+1));  
	}
	head.destination[8]='\0';

	for (int i = 0; i < 8; ++i)
	{
		head.source[i]=s1[i];
	}
	buffer+=16;

	return head;
}


//získa ipv4 hlavicku a ulozi ju do struktury
ipv4Header getIpv4Header(unsigned char * buffer)
{
	ipv4Header head;

	head.data=allocCharBuffer(buffer,13);
	buffer+=2;
	unsigned char* len=allocCharBuffer(buffer,3);
	buffer+=7;
	head.totalLength=(unsigned short)(len[0]<<8 | len[1]);
	head.protocol=allocCharBuffer(buffer,2);
	buffer+=3;
	head.source=allocCharBuffer(buffer,5);
	buffer+=4;
	head.destination=allocCharBuffer(buffer,5);
	buffer+=4;

	return head;
}


//získa udp hlavicku a ulozi ju do struktury
udpHeader getUdpHeader(unsigned char * buffer)
{
	udpHeader head;

	head.source=allocCharBuffer(buffer,3);
	buffer+=2;
	head.destination=allocCharBuffer(buffer,3);
	buffer+=2;
	head.length=allocCharBuffer(buffer,3);
	buffer+=2;

	return head;
}

//získa tcp hlavicku a ulozi ju do struktury
tcpHeader getTcpHeader(unsigned char * buffer)
{
	tcpHeader head;
	
	head.source=allocCharBuffer(buffer,3);
	buffer+=2;
	head.destination=allocCharBuffer(buffer,3);
	buffer+=2;
	buffer+=8;
	unsigned char *temp=allocCharBuffer(buffer,2);
	head.length=temp[0]/4;
	buffer+=1;
	return head;
}

//získa velkost paketu v Bytoch
int getSizeOfPacket(unsigned char * buffer)
{
	buffer+=8;
	char *size=NULL;
	for (int j = 0; j < 4; ++j)
	{
		if(size==NULL){
			size=(char*)malloc(1);
			memcpy(size,buffer,1);
			buffer++;
		}
		else{
			size=(char*)realloc(size,j+1);
			memcpy(size+j,buffer,1);
			buffer++;
		}
	}
  	int x = *(int *)size;
	return x;
}

//Skontroluje ci sa zadana adresa zhoduje s adresou v pakete
bool checkMacAdress(ethHeader head,int * macArg,int sflag,int dflag)
{
	bool check1=false;
	bool check2=false;
	if(sflag)
	{
		check1=true;
		for (int i = 0; i < 6; ++i)
		{
			if(head.source[i]!=macArg[i])
			{
				check1=false;
			}
		}
	}
	if(dflag)
	{
		check2=true;
		for (int i = 0; i < 6; ++i)
		{
			if(head.destination[i]!=macArg[i])
			{
				check2=false;
			}
		}
	}
	return (check1 || check2);
}

//Skontroluje ci sa zadana adresa zhoduje s adresou v pakete
bool checkIpv4Adress(ipv4Header head,int * ipv4Arg,int sflag,int dflag)
{
	bool check1=false;
	bool check2=false;
	if(sflag)
	{
		check1=true;
		for (int i = 0; i < 4; ++i)
		{
			if(head.source[i]!=ipv4Arg[i])
			{
				check1=false;
			}
		}
	}
	if(dflag)
	{
		check2=true;
		for (int i = 0; i < 4; ++i)
		{
			if(head.destination[i]!=ipv4Arg[i])
			{
				check2=false;
			}
		}
	}
	return (check1 || check2);
}

//Skontroluje ci sa zadana adresa zhoduje s adresou v pakete
bool checkIpv6Adress(ipv6Header head,int * ipv6Arg,int sflag,int dflag)
{
	bool check1=false;
	bool check2=false;
	if(sflag)
	{
		check1=true;
		for (int i = 0; i < 8; ++i)
		{
			if(head.source[i]!=ipv6Arg[i])
			{
				check1=false;
			}
		}
	}
	if(dflag)
	{
		check2=true;
		for (int i = 0; i < 8; ++i)
		{
			if(head.destination[i]!=ipv6Arg[i])
			{
				check2=false;
			}
		}
	}
	return (check1 || check2);
}

//Skontroluje ci sa zadany port zhoduje s adresou v pakete
bool checkTcpPort(tcpHeader head,int port,int sflag,int dflag)
{
	bool check1=false;
	bool check2=false;

	short src=((unsigned short)((head.source[0])))<<8 | ((head.source[1]));
	short dst=((unsigned short)((head.destination[0])))<<8 | ((head.destination[1]));
	if(sflag){
		if(src==port){
			check1=true;
		}
	}
	if(dflag){
		if(dst==port){
			check2=true;
		}
	}
	return (check1 || check2);
}

//Skontroluje ci sa zadany port s adresou v pakete
bool checkUdpPort(udpHeader head,int port,int sflag,int dflag)
{
	bool check1=false;
	bool check2=false;

	short src=((unsigned short)((head.source[0])))<<8 | ((head.source[1]));
	short dst=((unsigned short)((head.destination[0])))<<8 | ((head.destination[1]));

	if(sflag){
		if(src==port){
			check1=true;
		}
	}
	if(dflag){
		if(dst==port){
			check2=true;
		}
	}
	return (check1 || check2);
}

//iteruje vector a hladá v nom danu adresu, ak ju najde vraca jeho poziciu, inak vraca -1
int iterateMacVector(vector<address> vec,unsigned char * buffer)
{
	bool ok;
	for(std::vector<address>::size_type i = 0; i != vec.size(); i++)
	{
    	ok=true;
    	for (int j = 0; j < 6; ++j)
    	{
    		if(vec[i].addr[j]!=buffer[j])
    		{
    			ok=false;
    		}	
    	}
    	if(ok==true)
    	{
   			return i;
   		}
	}
	return -1;
}

//iteruje vector a hladá v nom danu adresu, ak ju najde vraca jeho poziciu, inak vraca -1
int iterateIpv4Vector(vector<address> vec,unsigned char * buffer)
{
	bool ok;
	for(std::vector<address>::size_type i = 0; i != vec.size(); i++)
	{
    	//printf("i=%d\n",i);
    	ok=true;
    	for (int j = 0; j < 4; ++j)
    	{
    		//printf("%d =?= %d \n",vec[i].addr[j],buffer[j]);
    		if(vec[i].addr[j]!=buffer[j])
    		{
    			ok=false;
    		}	
    	}
    	//printf("\n");
    	if(ok==true)
    	{
   			//printf("returning %d\n",i);
   			return i;
   		}
	}
	//printf("returning -1\n");
	return -1;
}


//iteruje vector a hladá v nom danu adresu, ak ju najde vraca jeho poziciu, inak vraca -1
int iterateDataVector(vector<portData> vec,int port)
{
	for(std::vector<portData>::size_type i = 0; i != vec.size(); i++)
	{
    	if(vec[i].port==port)
    	{
    		return i;
    	}	
	}
	return -1;
}

//Vyberie z vektora najvacsiu polozku a vrati jeho index
int pickNextAdress(vector<address> v)
{
	
	int max=0;
	int index=-1;
	
	for(std::vector<address>::size_type i = 0; i != v.size(); i++)
	{
		if(v[i].data1>max)
		{
			max=v[i].data1;
			index=(int)i;
		}
	}
	return index;
}

//Vyberie z vektora najvacsiu polozku a vrati jeho index
int pickNextPortData(vector<portData> v)
{
	
	int max=0;
	int index=-1;
	
	for(std::vector<portData>::size_type i = 0; i != v.size(); i++)
	{
		if(v[i].data1>max)
		{
			max=v[i].data1;
			index=(int)i;
		}
	}
	return index;
}
