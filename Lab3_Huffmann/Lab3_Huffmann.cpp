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

    // find and write code
    while (true) {
        // get char
        char ch = in.get();
        if (in.eof()) break;
        //find code
        for (auto& i : codeMap) {
            if (i->ch == ch) {
                for (auto j : i->code)
                    out.putbit(j);
                break;
            }
        }
    }
}

void decodeData(BitStream& in, HuffmanTree Tree, ofstream& out) {
    while (true) {
        HuffmanNode* node = nextVex(in, Tree);
        if (!node || node->freq == 0)
            break;
        out.put(node->ch);
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

    //print tree leaves number
    out.putchar(HC.size());

    // store Tree leaves
    storeHuffmanLeaf(HT, out);

    //
    HuffmanNode* tmp = NULL;
    for (auto& i : HC) {
        if (i->freq == 0) {
            tmp = i;
            HC.remove(i);
            break;
        }
    }

    // Encode data
    encodeData(in, HC, out);

    //
    if (tmp)
        for (auto& i : tmp->code)
            out.putbit(i);
}

void decompress(BitStream& in, ofstream& out) {
    // Basic conditions
    if (!in.is_open() || !out.is_open())
        return;

    //get leaves number
    int numb = in.getchar();

    // get stored Huffman leaves data from file
    priority_queue<HuffmanNode> p_queue;
    for (int i = 0; i < numb; i++) {
        char ch = in.getchar();
        int freq = in.getchar();
        p_queue.push(HuffmanNode(ch, freq));
    }

    // Init Huffman Tree
    HuffmanTree HT = initHuffmanTree(p_queue);

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