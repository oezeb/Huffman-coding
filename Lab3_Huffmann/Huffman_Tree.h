#ifndef HUFFMAN_TREE
#define HUFFMAN_TREE

#include<vector>
#include<list>
#include <queue>
#include"bitstream.h"

/*
* @author: KingEze
* @mail: kingeze@mail.ustc.edu.cn
* @date: 2020/12/11 6:32 AM
* @description: Huffman tree tools (Create tree, get code Map, store(get) Tree into(from) file...)
*/


typedef struct HuffmanNode {
    char ch;
    int freq;
    std::vector<int> code;
    HuffmanNode* left, * right;
    HuffmanNode(char ch = '?', int freq = 1, HuffmanNode* left = NULL, HuffmanNode* right = NULL)
        : ch(ch), freq(freq), left(left), right(right) {}

}HuffmanNode, * HuffmanTree;

typedef std::list<HuffmanNode*> HuffmanCode;


// Can be used to sort Huffman code
bool HuffmanCodeCompare(HuffmanNode* const& node1, HuffmanNode* const& node2) {
    return node1->code.size() < node2->code.size();
}

// Used in priority queue
bool operator<(HuffmanNode const& node1, HuffmanNode const& node2) {
    return node1.freq > node2.freq;
}

//
bool operator==(HuffmanNode const& node1, HuffmanNode const& node2) {
    return node1.ch == node2.ch &&
        node1.code == node2.code &&
        node1.freq == node2.freq &&
        node1.left == node2.left &&
        node1.right == node2.right;
}

// Create Huffman Tree from priority queue
HuffmanTree initHuffmanTree(std::priority_queue<HuffmanNode> p_queue) {

    // Basic conditions
    if (p_queue.empty()) return nullptr;

    // pop first node
    HuffmanNode* node1 = new HuffmanNode(p_queue.top());
    p_queue.pop();

    // Basic conditions
    if (p_queue.empty()) return node1;

    // pop second node
    HuffmanNode* node2 = new HuffmanNode(p_queue.top());
    p_queue.pop();

    // create first and second node parent node
    // which freq is the sum of children freq 
    p_queue.push(HuffmanNode('*', node1->freq + node2->freq, node1, node2));

    return initHuffmanTree(p_queue);
}


// Init given Huffman tree vertices code
HuffmanCode initHuffmanCode(HuffmanTree& T) {
    // Basic Conditions
    if (!T) //empty tree
        return HuffmanCode();
    if (!T->left && !T->right) //no child
        return HuffmanCode(1, T);

    // get current node code;
    std::vector<int> code = T->code;

    //init children codes
    // left child code equal to parent code+0
    if (T->left != nullptr) {
        T->left->code = code;
        T->left->code.push_back(0);
    }

    // right child code equal to parent code+1
    if (T->right != nullptr) {
        T->right->code = code;
        T->right->code.push_back(1);
    }

    // get result
    HuffmanCode HC = initHuffmanCode(T->left);
    HuffmanCode tmp = initHuffmanCode(T->right);
    HC.insert(HC.end(), tmp.begin(), tmp.end());

    return HC;
}

void storeHuffmanLeaf(HuffmanTree& T, BitStream& out) {
    //Basic Conditions
    if (!T || !out.is_open()) 
        return;
    if (!T->left && !T->right) { //leaf
        out.putchar(T->ch);
        out.putchar(T->freq);
        return;
    }
    storeHuffmanLeaf(T->left, out);
    storeHuffmanLeaf(T->right, out);
}

void storeHuffmanTree(HuffmanTree& T, BitStream& out) {
    // Basic conditions
    if (!T) return;

    // for leaves write 1 then leaf value
    // for non-leaves write 0
    if (!T->left && !T->right) { //isleaf
        out.putbit(1);
        out.putchar(T->ch);
    }
    else {
        out.putbit(0);
        storeHuffmanTree(T->left, out);
        storeHuffmanTree(T->right, out);
    }
}

HuffmanTree getHuffmanTree(BitStream& out) {
    // 1-> leaf; 0->non-leaf
    if (out.getbit() == 1) {
        char ch = out.getchar();
        return new HuffmanNode(ch);
    }
    else {
        HuffmanNode* left = getHuffmanTree(out);
        HuffmanNode* right = getHuffmanTree(out);
        return new HuffmanNode('?', 1, left, right);
    }
}

HuffmanNode* nextVex(BitStream& in, HuffmanTree Tree) {
    // Basic conditions
    if (!in.is_open() || in.getstream().eof() || !Tree)
        return NULL;

    // Get char
    if (!Tree->left && !Tree->right)  //leaf
        return Tree;

    // Traverse tree
    if (in.getbit() == 0)
        return nextVex(in, Tree->left);
    else
        return nextVex(in, Tree->right);
}

#endif // !HUFFMAN_TREE
