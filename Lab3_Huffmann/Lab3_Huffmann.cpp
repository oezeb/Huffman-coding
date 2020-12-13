#include <iostream>
#include<string>
#include"Huffman_Tree.h"
#include"frequencymap.h"
#include"bitstream.h"

using namespace std;


void encodeData(ifstream& in, HuffmanCode codeMap, BitStream& out) {
    // Basic Conditions
    if (!in.is_open() || !out.is_open())
        return;

    char next = in.get();
    // find and write code
    while (!in.eof()) {
        // get char
        char curr = next;
        next = in.get();
        //find code
        std::vector<int> code;
        for (auto& i : codeMap) {
            if (i.ch == curr) {
                code = i.code;
                break;
            }
        }

        //write data

        if (in.eof()) {
            out.putbit(0);
            for (auto j : code)
                out.putbit(j);
        }
        else {
            out.putbit(1);
            for (auto j : code)
                out.putbit(j);
        }
    }
}

char nextChar(BitStream& in, HuffmanTree Tree) {
    // Basic conditions
    if (!in.is_open() || in.getstream().eof() || !Tree)
        return EOF;

    // Get char
    if (!Tree->left && !Tree->right)  //leaf
        return Tree->ch;

    // Traverse tree
    if (in.getbit() == 0)
        return nextChar(in, Tree->left);
    else
        return nextChar(in, Tree->right);
}

void decodeData(BitStream& in, HuffmanTree Tree, ofstream& out) {
    while (true) {
        char ch = in.getbit();
        out.put(nextChar(in, Tree));
        if (ch == 0)
            break;
    }
}

void compress(ifstream& in, BitStream& out) {
    // Basic Conditions
    if (!in.is_open() || !out.is_open())
        return;
    
    // Get frequency map
    FrequencyMap freqMap = initFrequencyMap(in);

    // rewind in stream
    in.clear(); in.seekg(0);

    // Init priority queue
    priority_queue<HuffmanNode> p_queue;
    for (auto &i : freqMap)  // first->char ,second-> freq
        p_queue.push(HuffmanNode(i.first, i.second));

    // Init Huffman Tree
    HuffmanTree HT = initHuffmanTree(p_queue);

    // Get Huffman Code
    HuffmanCode HC = initHuffmanCode(HT);
    HC.sort(HuffmanCodeCompare); // sort Huffman Code List

    // store Huffman Tree
    storeHuffmanTree(HT, out);

    // Encode data
    encodeData(in, HC, out);
}

void decompress(BitStream& in, ofstream& out) {
    // Basic conditions
    if (!in.is_open() || !out.is_open())
        return;


    // get stored Huffman Tree from file
    HuffmanTree HT = getHuffmanTree(in);

    // Decode Data
    decodeData(in, HT, out);
}

string outputfileName(string inputfile) {
    inputfile.insert(inputfile.find('.'), "out");
    while (!inputfile.empty() && inputfile[inputfile.size() - 1] != '.')
        inputfile.pop_back();
    inputfile.pop_back();
    return inputfile.empty() ? "empty" : inputfile;
}

int main(int argc, char* argv[]){
    string inFile, outFile;
    if (argc > 1) 
        inFile = argv[1];
    else {
        cout << "File name : ";
        char tmp[100];
        cin.getline(tmp, 100);
        inFile = tmp;
    }

    bool cprss = true;
    cout << "compress(1) or decompress(0) ? ";
    cin >> cprss;

    if (cprss) {
        ifstream in(inFile, ios::in | ios::binary);
        if (!in.is_open()) return -1;
        outFile = inFile + ".hc";
        BitStream out(outFile.c_str(), Mode::write);
        compress(in, out);
        in.close();
        out.close();
    }
    else {
        BitStream in(inFile.c_str(), Mode::read);
        if (!in.is_open()) return -1;
        outFile = outputfileName(string(inFile));
        ofstream out(outFile, ios::binary);
        decompress(in, out);
        in.close();
        out.close();
    }
}