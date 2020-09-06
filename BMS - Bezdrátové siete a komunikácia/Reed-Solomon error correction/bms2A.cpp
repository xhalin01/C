//BMS project 2, ECC encoder
//Used Reed-Solomon error correction library - http://rscode.sourceforge.net/

#include "bms2.h"
#include "ecc.h"
#include <iostream>
#include <cstring>

void writeToFile (FILE *out, unsigned char *buffer, unsigned char *segment, const unsigned tmp,int wordCount);
int encodeMem(FILE *f, unsigned char *const memmory, const int wordCount);

int main(int argc, char** argv)
{
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
	const auto filename = std::string(argv[1]) + ".out";
	auto out = openFile(filename, open_mode_e::write);
	if (!out)
	{
		fclose(f);
		std::cerr << "Unable to open output file '" << filename << "'." << std::endl;
		return -1;
	}

	const auto wordCount = getFileSize(f) / dataLength;	//number of words in file
	const auto memsize = wordCount * (dataLength + NPAR);   
	auto buffer = memReserve(memsize);					//reserve memmory
	if (buffer == nullptr)
	{
		fclose(f);
		fclose(out);
		std::cerr << "Bad alloc." << std::endl;
		return -1;
	}

	initialize_ecc();
	//encode file
	const auto n = encodeMem(f, buffer, wordCount);
	//write encoded buffer to file
	writeToFile(out, buffer, segment, n, wordCount); 
	finalize(buffer, f, out);

	return 0; 
}

//Function for writing encoded memmory buffer to file,
void writeToFile (FILE *out, unsigned char *buffer, unsigned char *segment, const unsigned tmp,int wordCount)
{
	//Function for interleaving last segment(burst error prevention)
	const auto interleaveLastWord = [&segment, &out](const int pos)
	{
		fwrite(segment + pos, sizeof(unsigned char), 1, out);
	};

	for (auto i = 0U; i < segmentLength; ++i)
	{
        fwrite(buffer + (wordCount*i), wordCount, 1, out);
        //if last word, was uncomplete then interleaving is needed
        if (i < tmp + NPAR && tmp != 0)
			interleaveLastWord(i);
    }
}

//Function for encoding segments to memmory
int encodeMem(FILE *f, unsigned char *const memmory, const int wordCount)
{
	//Function for interleaving segments, whole memmory is divided to size_of_file/dataLength segments
	//in each segment there is one byte from each segment
	const auto interleave = [&memmory, &wordCount](const unsigned char *const segment, const int cycle)
	{
		// Helper lambda returning desired interleaving index
		const auto getIndex = [&](const unsigned i) { return wordCount * i + (cycle - 1); };

		for (auto i = 0U; i < segmentLength; ++i)
			memmory[getIndex(i)] = segment[i];
	};

    auto cycle = 0U;
    auto tmp = 0U;
    unsigned char buffer[dataLength];
    while (!feof(f))
	{
        //read chunk from file
        memset(&buffer, 0, dataLength); 
        tmp = fread(buffer, sizeof(unsigned char), dataLength, f);

        //encode_data() from ecc.h
        memset(&segment, 0, segmentLength);
        encode_data(buffer, tmp, segment);
        ++cycle;

        //if not last word then interleave
        if (tmp == dataLength)
            interleave(segment, cycle);
    }
    //return size of last read word
    return tmp; 
}
