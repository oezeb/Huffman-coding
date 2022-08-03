#include <iostream>
#include <string.h>
#include "huffman.h"

using namespace std;

int main(int argc, char** argv) {
	/**
	 * commands:
	 *    huffman -c <infile> <outfile>
	 *    huffman -d <infile> <outfile>
	 */

	if (argc == 4) {
		if (strcmp(argv[1], "-c") == 0) {
			huffman::compress(argv[2], argv[3]);
			return 0;
		} else if (strcmp(argv[1], "-d") == 0) {
			huffman::decompress(argv[2], argv[3]);
			return 0;
		}
	}

	cout << "Usage:" << endl;
	cout << "      g++ -o huffman main.cc bitstream.h huffman.h" << std::endl;
	cout << "      huffman -c <infile> <outfile>" << endl;
	cout << "      huffman -d <infile> <outfile>" << endl;
	return 0;
}