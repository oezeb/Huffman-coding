#ifndef HUFFMAN
#define HUFFMAN

#include <queue>
#include <deque>
#include <unordered_map>

#include "bitstream.h"

/*
* @author: KingEze
* @mail: kingeze@mail.ustc.edu.cn
* @date: 2021/01/12
*/

typedef std::deque<bit> code;
typedef std::unordered_map<int, int> freq_map;
typedef std::unordered_map<int, code> code_map;

struct node {
    int value;
    size_t freq;
    node* left, * right;
    node(int value, size_t freq = 1, node* left = NULL, node* right = NULL)
        : value(value), freq(freq), left(left), right(right) {}
    
    class freq_comparison {
    public:
        bool operator()(const node* node1, const node* node2) {
            return node1->freq > node2->freq;
        }
    };

    typedef std::priority_queue<node*, std::vector<node*>, node::freq_comparison> priority_queue;
};



class huffman {
public:
    static void compress(const char* infile, const char* outfile) {
        std::cout << "compressing " << infile << "-->" << outfile << " ..." << std::endl;

        bitstream in(infile, openmode::read);
        if(!in.is_open())
            return;

        freq_map _freq_map;
        std::vector<int> contents;
        while (true) {
            int byte = in.getbyte();
            if (in.eof())
                break;
            contents.push_back(byte);
            _freq_map[byte]++;
        }

        node::priority_queue p_queue;
        for (auto& e : _freq_map)
            p_queue.push(new node(e.first, e.second));

        node* tree = buildtree(p_queue);

        bitstream out(outfile, openmode::write);
        storetree(tree, out);

        writesize(contents.size(), out);
        
        code _code;
        code_map _code_map;
        initcodemap(tree, _code_map, _code);

        for (auto& e : contents)
            for (auto& ee : _code_map[e])
                out.putbit(ee);
    }

    static void decompress(const char* infile, const char* outfile) {
        std::cout << "decompressing " << infile << "-->" << outfile << " ..." << std::endl;

        bitstream in(infile, openmode::read);
        if(!in.is_open())
            return;

        node* tree = retrievetree(in);

        size_t size = retrievesize(in);

        node* root = tree;
        size_t i = 0;
        bitstream out(outfile, openmode::write);
        while (i < size && root && !in.stdeof()) {
            if (root->left == NULL && root->right == NULL) {
                out.putbyte(root->value);
                root = tree;
                i++;
            }

            if (in.getbit()) root = root->right;
            else root = root->left;
        }
    }

private:
    static node* buildtree(node::priority_queue& p_queue) {
        if (p_queue.empty())
            return NULL;

        // get first node
        node* node1 = p_queue.top();
        p_queue.pop();

        if (p_queue.empty()) return node1;

        // get second node
        node* node2 = p_queue.top();
        p_queue.pop();

        // create first and second node parent node
        // which freq is the sum of children freq 
        p_queue.push(new node('\0', node1->freq + node2->freq, node1, node2));

        return buildtree(p_queue);
    }

    static void storetree(node* root, bitstream& out) {
        if (root == NULL) return;
        // for leaves write 1 then leaf value
        // for non-leaves write 0
        if (root->left == NULL && root->right == NULL) { //isleaf
            out.putbit(1);
            out.putbyte(root->value);
        }
        else {
            out.putbit(0);
            storetree(root->left, out);
            storetree(root->right, out);
        }
    }

    static node* retrievetree(bitstream& in) {
        // 1-> leaf; 0->non-leaf
        if (in.getbit()) {
            int byte = in.getbyte();
            return new node(byte);
        }
        else {
            node* left = retrievetree(in);
            node* right = retrievetree(in);
            return new node('\0', 1, left, right);
        }
    }

    static void writesize(size_t size, bitstream& out) {
        for (int i = 0; i < 4; i++) {
            out.putbyte(size & 0XFF);
            size >>= BYTE;
        }
    }

    static size_t retrievesize(bitstream& in) {
        size_t size = 0;
        for (int i = 0; i < 4; i++) {
            size_t value = in.getbyte();
            value <<= i * BYTE;
            size |= value;
        }
        return size;
    }

    static void initcodemap(node* root, code_map &_code_map, code& code) {
        if(root==NULL)
            return;
        
        if (root->left == NULL && root->right == NULL) {
            _code_map[root->value] = code;
            return;
        }
        
        code.push_back(0);
        initcodemap(root->left, _code_map, code);
        code.pop_back();

        code.push_back(1);
        initcodemap(root->right, _code_map, code);
        code.pop_back();
    }
};

#endif // !HUFFMAN