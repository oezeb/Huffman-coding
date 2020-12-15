#include <iostream>
#include <string>
#include "Huffman_Tree.h"
#include "frequencymap.h"
#include "bitstream.h"
#include "console.h"
#include "coding.h"

using namespace std;

int main(int argc, char* argv[]){
    
    //arguments list
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++)
        args.push_back(argv[i]);
    
    // new console instance
    Console cmd(args);

    if (cmd.getz()) {//compress
        std::ifstream in(cmd.getinputfile(), std::ios::in | std::ios::binary);
        if (!in.is_open())
            return -1;
        BitStream out(cmd.getoutputfile().c_str(), Mode::write);
        compress(in, out);
        in.close();
        out.close();
    }
    else if(cmd.getu()) {
        BitStream in(cmd.getinputfile().c_str(), Mode::read);
        if (!in.is_open())
            return -1;
        std::ofstream out(cmd.getoutputfile(), std::ios::out | std::ios::binary);
        decompress(in, out);
        in.close();
        out.close();
    }
    return 0;
}