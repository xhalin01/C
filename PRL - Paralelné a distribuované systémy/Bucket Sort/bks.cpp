#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <algorithm>

#define TAG 0
using namespace std;

vector<int> getNumbersFromFile();
void getDistributionTable(int cpuNum, int numCount, int* distributionTable);
int getMyLevel(int index);
int getMyNumCount(int treeCapacity,int myLevel);
int compare(const void* a, const void* b);
int getParentIndex(int n);


int main(int argc, char* argv[])
{
	int cpuNum;   //count of used cpus
    int myId;     //rank of each cpu
    int numCount = atoi(argv[1]);     //count of numbers to sort
    int treeCapacity = atoi(argv[2]); //capacity of tree
    MPI_Status mpiStatus; 

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &cpuNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

	std::vector<int> numbers=getNumbersFromFile(); 

	//if sum of given numbers to sort is greater than 2, then create tree
    if(numbers.size()>2){
    	int myLevel=getMyLevel(myId);
    
		int numOfLeaves=pow(2,getMyLevel(cpuNum-1));
	    int treeHeight=1+(getMyLevel(cpuNum-1));
	    int leafCapacity=treeCapacity/numOfLeaves;

	    int myNumCount=getMyNumCount(treeCapacity,myLevel);
		int *myNumbs=(int*)malloc((myNumCount) * (sizeof(int)));
		
		int* distributionTable = (int*)malloc(numOfLeaves * (sizeof(int)));
	    getDistributionTable(numOfLeaves, numCount, distributionTable);

	    

	    vector<int> v1; //one half of numbers
		vector<int> v2; //other half
		vector<int> v3; //merged half
		vector<int> v;  //temp vector

		//if leaf node
    	if(myId>=(numOfLeaves-1)){
	        //fill all numbers with -1
	        for (int i = 0; i < myNumCount; ++i){
	        	myNumbs[i]=-1;
	        }
	        //count sum of numbers which was read by previous leaves
	        int prevNumCount=0;
	        for (int i = 0; i < myId-numOfLeaves+1; ++i){
	        	prevNumCount+=distributionTable[i];
	        }
	        //read numbers from file wich belongs to exact leaf
	        for (int i = 0; i < distributionTable[(myId-numOfLeaves+1)]; ++i){
	        	myNumbs[i]=numbers[prevNumCount+i];
	        }
	        //create vector for sorting in leaves
	        for (int i = 0; i < leafCapacity; ++i){
	        	v.push_back(myNumbs[i]);
	        }
	        sort(v.begin(), v.end());
	        //get array of ints from vector
	        myNumbs=v.data();
		}

		for (int i = treeHeight-1; i > 0; --i){                                                   //cycle repeats for (treeHeight-1) times
			if(myLevel==i){                           											  //if sender 
	 			MPI_Send(myNumbs,myNumCount, MPI_INT, getParentIndex(myId), TAG, MPI_COMM_WORLD); //send to parent
			}
			else if(myLevel==i-1){                                                                //if reciever(parent)
				int *a=(int*)malloc((myNumCount/2) * (sizeof(int)));
				int *b=(int*)malloc((myNumCount/2) * (sizeof(int)));

				MPI_Recv(a, myNumCount/2, MPI_INT,myId*2+1, TAG, MPI_COMM_WORLD, &mpiStatus);     //recieve 2 halves
				MPI_Recv(b, myNumCount/2, MPI_INT,myId*2+2, TAG, MPI_COMM_WORLD, &mpiStatus);
 
				for (int i = 0; i < myNumCount/2; ++i){                                           //mage 2 vectors
					v1.push_back(a[i]);
					v2.push_back(b[i]);
				}

				merge(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));                 //merge vectors
				myNumbs=v3.data();                                                                //get array if ints
			}
		}

		//final printing of given unsorted and sorted numbers
		if(myId==0){
			for (int i = 0; i < numCount; ++i){
				printf("%d ",numbers[i]);
			}
			printf("\n");

			for (int i = 0; i < myNumCount; ++i){
				if(myNumbs[i]!=-1)
					printf("%d\n",myNumbs[i]);
			}
		}
    }
    //if sum of given numbers to sort is lover than 2, then tree is not generated,
    else{ 
    	if(numbers.size()==2){ //if there are two numbers to sort, compare them and print
    		printf("%d %d \n",numbers[0],numbers[1]);
    		if(numbers[0]>numbers[1]){
    			numbers[1]+=numbers[0];
    			numbers[0]=numbers[1]-numbers[0];
    			numbers[1]=numbers[1]-numbers[0];
    		}
    		printf("%d\n%d\n",numbers[0],numbers[1]);
    	}
    	else{ //if there is only one number to sort, print it
    		printf("%d \n%d\n",numbers[0],numbers[0]);
    	}
    }
	
	MPI_Finalize();
}

//function for reading numbers from file
vector<int> getNumbersFromFile()
{
    std::vector<int> v;
    int number;

    fstream f;
    f.open("numbers", ios::in);
    while (f.good()) {
        number = f.get();
        if (!f.good())
            break;
        v.push_back(number);
    }
    f.close();
    return v;
}


//function for generating distribution table for leaf nodes, in case that given numbers
//to sort cannot be distributed equally this function will attempt to distribute it
//as evenly as possible, e.g.:
//                   node1  node2 node3 node4
//16 numbers to sort:  [4]   [4]   [4]   [4]
//11 numbers to sort:  [3]   [3]   [2]   [3]
void getDistributionTable(int cpuNum, int numCount, int* distributionTable){
    int baseNum = numCount / cpuNum;
    int residue = numCount % cpuNum;

    for (int i = 0; i < cpuNum; ++i) {
        distributionTable[i] = baseNum;
    }

    for (int i = 0; i < residue; ++i) {
        if (i % 2 == 0) {
            distributionTable[i / 2] = distributionTable[i / 2] + 1;
        }
        else {
            distributionTable[(cpuNum - 1) - (i / 2)] = distributionTable[(cpuNum - 1) - (i / 2)] + 1;
        }
    }
}

//function returns level of node
int getMyLevel(int index){
	return floor(log(index + 1)/log(2));
}

//function returns capacity of node from given level
int getMyNumCount(int treeCapacity,int myLevel){
	return (treeCapacity/pow(2,myLevel));
}

//function returns index of parent node from given node
int getParentIndex(int n){
	return floor( (n - 1) / 2 );
}