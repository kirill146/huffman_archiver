#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <vector>
#include <cstdlib>

class huffman
{
private:
    struct tree_node {
        unsigned char val;
        long long frequency = 0;
        tree_node *l = nullptr, *r = nullptr;

        tree_node() {}
        tree_node(unsigned char val, long long frequency, tree_node* l, tree_node* r)
            : val(val)
            , frequency(frequency)
            , l(l)
            , r(r)
        {}
        ~tree_node() {
            if (l) {
                delete l;
                delete r;
            }
        }
    };
    struct code_of_symbol {
        size_t size = 0;
        unsigned char a[32];
    };

    friend bool freq_cmp(tree_node* a, tree_node* b);
public:
    void write_to_buffer(bool t);
    void write_to_buffer(unsigned char c);


public:
    huffman();
    ~huffman();
    void build_tree(std::vector<long long> const&);
    void calc_header();
    void encode(const unsigned char* const buf, size_t size, bool is_last_block);
    void decode(const unsigned char* const buf, size_t size, bool is_last_block);
    unsigned char* const get_buffer();
    size_t get_size_of_buffer();
    void dfs(tree_node* node, int path_len);
    void dfs_write_tree_representation(tree_node* node);
    void flush_buffer();
    bool read_from_user_buffer();
    unsigned char read_char_from_user_buffer();
    void decode_tree(tree_node*& node, std::vector<unsigned char>& alphabet);
    void calc_code_of_symbols(tree_node* node, std::vector<bool>& v);

public:
    tree_node* tree_head = nullptr;
    long long len = 0;
    size_t const max_size_of_buffer = 4 * 1024 * 255;
    unsigned char* const buffer = (unsigned char*)operator new(max_size_of_buffer);
    const unsigned char* cur_user_buffer = nullptr;
    int pos_in_cur_user_buffer = 0;
    size_t size_of_buffer = 0;
    unsigned char tail = 0;
    size_t size_of_tail = 0;
    size_t size_of_alphabet = 0;
    bool need_to_calc_header = true;
    int cnt_of_reading_symbols = 0;
    int cnt_of_unused_bits = 0;
    code_of_symbol code_of_symbols[256];
    int pos = 0;
    tree_node* cur_node;
};

#endif // HUFFMAN_H
