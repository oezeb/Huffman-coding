#include"Huffman_Tree.h"
#include"frequencymap.h"
#include"bitstream.h"

void encodeData(std::ifstream& in, HuffmanCode codeMap, BitStream& out) {
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
            if (i->ch == curr) {
                code = i->code;
                break;
            }
        }

        //write data
        if (in.eof()) {
            out.putbit(0);
            for (auto &j : code)
                out.putbit(j);
        }
        else {
            out.putbit(1);
            for (auto &j : code)
                out.putbit(j);
        }
    }
}

void compress(std::ifstream& in, BitStream& out) {
    // Basic Conditions
    if (!in.is_open() || !out.is_open())
        return;

    // Get frequency map
    FrequencyMap freqMap = initFrequencyMap(in);

    // rewind in stream
    in.clear(); in.seekg(0);

    // Init priority queue
    std::priority_queue<HuffmanNode> p_queue;
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

void decodeData(BitStream& in, HuffmanTree Tree, std::ofstream& out) {
    while (true) {
        char ch = in.getbit();
        out.put(nextVex(in, Tree)->ch);
        if (ch == 0)
            break;
    }
}

void decompress(BitStream& in, std::ofstream& out) {
    // Basic conditions
    if (!in.is_open() || !out.is_open())
        return;


    // get stored Huffman Tree from file
    HuffmanTree HT = getHuffmanTree(in);

    // Decode Data
    decodeData(in, HT, out);
}
