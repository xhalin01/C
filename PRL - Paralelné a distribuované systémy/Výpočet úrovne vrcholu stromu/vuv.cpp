#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>
#include <math.h>

using namespace std;
#define TAG 0


//STRUCT DEFINES EDGE BY STARTING AND FINISHING NODE
struct edge{
    char from;
    char to;
};

//STRUCT CONTAINS FORWARD, REVERSE EDGES AND NEIGHBOURS IF THEY EXISTS
struct node{
    struct edge forward;
    struct edge reverse;
    struct node* next;
    int edgeId;
};

bool haveChild1(int i,string tree);
bool haveChild2(int i,string tree);
node* getEtour(vector<node*> tempList,node *myEdges,vector<node*> adjList,int myId);
node *findReverse(vector<node*> adjList,edge reverse);
bool pathToRoot(node* tempNode,node* root);
int getWeight(string tree,node* myEdges);
int getEtourId(node *myEtour,vector<node*> tempList,int myId);
int getEtourIndex(int e,int *allEtours[2],int size);

int main(int argc, char* argv[])
{

    double t1, t2;
    int cpuNum;
    int myId;
    MPI_Status mpiStatus;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &cpuNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);
    string tree=argv[1];
    int edgeNum=atoi(argv[2]);

    node *myEtour;
    node *myEdges;
    int myWeight;
    int mySuffix;
    int myRank=0;


    std::vector<node*> adjList;
    std::vector<node*> tempList; 

    ///////////////////////////////////////////////COMPUTE NUMBERS OF NODES AND LEAVES
    int x = tree.length();
    int n = 1;
    int y = 0;
    int remain;
    
    while(true){
        y=pow(2,n)-1;         
        remain=x-y;
        if(remain<=0)
            break;
        n++;
            
    }
    n--;
    
    int leaves=pow(2,n)+remain;
    int nodeNum=tree.length()-leaves-1;
    if(remain!=0){
        int prevLevel=((pow(2,(n--))-1)/2)+1;
        int tempLeaves=leaves;
        if(tempLeaves%2==1)
                tempLeaves++;
        int claimedNodes=tempLeaves/2;
        leaves=leaves+(prevLevel-claimedNodes);
        nodeNum=tree.length()-1-leaves;
    }

    //////////////////////////////////////////////////////////CREATE ADJACENCY LIST
    int uniqueId=0;
    for (int i = 1; i <= edgeNum+1; ++i)
    {
        //ROOT HAS 2 NEIGHBOURS(left and right child)
        if(i==1){ 
            if(haveChild1(i,tree)){
                edge e1;
                edge e2;
                    
                e1.from=tree[i-1];
                e1.to=tree[i*2-1];

                e2.from=tree[i*2-1];
                e2.to=tree[i-1];

                node *n1=(node* )malloc(sizeof(node));
                (*n1).forward=e1;
                n1->reverse=e2;
                n1->edgeId=uniqueId++;

                bool ch2=(haveChild2(i,tree));
                node *n2=(node* )malloc(sizeof(node));;
                if(ch2){
                    edge e3;
                    edge e4;

                    e3.from=tree[i-1];
                    e3.to=tree[i*2];

                    e4.from=tree[i*2];
                    e4.to=tree[i-1];

                    
                    n2->forward=e3;
                    n2->reverse=e4;
                    n2->next=nullptr;
                    n2->edgeId=uniqueId++;
                }
                n1->next=(ch2)? n2 : nullptr;

                adjList.push_back(n1);
                tempList.push_back(n1);
                if(ch2)
                    tempList.push_back(n2);
            }
            else{
                //TREE HAS ONLY ROOT NODE, PRINT IT AND EXIT
                printf("%c:0\n",tree[0]);
                MPI_Finalize();
                exit(0);
            }
        }
        //NODES HAS 3 NEIGHBOUR(parent,left and right child)
        else if(i>1 && i<=nodeNum+1){
            edge a1,a2;
            edge b1,b2;
            edge c1,c2;

            node* a=(node* )malloc(sizeof(node));;
            node* b=(node* )malloc(sizeof(node));;
            node* c=(node* )malloc(sizeof(node));;

            bool ch1=(haveChild1(i,tree));
            bool ch2=(haveChild2(i,tree)); 

            a1.from=tree[i-1];
            a1.to=tree[i/2-1];
            a2.from=tree[i/2-1];
            a2.to=tree[i-1];

            a->forward=a1;
            a->reverse=a2;
            a->edgeId=uniqueId++;
            if(ch1){
                b1.from=tree[i-1];
                b1.to=tree[i*2-1];
                b2.from=tree[i*2-1];
                b2.to=tree[i-1];

                b->forward=b1;
                b->reverse=b2;
                b->edgeId=uniqueId++;
            }
            if(ch2){
                c1.from=tree[i-1];
                c1.to=tree[i*2];
                c2.from=tree[i*2];
                c2.to=tree[i-1];

                c->forward=c1;
                c->reverse=c2;
                c->edgeId=uniqueId++;
            }
            
            a->next=(ch1 ? b : nullptr);
            b->next=(ch2 ? c : nullptr);
            c->next=nullptr;

            adjList.push_back(a);

            tempList.push_back(a);
            if(ch1)
                tempList.push_back(b);
            if(ch2)
                tempList.push_back(c);
        }
        //LEAVES HAS ONLY 1 NEIGHBOUR(parrent)
        else{
            edge e1,e2;
            e1.from=tree[i-1];
            e1.to=tree[i/2-1];
            e2.from=tree[i/2-1];
            e2.to=tree[i-1];
            node *n=(node* )malloc(sizeof(node));;
            n->forward=e1;
            n->reverse=e2;
            n->edgeId=uniqueId++;
            n->next=nullptr;

            adjList.push_back(n);
            tempList.push_back(n);
        }
    }

    //////////////////////////////////////////////EACH CPU GETS ONE EDGE FROM EDGE LIST
    myEdges=tempList[myId];
    myEdges->edgeId=myId;

    ///////////////////////////////////////////////////////////COMPUTE ETOUR AND WEIGHT
    myEtour=getEtour(tempList,myEdges,adjList,myId);
    myWeight=getWeight(tree,myEdges);
    

    //////////////////////////////////////////////////////////SEND ETOUR TO MASTER NODE
    int myEtourId=getEtourId(myEtour,tempList,myId);
    int size=tempList.size();

    MPI_Send(&(myEdges->edgeId), 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    MPI_Send(&myEtourId, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    int allEtours[size][2];
    

    if(myId==0){
        for (int i = 0; i < size; ++i){
            int tempE;
            int tempId;

            MPI_Recv(&tempId, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &mpiStatus);
            MPI_Recv(&tempE, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &mpiStatus);
            
            allEtours[i][0]=tempE;
            allEtours[i][1]=tempId;

            //printf("recieved %d etour from %d\n",tempE,tempId);
        }

        int newEtour[size];
        for (int i = 0; i < size; ++i){
            if(i==0){
                newEtour[i]=allEtours[0][0];
            }
            else{
                int index;
                for (int j = 0; j < size; ++j){
                    if(allEtours[j][1]==newEtour[i-1]){
                       index=j;
                    }
                }
                newEtour[i]=allEtours[index][0];
            }
        }
        for (int i = 0; i < size; ++i){
            MPI_Send(newEtour, size, MPI_INT, i, TAG, MPI_COMM_WORLD);
        }
    }

    int succ[size];
    MPI_Recv(succ, size, MPI_INT, 0, TAG, MPI_COMM_WORLD, &mpiStatus);
    
    ////////////////CORECT SUCC LIST(delete last edge and add first edge from root)
    int newSucc[size];
    for (int i = 0; i < size; ++i){
        if(i==0){
            newSucc[i]=0;
        }
        else{
            newSucc[i]=succ[i-1];
        }
        
    }

    for (int i = 0; i < size; ++i){
        succ[i]=newSucc[i];
    }

    /////////////////////////////////////////SEND ALL WEIGHTS TO MASTER NODE
    MPI_Send(&myWeight, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    
    int val[size];
    int newVal[size];
    int tempVal[size];

    if(myId==0){
        for (int i = 0; i < size; ++i){
            int tempW;
            MPI_Recv(&tempW, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, &mpiStatus);
            val[i]=tempW;
            tempVal[i]=tempW;
        }

        for (int i = 0; i < size; ++i){
           newVal[i]=val[succ[i]];
        }
        
        for (int i = 0; i < size; ++i){
            MPI_Send(newVal, size, MPI_INT, i, TAG, MPI_COMM_WORLD);
        }

        
    }

    MPI_Recv(val, size, MPI_INT, 0, TAG, MPI_COMM_WORLD, &mpiStatus);

    ////////////////////////////////////////////////////////COMPUTE SUFFIX
    for (int i = 0; i < size; i++){
        if(i==0){
            val[i]=0;
        }
        else{
            val[i]=val[i-1]+val[i];
        }
    }

    int myValIndex;
    for (int i = 0; i < size; ++i){
        if(succ[i]==myId){
            myValIndex=i;
        }
    }

    ///////////////////////////////////////////////////////COMPUTE AND SEND LEVEL TO MASTER NODE
    int myLevel=val[myValIndex]+1;
    

    if(myWeight==1){
        MPI_Send(&myId, size, MPI_INT, 0, TAG, MPI_COMM_WORLD);
        MPI_Send(&myLevel, size, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    }


    if(myId==0){
        int rcvLvl;
        int rvcId;
        int levels[size][2];


        //////////////////////////////////////////////////MAKE ARRAY OF NODES WHICH MASTER NODE SHOULD WAIT FOR
        int waitFrom[edgeNum];
        int next=0;
        for (int i = 0; i < edgeNum; ++i)
        {
            while(tempVal[next]==-1 && next<=size){
                next++;
            }
            waitFrom[i]=next;
            next++;
        }

        ///////////////////////////////////////////////RECIEVE LEVELS FROM FORWARD NODES

        for (int i = 0; i < edgeNum; ++i)
        {
            MPI_Recv(&rvcId, size, MPI_INT, waitFrom[i], TAG, MPI_COMM_WORLD, &mpiStatus);
            MPI_Recv(&rcvLvl, size, MPI_INT, waitFrom[i], TAG, MPI_COMM_WORLD, &mpiStatus);
            levels[i][0]=rvcId;
            levels[i][1]=rcvLvl;
        }

        int c;
        
        printf("%c:0,",tree[0]);
        for (int i = 0; i < edgeNum; ++i){
            n=tempList[levels[i][0]]->forward.to;
            c=levels[i][1];
            printf("%c:%d",n,c);
            if(i!=edgeNum-1){
            	printf(",");
            }
        }
        printf("\n");
    }
    MPI_Finalize();
}


//Returns true if node has left child
bool haveChild1(int i,string tree){
    int temp=tree.length()-(i*2);
    if(temp>=0)
        return true;
    else
        return false;
}

//Returns true if node has right child
bool haveChild2(int i,string tree){
    int temp=tree.length()-(i*2+1);
    if(temp>=0){
        return true;
    }
    else{
        return false;
    }
}


//Returns edge which follows given edge
node* getEtour(vector<node*> tempList,node *myEdges,vector<node*> adjList,int myId){
    edge reverse=myEdges->reverse;
    for (int i = 0; i < tempList.size(); ++i)
    {
        if(tempList[i]->forward.from==reverse.from && reverse.to==tempList[i]->forward.to){
            bool next=(tempList[i]->next==nullptr) ? false : true;
            if(next){
                return tempList[i]->next;
            }
            else{
                node *tempNode=findReverse(adjList,tempList[i]->forward);
                if(pathToRoot(tempNode,adjList[0])){
                    tempNode=myEdges;
                }
                return tempNode;
            }
        }
    }
}

//Returns true if path is leading to root(last edge)
bool pathToRoot(node* tempNode,node* root){
    if(tempNode->forward.from==root->forward.from){
        return true;
    }
    return false;
}


//Returns reversing edge from edge list
node *findReverse(vector<node*> adjList,edge reverse){
    for (int i = 0; i < adjList.size(); ++i)
    {
        if(adjList[i]->forward.from==reverse.from)
        {
            return adjList[i];
        }
    }
    return nullptr;
}

//Computes weight of node
int getWeight(string tree,node* myEdges){
    int fromIndex;
    int toIndex;

    fromIndex=tree.find(myEdges->forward.from);
    toIndex=tree.find(myEdges->forward.to);

    if((fromIndex-toIndex)<0)
        return 1;
    return -1;
}

//Returns ID of following edge
int getEtourId(node *myEtour,vector<node*> tempList,int myId){
    for (int i = 0; i < tempList.size(); ++i)
    {
        if(tempList[i]->forward.from==myEtour->forward.from && tempList[i]->forward.to==myEtour->forward.to){
            return tempList[i]->edgeId;
        }
    }
    return -1;
}