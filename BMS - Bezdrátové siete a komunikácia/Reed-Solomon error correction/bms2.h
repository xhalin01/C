#pragma once
#include "ecc.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>

//Helper variables, NPAR - number of parity bits from ecc.h
const static int dataLength = 147;
const static int segmentLength  = dataLength + NPAR;
unsigned char segment[256];

//Function returns the size of given file
int getFileSize(FILE*const f)
{
    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    rewind(f);
    return size;
}

//Checks validity of user input
inline bool checkParams(const int& argc)
{
	return argc == 2;
}

//Helper enum for function below
enum class open_mode_e
{
	read,
	write
};

//Function returns file descriptor of given file
FILE* openFile(const std::string& path, const open_mode_e mode)
{
	FILE *f = nullptr;

	switch (mode)
	{
		case open_mode_e::read:  f = fopen(path.c_str(), "r"); break;
		case open_mode_e::write: f = fopen(path.c_str(), "w"); break;
	}

	return f;
}

//Reserves memory for encoding/decoding, and sets it to zeros(calloc)
unsigned char* memReserve(const unsigned size)
{
    return size == 0 ? nullptr: static_cast<unsigned char *>(calloc(size, sizeof(unsigned char)));
}

//Cleaning after program is done
void finalize(unsigned char* memp, FILE* f, FILE* out)
{
    free(memp);
	memp = nullptr;
    fclose(f);
	f = nullptr;
    fclose(out);
	out = nullptr;
}
