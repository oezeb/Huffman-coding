#include "huffman.h"

int main() {
	huffman::compress("test/mississippi.txt", "test/out/mississippi.o");
	huffman::decompress("test/out/mississippi.o", "test/out/mississippi.o.txt");

	huffman::compress("test/lenna.jpg", "test/out/lenna.o");
	huffman::decompress("test/out/lenna.o", "test/out/lenna.o.jpg");

    huffman::compress("test/civil_code.txt", "test/out/civil_code.o");
	huffman::decompress("test/out/civil_code.o", "test/out/civil_code.o.txt");
	
    huffman::compress("test/sherlock_holmes.txt", "test/out/sherlock_holmes.o");
	huffman::decompress("test/out/sherlock_holmes.o", "test/out/sherlock_holmes.o.txt");
}