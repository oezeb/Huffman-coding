#ifndef BIT_STREAM
#define BIT_STREAM

#include<fstream>

/*
* @author: KingEze
* @mail: kingeze@mail.ustc.edu.cn
* @date: 2020/12/11 15:00 AM
* @description: class that read(write) data from(into) a binary file
*/

#define BYTE  8
#define POW2_0 1
#define POW2_1 2
#define POW2_2 4
#define POW2_3 8
#define POW2_4 16
#define POW2_5 32
#define POW2_6 64
#define POW2_7 128

typedef enum { read, write, app, none } Mode;

class BitStream {
private:
	std::fstream stream;
	Mode mode;
	int buffer;
	int index;
	int pow2(unsigned int i); // return 2^i (0 <= i <= 7)

public:
	BitStream() : mode(none), buffer(0), index(BYTE) {}

	BitStream(char const* filename, Mode openMode) { open(filename, openMode); }

	~BitStream() { close(); }

	void open(char const* filename, Mode openMode);

	std::fstream& getstream() { return stream; }

	int getbit();

	int getchar();

	void putbit(int bit);

	void putchar(int ch);

	bool is_open() { return stream.is_open(); }

	bool eof() { return  buff_is_empty() && stream.eof(); }

	bool buff_is_empty();

	void close();

	void put_buffer_bits();

	void put_buffer_char();
};


void BitStream::open(char const* filename, Mode openMode) {
	//init class variables
	mode = openMode;
	buffer = 0;
	// open file
	switch (openMode) {
	case read:
		stream.open(filename, std::ios::in | std::ios::binary);
		index = 0;
		break;
	case write:
		stream.open(filename, std::ios::out | std::ios::binary);
		index = BYTE;
		break;
	case app:
		stream.open(filename, std::ios::app | std::ios::binary);
		index = BYTE;
		break;
	}
}

int BitStream::getbit() {
	int bit = -1;
	if (mode != read) // current stream is not input stream
		return bit;

	if (buff_is_empty()) {
		// get new data
		buffer = stream.get();
		index = BYTE;
		// get bit from new data
		bit = getbit();
	}
	else {
		// get the MSB of buffer binary equivalent
		bit = buffer & 0X80 ? 1 : 0;

		// left shift buffer
		buffer <<= 1;
		--index;
	}
	return bit;
}

int BitStream::getchar() {
	if (mode != read) // current stream is not input stream
		return -1;

	// get 8 bit (equivalent to char) and return 
	int ch = 0;
	for (int i = BYTE - 1; i >= 0; i--) { // get 8 bit
		int bit = getbit();
		if (bit)
			ch += pow2(i);
	}
	return ch;
}

void BitStream::putbit(int bit) {
	// Basic Conditions
	if (mode != app && mode != write)
		return;

	if ((bit == 0 || bit == 1)) {
		index--;
		if (bit) //when bit == 0 , bit*pow(2,i) is 0 too
			buffer += pow2(index);
		if (index <= 0) { // buffer is full
			// write out buffer then reset it
			stream.put(buffer);
			buffer = 0;
			index = BYTE;
		}
	}
	else { // input is not binary
		putchar(bit);
	}
}

void BitStream::putchar(int ch) {
	// Basic Conditions
	if (mode != app && mode != write)
		return;
	// output all 8 bit char bit by bit 
	for (int i = 0; i < BYTE; i++) {
		int bit = ch & 0X80 ? 1 : 0;
		ch <<= 1;
		putbit(bit);
	}
}

bool BitStream::buff_is_empty() {
	// input stream: buffer is empty when index == 0
	// output stream: buffer is empty when index == BYTE
	if (mode == read)
		return  index == 0;
	else if (mode == none)
		return true;
	else
		return index == BYTE;
}

void BitStream::close() {
	// output reamaining dat befor closing
	put_buffer_char();
	stream.close();
}

void BitStream::put_buffer_bits() {
	while (index != BYTE) {
		// get the MSB of buffer binary equivalent
		int bit = buffer & 0X80 ? 1 : 0;
		stream.put(bit);

		// left shift buffer
		buffer <<= 1;
		++index;
	}
}

void BitStream::put_buffer_char() {
	if ((mode == app || mode == write) && !buff_is_empty()) {
		stream.put(buffer);
		buffer = 0;
		index = BYTE;
	}
}

int BitStream::pow2(unsigned int exp) {
	switch (exp) {
	case 0:  return POW2_0;
	case 1:  return POW2_1;
	case 2:  return POW2_2;
	case 3:  return POW2_3;
	case 4:  return POW2_4;
	case 5:  return POW2_5;
	case 6:  return POW2_6;
	case 7:  return POW2_7;
	default: return -1;
	}
}

#endif // !BIT_STREAM