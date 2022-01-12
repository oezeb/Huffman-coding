#ifndef BIT_STREAM
#define BIT_STREAM

#include <stdexcept>
#include<fstream>

/*
* @author: KingEze
* @mail: kingeze@mail.ustc.edu.cn
* @date: 2020/12/11 15:00 AM
*/

#define BYTE  8

typedef bool bit;

enum openmode { read, write, append };

const size_t POW_ARR[] = { 1, 2, 4, 8, 16, 32, 64, 128}; // POW_ARR[i] contains 2^i 

class bitstream {
private:
	std::fstream _stream;
	openmode _mode;
	int _buffer;
	size_t _buffercount;

public:
	bitstream(const char* filename, openmode mode)
		: _mode(mode), _buffer(0), _buffercount(0) {
		// open stream
		switch(mode) {
			case read: 
				_stream.open(filename, std::ios::in | std::ios::binary); 
				break;
			case write:
				_stream.open(filename, std::ios::out | std::ios::binary); 
				break;
			case append: 
				_stream.open(filename, std::ios::app | std::ios::binary); 
				break;
		}
	}

	~bitstream() { close(); }

	bit getbit() {
		if(_mode == write || _mode == append)
			throw std::runtime_error("getbit() failed");

		if (_buffercount == 0) { // buffer is empty
			_buffer = _stream.get();
			_buffercount = BYTE;
		}
		// get the MSB of buffer
		bit ans = _buffer & 0X80 ? 1 : 0;

		// left shift buffer
		_buffer <<= 1;
		--_buffercount;
		return ans;
	}

	int getbyte() {
		if (_buffercount == 0) // buffer is empty
			return _stream.get();

		// get 8 bit (1 byte)
		int byte = 0;
		for (int i = BYTE - 1; i >= 0; i--) {
			bit value = getbit();
			if (value)
				byte += POW_ARR[i];
		}
		return byte;
	}

	void putbit(bit value) {
		if(_mode == read)
			throw std::runtime_error("getbit() failed");
		
		++_buffercount;
		if (value)
			_buffer += POW_ARR[BYTE-_buffercount];

		if(_buffercount == BYTE) { // buffer is full
			_stream.put(_buffer);
			_buffer = 0;
			_buffercount = 0;
		}
	}

	void putbyte(int byte) {
		if(_buffercount == 0) // buffer is empty
			_stream.put(byte);
		else
		// output all 8 bit char bit by bit 
		for (int i = 0; i < BYTE; i++) {
			// get the MSB of ch
			bit value = byte & 0X80 ? 1 : 0;
			byte <<= 1;
			putbit(value);
		}
	}

	void close() {
		if (_buffercount) // buffer is not empty
			if (_mode == write || _mode == append)
				_stream.put(_buffer);
		_stream.close();
	}

	bool is_open() { return _stream.is_open(); }
	
	bool eof() { return  _buffercount == 0 && stdeof(); }

	bool stdeof() { return _stream.eof(); }
};

#endif // !BIT_STREAM
