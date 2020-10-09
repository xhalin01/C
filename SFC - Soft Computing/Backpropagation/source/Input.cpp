//
// Created by Majkl on 2017-11-29.
//


#include "Input.h"


Input::Input(Arguments args) {
    std::ifstream inputFile;
    inputFile.open(args.inputFile);
    outputFile=args.outputFile;
    learningRate=args.learningRate;
    printFirstCycle=args.printFirstCycle;
    printLastCycle=args.printLastCycle;
    printErrorOnly=args.printErrorOnly;
    epsilon=args.epsilon;
    allowedTime=args.allowedTime;

    std::string line;
    getline(inputFile,line);

    std::stringstream stream(line);

    while (!stream.eof()) {
        int n;
        stream >> n;
        topology.push_back(n);
    }

    while (!inputFile.eof()){
        input.push_back(std::vector<double> () );
        output.push_back(std::vector<double> () );

        std::string line;
        getline(inputFile,line);
        std::stringstream stream(line);
        for (int i = 0; i < topology[0]; ++i) {
            double d;
            stream >> d;
            input.back().push_back(d);
        }

        for (int j = 0; j < topology.back(); ++j) {
            double d;
            stream >> d;
            output.back().push_back(d);
        }
        if(output.back().size() != topology.back() or input.back().size() != topology[0]) {
            fprintf(stderr, "Error parsing input file\n");
            exit(3);
        }
    }
    if(output.size() != input.size()){
        fprintf(stderr, "Error parsing input file\n");
        exit(3);
    }

}