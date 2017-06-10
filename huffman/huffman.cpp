#include "huffman.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

/* header format:
 * size_of_alphabet - 8 bit
 * alphabet in tree dfs order - size_of_alphabet * 8 bit
 * tree - size_of_alphabet * 2 - 2 bit
 *      go to left - 0
 *      go up while we right child and then go to right - 1
 * cnt_of_unused_bits (in end of file) - 3 bit
 * summary: size_of_alphabet * 10 + 9 bit
*/

huffman::huffman() {}

huffman::~huffman() {
    operator delete(buffer);
    if (tree_head) {
        delete tree_head;
    }
}

bool freq_cmp(huffman::tree_node* a, huffman::tree_node* b) {
    return a->frequency < b->frequency;
}

void huffman::build_tree(std::vector<long long> const& cnt) {
    std::vector<tree_node*> frequences;
    for (int i = 0; i < 256; i++) {
        if (cnt[i]) {
            size_of_alphabet++;
            frequences.push_back(new tree_node((unsigned char)i, cnt[i], nullptr, nullptr));
        }
    }
    std::sort(frequences.begin(), frequences.end(), freq_cmp);
    std::vector<tree_node*> sum;
    while (size_of_alphabet < 2) {
        ++size_of_alphabet;
        frequences.push_back(new tree_node(0, 0, nullptr, nullptr));
    }
    size_t freq_p = 0, sum_p = 0;
    for (int i = 0; i < (int)size_of_alphabet - 1; i++) {
        long long sum_freq_freq = (freq_p + 1 < frequences.size()
                                   ? frequences[freq_p]->frequency + frequences[freq_p + 1]->frequency
                                   : LLONG_MAX);
        long long sum_freq_sum = (freq_p < frequences.size() && sum_p < sum.size()
                                   ? frequences[freq_p]->frequency + sum[sum_p]->frequency
                                   : LLONG_MAX);
        long long sum_sum_sum = (sum_p + 1 < sum.size()
                                   ? sum[sum_p]->frequency + sum[sum_p + 1]->frequency
                                   : LLONG_MAX);
        if (sum_freq_freq <= sum_freq_sum && sum_freq_freq <= sum_sum_sum) {
            sum.push_back(new tree_node(0, sum_freq_freq, frequences[freq_p], frequences[freq_p + 1]));
            freq_p += 2;
        } else if (sum_freq_sum <= sum_freq_freq && sum_freq_sum <= sum_sum_sum) {
            sum.push_back(new tree_node(0, sum_freq_sum, frequences[freq_p], sum[sum_p]));
            freq_p++;
            sum_p++;
        } else { // sum_sum_sum <= sum_freq_freq && sum_sum_sum <= sum_freq_sum
            sum.push_back(new tree_node(0, sum_sum_sum, sum[sum_p], sum[sum_p + 1]));
            sum_p += 2;
        }
    }
    tree_head = sum[sum_p];
    std::vector<bool> v;
    calc_code_of_symbols(tree_head, v);
}

void huffman::calc_code_of_symbols(tree_node* node, std::vector<bool>& v) {
    if (node->l) {
        v.push_back(false);
        calc_code_of_symbols(node->l, v);
        v.pop_back();
        v.push_back(true);
        calc_code_of_symbols(node->r, v);
        v.pop_back();
        return;
    }
    unsigned char cur = node->val;
    code_of_symbols[cur].size = v.size();
    unsigned char c = 0;
    for (int i = 0; i < (int)v.size(); i++) {
        c = ((c << 1) | v[i]);
        if ((i & 7) == 7) {
            code_of_symbols[cur].a[i >> 3] = c;
        }
    }
    code_of_symbols[cur].a[v.size() >> 3] = c;
}

unsigned char* const huffman::get_buffer() {
    return buffer;
}

size_t huffman::get_size_of_buffer() {
    return size_of_buffer;
}

inline void huffman::write_to_buffer(bool t) {
    tail = ((tail << 1) | t);
    if (++size_of_tail == 8) {
        size_of_tail = 0;
        buffer[size_of_buffer++] = tail;
    }
}

inline void huffman::write_to_buffer(unsigned char c) {
    buffer[size_of_buffer++] = (tail << (8u - size_of_tail)) | (c >> size_of_tail);
    tail = c;
}

void huffman::dfs(tree_node* node, int path_len) {
    if (!node->l) {
        write_to_buffer(node->val);
        len += path_len * node->frequency;
        return;
    }
    dfs(node->l, path_len + 1);
    dfs(node->r, path_len + 1);
}

void huffman::dfs_write_tree_representation(tree_node *node) {
    if (node->l) {
        write_to_buffer(false);
        dfs_write_tree_representation(node->l);
        write_to_buffer(true);
        dfs_write_tree_representation(node->r);
    }
}

void huffman::calc_header() {
    write_to_buffer((unsigned char)size_of_alphabet);
    dfs(tree_head, 0);
    dfs_write_tree_representation(tree_head);
    cnt_of_unused_bits = (int)(8 - (len + (long long)size_of_alphabet * 10ll + 9) % 8) % 8;
    for (int i = 2; i >= 0; --i) {
        write_to_buffer((bool)((cnt_of_unused_bits >> i) & 1));
    }
}

void huffman::flush_buffer() {
    size_of_buffer = 0;
}

inline bool huffman::read_from_user_buffer() {
    bool res = static_cast<bool>((cur_user_buffer[pos_in_cur_user_buffer >> 3] >> (~pos_in_cur_user_buffer & 7)) & 1);
    ++pos_in_cur_user_buffer;
    return res;
}

unsigned char huffman::read_char_from_user_buffer() {
    unsigned char res = cur_user_buffer[pos_in_cur_user_buffer >> 3];
    pos_in_cur_user_buffer += 8;
    return res;
}

void huffman::encode(const unsigned char* const buf, size_t size, bool is_last_block) {
    flush_buffer();
    cur_user_buffer = buf;
    pos_in_cur_user_buffer = 0;
    if (need_to_calc_header) {
        calc_header();
        need_to_calc_header = false;
    }
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < (code_of_symbols[buf[i]].size >> 3); ++j) {
            write_to_buffer(code_of_symbols[buf[i]].a[j]);
        }
        for (int j = (code_of_symbols[buf[i]].size & 7) - 1; j >= 0; --j) {
            write_to_buffer((bool)((code_of_symbols[buf[i]].a[code_of_symbols[buf[i]].size >> 3] >> j) & 1));
        }
    }
    if (is_last_block) {
        while (size_of_tail != 0) {
            write_to_buffer(false);
        }
    }
}

void huffman::decode_tree(tree_node*& node, std::vector<unsigned char>& alphabet) {
    if (cnt_of_reading_symbols == (int)size_of_alphabet * 2 - 2) {
        node->val = alphabet[pos];
        return;
    }
    bool t = read_from_user_buffer();
    ++cnt_of_reading_symbols;
    if (!t) {
        node->l = new tree_node();
        node->r = new tree_node();
        decode_tree(node->l, alphabet);
        decode_tree(node->r, alphabet);
    } else {
        node->val = alphabet[pos++];
    }
}

void huffman::decode(const unsigned char* const buf, size_t size, bool is_last_block) {
    cur_user_buffer = buf;
    pos_in_cur_user_buffer = 0;
    if (need_to_calc_header) {
        size_of_alphabet = read_char_from_user_buffer();
        if (size * 8 < size_of_alphabet * 10 + 9) {
            throw std::invalid_argument("invalid format of header");
        }
        if (size_of_alphabet == 0) {
            size_of_alphabet = 256;
        }
        std::vector<unsigned char> alphabet(size_of_alphabet);
        for (int i = 0; i < (int)size_of_alphabet; i++) {
            alphabet[i] = read_char_from_user_buffer();
        }
        tree_head = new tree_node();
        decode_tree(tree_head, alphabet);
        for (int i = 0; i < 3; i++) {
            cnt_of_unused_bits = ((cnt_of_unused_bits << 1) | (read_from_user_buffer()));
        }
        need_to_calc_header = false;
        cur_node = tree_head;
    }
    flush_buffer();
    int cnt = size * 8 - pos_in_cur_user_buffer - cnt_of_unused_bits * is_last_block;
    while (cnt--) {
        if (read_from_user_buffer()) {
            cur_node = cur_node->r;
        } else {
            cur_node = cur_node->l;
        }
        if (!cur_node->l) {
            write_to_buffer(cur_node->val);
            cur_node = tree_head;
        }
    }
    if (is_last_block && cur_node != tree_head) {
        throw std::invalid_argument("can not decode data");
    }
}

