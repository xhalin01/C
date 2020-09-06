//BMS project 2, ECC decoder
//Used Reed-Solomon error correction library - http://rscode.sourceforge.net/

#include "bms2.h"
#include "ecc.h"
#include <iostream>
#include <string.h>

void errorCorection();
void decodeWord(const unsigned char*const buffer, FILE *const out, const int codewordNum,
	            const int codewordCount, const int lastWordSize);
void decodeLastWord(const unsigned char *const buffer, FILE*const out, const unsigned wordCount,
					const unsigned lastWordSize);
void decode(const unsigned char*const buffer, FILE *const out, const unsigned wordCount,
			const unsigned lastWordSize);


int main(int argc, char** argv) {
	//check input from user
	if (!checkParams(argc))
	{
		std::cerr << "Bad args." << std::endl;
		return -1;
	}

	//open input file
	auto f = openFile(argv[1], open_mode_e::read);
	if (!f)
	{
		std::cerr << "Unable to open input file '" << argv[1] << "'." << std::endl;
		return -1;
	}

	//create output file
	const auto filename = std::string(argv[1]) + ".ok";
	auto out = openFile(filename, open_mode_e::write);
	if (!out)
	{
		fclose(f);
		std::cerr << "Unable to open output file '" << filename << "'." << std::endl;
		return -1;
	}

	//get size of file and number of codewords in input file
	const auto fileSize = getFileSize(f);
	const auto wordCount = fileSize / segmentLength;
	const auto lastWordSize = fileSize % segmentLength;

	//reserve memmory for codewords
	auto buffer = memReserve(fileSize);
	if (buffer == nullptr)
	{
		fclose(f);
		fclose(out);
		std::cerr << "Bad alloc." << std::endl;
		return -1;
	}

	//read file, decode input and write result to file
	fread(buffer, sizeof(unsigned char), fileSize, f);
	decode(buffer, out, wordCount, lastWordSize);
	finalize(buffer, f, out);

	return 0;
}


//Correct errors in codewords if any,
//decode_data(), from ecc.h
void errorCorection()
{
    decode_data(segment, segmentLength);
    if (check_syndrome() != 0)
        correct_errors_erasures(segment, segmentLength, 0, nullptr);
}

//Deinterleave segment,correct errors and write result to output file
void decodeWord(const unsigned char*const buffer, FILE *const out, const int codewordNum,
	            const int codewordCount, const int lastWordSize)
{
    auto temp = lastWordSize;

    //deinterleave next segment
    for (auto i = 0U; i < segmentLength; i++)
    {
        --temp;
        const auto index = temp <= 0 
    						? (codewordCount + 1) * lastWordSize + codewordCount * (i - lastWordSize) + codewordNum
                            : (codewordCount + 1) * i + codewordNum;
        segment[i] = buffer[index];
    }

    //correct errors in deinterleaved word
    errorCorection();
    //write to file
    fwrite(segment, sizeof(unsigned char), dataLength, out);
}

//Decoding last word
void decodeLastWord(const unsigned char *const buffer, FILE*const out, const unsigned wordCount,
					const unsigned lastWordSize)
{
	for (auto i = 0U; i < lastWordSize; ++i)
		segment[i] = buffer[(i+1)*(wordCount+1) - 1];

	errorCorection();
	fwrite(segment, sizeof(unsigned char), lastWordSize - NPAR, out);
}

//decode each segment in file
void decode(const unsigned char*const buffer, FILE *const out, const unsigned wordCount,
			const unsigned lastWordSize)
{
    //incitialization of rs code library
    initialize_ecc();

    //Decoding of each segment in file;
    for (auto i = 0U;i< wordCount + 1; ++i)
	{
        memset(&segment, 0, segmentLength);
        if (i < wordCount)
			decodeWord(buffer, out, i, wordCount, lastWordSize);
		else
    		decodeLastWord(buffer, out, wordCount, lastWordSize);
    }
}