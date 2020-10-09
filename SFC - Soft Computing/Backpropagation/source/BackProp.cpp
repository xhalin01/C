#include "Arguments.h"
#include "Input.h"
#include "Network.h"

int main(int argc, char** argv) {
    Arguments myArguments(argc,argv);

    Input myInput(myArguments);

    Network myNetwork(myInput);

    myNetwork.run();

    return 0;
}
