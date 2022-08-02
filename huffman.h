#ifndef HUFFMAN
#define HUFFMAN

#include <queue>
#include <deque>
#include <unordered_map>
#include <iostream>

#include "bitstream.h"

/*
* @author: KingEze
* @mail: kingeze@mail.ustc.edu.cn
* @date: 2021/01/12
*/

namespace huffman {
    typedef std::deque<bit> code;
    typedef std::unordered_map<int, size_t> freq_map;
    typedef std::unordered_map<int, code> code_map;

    struct node {
        int value;
        size_t freq;
        node* left, * right;
        node(int value, size_t freq, node* left = NULL, node* right = NULL)
            : value(value), freq(freq), left(left), right(right) {}
        
        class freq_comparison {
        public:
            bool operator()(const node* node1, const node* node2) {
                return node1->freq > node2->freq;
            }
        };

        typedef std::priority_queue<node*, std::vector<node*>, node::freq_comparison> priority_queue;
    };

    void compress(const char* in_file, const char* out_file);
    void decompress(const char* in_file, const char* out_file);

    node* _buildtree(node::priority_queue& p_queue);
    node* _parent(node* root, const node* child);
    void _initcodemap(node* root, code_map &_code_map, code& code);
    std::pair<node*, size_t> _maxdepth(node* root, size_t depth = 0);
    void _storetree(node* root, bitstream& out);
    node* _retrievetree(bitstream& in);
    void _printtree(node* root, std::string indent = "");
}

void huffman::_printtree(node* root, std::string indent) {
    if (root == NULL) return;
    std::cout << indent << root->value << std::endl;
    _printtree(root->left, indent + "  ");
    _printtree(root->right, indent + "  ");
}

void huffman::compress(const char* infile, const char* outfile) {
    std::cout << "compressing " << infile << "-->" << outfile << " ..." << std::endl;
    
    bitstream in(infile, openmode::read);
    if(!in.is_open()) return;
    
    freq_map _freq_map;
    std::vector<int> contents;
    while (true) {
        int byte = in.getbyte();
        if (in.eof()) break;

        contents.push_back(byte);
        _freq_map[byte]++;
    }
    
    node::priority_queue p_queue;
    for (auto& e : _freq_map)
        p_queue.push(new node(e.first, e.second));
    node* eof_node = new node('\0', 0);
    p_queue.push(eof_node);

    node* tree = _buildtree(p_queue);
    
    bitstream out(outfile, openmode::write);
    _storetree(tree, out);

    node* parent = _parent(tree, eof_node);
    parent->left = NULL;

    code _code;
    code_map _code_map;
    _initcodemap(tree, _code_map, _code);

    auto eof_code = _code_map[parent->right->value];
    eof_code.back() = 0;

    for (auto& e : contents)
    for (auto& ee : _code_map[e])
        out.putbit(ee);

    for (auto& e : eof_code)
        out.putbit(e);
        
    out.close();
}

void huffman::decompress(const char* infile, const char* outfile) {
    std::cout << "decompressing " << infile << "-->" << outfile << " ..." << std::endl;

    bitstream in(infile, openmode::read);
    if(!in.is_open()) return;

    node* tree = _retrievetree(in);

    auto e = _maxdepth(tree);
    auto eof = _parent(tree, e.first)->left;

    node* root = tree;
    bitstream out(outfile, openmode::write);
    while (root && root != eof && !in.stdeof()) {
        if (root->left == NULL && root->right == NULL) {
            out.putbyte(root->value);
            root = tree;
        }

        if (in.getbit()) root = root->right;
        else root = root->left;
    }

    out.close();
}
    
std::pair<huffman::node*, size_t> huffman::_maxdepth(node* root, size_t depth) {
    if (root == NULL) return {nullptr, depth};

    auto left = _maxdepth(root->left, depth + 1);
    auto right = _maxdepth(root->right, depth + 1);

    if (left.first == NULL && right.first == NULL)
        return {root, depth};
    else if (left.first == NULL)
        return right;
    else if (right.first == NULL)
        return left;
    else if (left.second > right.second)
        return left;
    else
        return right;
}

huffman::node* huffman::_buildtree(node::priority_queue& p_queue) {
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

    return _buildtree(p_queue);
}

huffman::node* huffman::_parent(node* root, const node* child) {
    if (root == NULL) return NULL;
    if (root->left == child || root->right == child)
        return root;
    auto left = _parent(root->left, child);
    if (left) return left;
    auto right = _parent(root->right, child);
    if (right) return right;
    return NULL;
} 

void huffman::_storetree(node* root, bitstream& out) {
    if (root == NULL) return;
    // for leaves write 1 then leaf value
    // for non-leaves write 0
    if (root->left == NULL && root->right == NULL) { //isleaf
        out.putbit(1);
        out.putbyte(root->value);
    }
    else {
        out.putbit(0);
        _storetree(root->left, out);
        _storetree(root->right, out);
    }
}

huffman::node* huffman::_retrievetree(bitstream& in) {
    // 1-> leaf; 0->non-leaf
    if (in.getbit()) {
        int byte = in.getbyte();
        return new node(byte, 1);
    }
    else {
        node* left = _retrievetree(in);
        node* right = _retrievetree(in);
        return new node('\0', 1, left, right);
    }
}

void huffman::_initcodemap(node* root, code_map &_code_map, code& code) {
    if(root==NULL) return;
        
    if (root->left == NULL && root->right == NULL) {
        _code_map[root->value] = code;
        return;
    }
        
    code.push_back(0);
    _initcodemap(root->left, _code_map, code);
    code.pop_back();

    code.push_back(1);
    _initcodemap(root->right, _code_map, code);
    code.pop_back();
}

#endif // !HUFFMAN