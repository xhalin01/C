#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>

using namespace std;
#define TAG 0

vector<int> getNumbersFromFile();
void getDistributionTable(int cpuNum, int numCount, int* distributionTable);
int compare(const void* a, const void* b);
bool haveNeighbour(int i, int cycles);
int prevNumCount(int* distributionTable, int k);
int getCycles(int cpuNum);
void mergeArrays(int* a, int sizeA, int* b, int sizeB, int* c);

int main(int argc, char* argv[])
{
    int cpuNum;
    int myId;
    int numCount = atoi(argv[1]);
    MPI_Status mpiStatus;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &cpuNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

    vector<int> allNumbers;
    int right;
    int* myNumbs;
    int* neighNumbs;
    int numCountToAlloc;


///////////////////////////////////CPU#0 DISTRIBUTES NUMBERS

    if (myId == 0) {
        allNumbers = getNumbersFromFile();
        if ((numCount % cpuNum) != 0) {               //distribute carefully, numCount not divideable by cpuNum
            int* distributionTable = (int*)malloc(cpuNum * (sizeof(int)));
            getDistributionTable(cpuNum, numCount, distributionTable);
            for (int i = 0; i < cpuNum; ++i) {
                
                int* numbersToSend = (int*)malloc(distributionTable[i] * (sizeof(int)));

                if (i == 0) {
                    for (int j = 0; j < distributionTable[i]; ++j) {
                        numbersToSend[j] = allNumbers[j];
                    }
                }
                else {
                    for (int j = 0; j < distributionTable[i]; ++j) {
                        int prev = prevNumCount(distributionTable, i);
                        numbersToSend[j] = allNumbers[prev + j];
                    }
                }

                int nums = distributionTable[i];
                MPI_Send(&nums, 1, MPI_INT, i, TAG, MPI_COMM_WORLD); //numsCount to alloc
                if (i != cpuNum - 1){
                    MPI_Send(&distributionTable[i + 1], 1, MPI_INT, i, TAG, MPI_COMM_WORLD); //numsCount of right                  
                }
                else{
                    MPI_Send(&distributionTable[i], 1, MPI_INT, i, TAG, MPI_COMM_WORLD); //does not have right
                }
                MPI_Send(numbersToSend, nums, MPI_INT, i, TAG, MPI_COMM_WORLD); //actual numbers to send
                free(numbersToSend);
            }
        }

        else {                                         //distribute normally
            int* numbersToSend = (int*)malloc(numCount / cpuNum * (sizeof(int)));
            for (int i = 0; i < cpuNum; ++i) {
                for (int j = 0; j < numCount / cpuNum; ++j) {
                    numbersToSend[j] = allNumbers[(i * numCount / cpuNum) + j];
                }

                int nums = numCount / cpuNum;
                MPI_Send(&nums, 1, MPI_INT, i, TAG, MPI_COMM_WORLD); //numsCount to alloc
                MPI_Send(&nums, 1, MPI_INT, i, TAG, MPI_COMM_WORLD); //numsCount of right
                MPI_Send(numbersToSend, nums, MPI_INT, i, TAG, MPI_COMM_WORLD); //actual numbers to send
            }
            free(numbersToSend);
        }
    }

    MPI_Recv(&numCountToAlloc, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &mpiStatus);
    MPI_Recv(&right, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &mpiStatus);
    myNumbs = (int*)malloc(numCountToAlloc * (sizeof(int)));
    MPI_Recv(myNumbs, numCountToAlloc, MPI_INT, 0, TAG, MPI_COMM_WORLD, &mpiStatus);

    if (myId == 0) {
        int* distributionTable = (int*)malloc(cpuNum * (sizeof(int)));
        getDistributionTable(cpuNum, numCount, distributionTable);
    }

    ////////////////////////////////PREPROCESS
    

    qsort(myNumbs, numCountToAlloc, sizeof(int), compare);
    ///////////////////////////////START SORTING

    int cycles = getCycles(cpuNum);

    for (int k = 0; k < cycles; ++k) {
        for (int i = 0; i < cycles * 2; i += 2) {
            if (haveNeighbour(i, cpuNum - 1)) {
                /////one cpu sends his numbers and waits for sorted half
                if (myId == i + 1) {
                    MPI_Send(myNumbs, numCountToAlloc, MPI_INT, myId - 1, TAG, MPI_COMM_WORLD);
                    MPI_Recv(myNumbs, numCountToAlloc, MPI_INT, myId - 1, TAG, MPI_COMM_WORLD, &mpiStatus);
                }
                else if (myId == i) {
                    ////one cpu waits for numbers, merge them, sorts them and sends one half
                    int* recievedNums = (int*)malloc(right * (sizeof(int)));
                    MPI_Recv(recievedNums, right, MPI_INT, myId + 1, TAG, MPI_COMM_WORLD, &mpiStatus);
                    int merged[right + numCountToAlloc];
                    mergeArrays(recievedNums, right, myNumbs, numCountToAlloc, merged);
                    qsort(merged, right + numCountToAlloc, sizeof(int), compare);

                    for (int i = 0; i < numCountToAlloc; ++i)
                        myNumbs[i] = merged[i];

                    for (int i = 0; i < right; ++i)
                        recievedNums[i] = merged[numCountToAlloc + i];

                    MPI_Send(recievedNums, right, MPI_INT, myId + 1, TAG, MPI_COMM_WORLD);
                    free(recievedNums);
                }
            }
        }
        for (int i = 1; i < cycles * 2; i += 2) {
            if (haveNeighbour(i, cpuNum - 1)) {
                   //one cpu sends his numbers and waits for sorted half
                if (myId == i + 1) {
                    MPI_Send(myNumbs, numCountToAlloc, MPI_INT, myId - 1, TAG, MPI_COMM_WORLD);
                    MPI_Recv(myNumbs, numCountToAlloc, MPI_INT, myId - 1, TAG, MPI_COMM_WORLD, &mpiStatus);
                }
                else if (myId == i) {
                    ////one cpu waits for numbers, merge them, sorts them and sends one half
                    int* recievedNums = (int*)malloc(right * (sizeof(int)));
                    MPI_Recv(recievedNums, right, MPI_INT, myId + 1, TAG, MPI_COMM_WORLD, &mpiStatus);

                    int merged[right + numCountToAlloc];
                    mergeArrays(recievedNums, right, myNumbs, numCountToAlloc, merged);
                    qsort(merged, right + numCountToAlloc, sizeof(int), compare);

                    for (int i = 0; i < numCountToAlloc; ++i)
                        myNumbs[i] = merged[i];

                    for (int i = 0; i < right; ++i)
                        recievedNums[i] = merged[numCountToAlloc + i];
                    MPI_Send(recievedNums, right, MPI_INT, myId + 1, TAG, MPI_COMM_WORLD);
                    free(recievedNums);
                }
            }
        }
    }

    //////////////////////////////PRINT RESULT

    if (myId != 0) {
        MPI_Send(myNumbs, numCountToAlloc, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    }
    else {
        for (int i = 0; i < allNumbers.size(); ++i) {
            printf("%d ", allNumbers[i]);
        }
        printf("\n");

        int sorted[numCount];

        for (int i = 0; i < numCountToAlloc; ++i) {
            sorted[i] = myNumbs[i];
        }

        int* distributionTable = (int*)malloc(cpuNum * (sizeof(int)));
        getDistributionTable(cpuNum, numCount, distributionTable);

        for (int i = 1; i < cpuNum; ++i) {
            int recv[distributionTable[i]];
            MPI_Recv(recv, distributionTable[i], MPI_INT, i, TAG, MPI_COMM_WORLD, &mpiStatus);
            int prev = prevNumCount(distributionTable, i);
            for (int j = 0; j < distributionTable[i]; ++j) {
                sorted[prev + j] = recv[j];
            }
        }

        for (int i = 0; i < numCount; ++i) {
            printf("%d\n", sorted[i]);
        }
        
    }
    MPI_Finalize();
    return 0;
}


//function for computing number of neccesary cycles
int getCycles(int cpuNum)
{
    if (cpuNum % 2 == 0) {
        return cpuNum / 2;
    }
    return cpuNum / 2 + 1;
}


//counts number of all previous numbers in distribution table
int prevNumCount(int* distributionTable, int k)
{
    int n = 0;
    for (int i = 0; i < k; ++i) {
        n += distributionTable[i];
    }
    return n;
}

//returns true if there is another cpu on the right
bool haveNeighbour(int i, int cpuNum)
{

    if (cpuNum - i >= 1) {
        return true;
    }
    return false;
}


//auxiliary function for sorting
int compare(const void* a, const void* b)
{
    if (*(int*)a == *(int*)b)
        return 0;
    return *(int*)a < *(int*)b ? -1 : 1;
}


//functions for reading numbers from file
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


//function creates table of numbers count which will be distributed to each CPU
void getDistributionTable(int cpuNum, int numCount, int* distributionTable)
{
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

//function merges 2 arrays
void mergeArrays(int* a, int sizeA, int* b, int sizeB, int* c)
{
    memcpy(c, a, sizeA * sizeof(int));
    memcpy(c + sizeA, b, sizeB * sizeof(int));
}
