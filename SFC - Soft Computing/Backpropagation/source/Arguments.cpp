//
// Created by Majkl on 2017-11-29.
//

#include "Arguments.h"

Arguments::Arguments(int argc, char **argv) {
    inputSet = false;
    outputSet = false;
    bool input=false;

    for (int i = 1; i < argc; ++i) {
        if(strcmp(argv[i],"-i")==0){
            input=true;
            if(i!=argc-1) {
                FILE *file;
                file = fopen(argv[i + 1], "r");
                if (file == NULL) {
                    fprintf(stderr, "Input file:\"%s\" does not exists\n", argv[i + 1]);
                    exit(1);
                } else {
                    inputFile = argv[i + 1];
                    fclose(file);
                }
            }
            else{
                fprintf(stderr, "Missing input file name");
            }
        }
        else if(strcmp(argv[i],"-o")==0){
            if(i!=argc-1) {
                outputFile = argv[i + 1];
            }
            else {
                fprintf(stderr, "Missing output file name");
            }
        }
        else if(strcmp(argv[i],"-help")==0) {
            printHelp();
            exit(0);
        }
        else if(strcmp(argv[i],"-l")==0) {
            if(i!=argc-1){
                char* ptr = 0;
                strtod(argv[i+1], &ptr);
                if(*ptr != '\0' || ptr == argv[i+1]){
                    fprintf(stderr,"Wrong learning rate number \"%s\"",argv[i+1]);
                    exit(2);
                }
                else{
                    char* ptr = 0;
                    double d=strtod(argv[i+1], &ptr);
                    learningRate=d;
                    //fprintf(stderr,"%lf",d);
                }
            }
            else{
                fprintf(stderr,"Missing learning rate number");
            }
        }
        else if(strcmp(argv[i],"-e")==0) {
            if(i!=argc-1){
                char* ptr = 0;
                strtod(argv[i+1], &ptr);
                if(*ptr != '\0' || ptr == argv[i+1]){
                    fprintf(stderr,"Wrong epsilon number \"%s\"",argv[i+1]);
                    exit(2);
                }
                else{
                    char* ptr = 0;
                    double d=strtod(argv[i+1], &ptr);
                    epsilon=d;
                }
            }
            else{
                fprintf(stderr,"Missing epsilon number");
            }
        }
        else if(strcmp(argv[i],"-t")==0) {
            if(i!=argc-1){
                char* ptr = 0;
                strtod(argv[i+1], &ptr);
                if(*ptr != '\0' || ptr == argv[i+1]){
                    fprintf(stderr,"Wrong time number \"%s\"",argv[i+1]);
                    exit(2);
                }
                else{
                    char* ptr = 0;
                    double d=strtod(argv[i+1], &ptr);
                    allowedTime=d;
                }
            }
            else{
                fprintf(stderr,"Missing time number");
            }
        }
        else if(strcmp(argv[i],"-printFirstCycle")==0){
            printFirstCycle=true;
        }
        else if(strcmp(argv[i],"-printLastCycle")==0){
            printLastCycle= true;
        }
        else if(strcmp(argv[i],"-printErrorOnly")==0){
            printErrorOnly= true;
        }
        else if(argc==1){
            fprintf(stderr,"Wrong arguments\n");
            printHelp();
            exit(4);
        }
    }
    if(inputFile=="" && !input){
        fprintf(stderr,"Missing training data in input file");
        exit(1);
    }
}

void Arguments::printHelp(){
    printf("Bacpropagarion neural newtwork\n"
                   "Usage: \n"
                   "[-help] prints this help\n"
                   "-i fileName input for network in file\n"
                   "[-o fileName] prints output to file\n"
                   "[-l learningRate] sets learning rate number (default 0.15)\n"
                   "[-e epsilon] sets precision(default 0.1)\n"
                   "[-t time] sets allowed time to compute(default 10 seconds)\n"
                   "[-printFirstCycle] prints results of first cycle only\n"
                   "[-printFirstCycle] prints results of last cycle only\n"
                   "[-printErrorOnly] prints errors of cycles only (ignores printFirstCycle and printFirstCycle)\n"
    );
}
