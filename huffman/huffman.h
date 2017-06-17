#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <vector>
#include <cstdlib>

class huffman
{
private:
    struct tree_node {
        u_char val;
        int64_t frequency = 0;
        tree_node *l = nullptr, *r = nullptr;

        tree_node() {}
        tree_node(u_char val, int64_t frequency, tree_node* l, tree_node* r)
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
        u_char a[32];
    };

    huffman(huffman const&) = delete;
    void write_to_buffer(bool t);
    void write_to_buffer(u_char c);
    void calc_header();
    void dfs(tree_node* node, int path_len);
    void dfs_write_tree_representation(tree_node* node);
    bool read_from_user_buffer();
    u_char read_char_from_user_buffer();
    void decode_tree(tree_node*& node, std::vector<u_char>& alphabet);
    void calc_code_of_symbols(tree_node* node, std::vector<bool>& v);

    tree_node* tree_head = nullptr;
    int64_t len = 0;
    size_t const max_size_of_buffer = 4 * 1024 * 255;
    std::vector<u_char> buffer;
    const u_char* cur_user_buffer = nullptr;
    int pos_in_cur_user_buffer = 0;
    size_t size_of_buffer = 0;
    u_char tail = 0;
    size_t size_of_tail = 0;
    size_t size_of_alphabet = 0;
    bool need_to_calc_header = true;
    int cnt_of_reading_symbols = 0;
    int cnt_of_unused_bits = 0;
    code_of_symbol code_of_symbols[256];
    int pos = 0;
    tree_node* cur_node;

    friend bool freq_cmp(tree_node* a, tree_node* b);

public:
    huffman();
    ~huffman();
    u_char* const get_buffer();
    size_t get_size_of_buffer();
    void build_tree(std::vector<int64_t> const&);
    void encode(const u_char* const buf, size_t size, bool is_last_block);
    void decode(const u_char* const buf, size_t size, bool is_last_block);
};

#endif // HUFFMAN_H
