#include <iostream>
#include "huffman.h"
#include <fstream>
#include <string.h>
#include <vector>
#include <ctime>

using namespace std;

size_t const size_of_block = 4096;

int64_t encode(const char* input, const char* output) {
    ifstream fin;
    fin.open(input, std::ios_base::binary);
    if (!fin) {
        throw std::invalid_argument("can not open input file");
    }
    ofstream fout;
    fout.open(output, std::ios_base::binary);
    if (!fout) {
        throw std::invalid_argument("can not open output file");
    }
    vector<u_char> buf(size_of_block);
    fin.seekg(0, fin.end);
    int64_t len_of_file = fin.tellg();
    fin.seekg(0, fin.beg);
    int64_t cnt_of_blocks = len_of_file / size_of_block + (bool)(len_of_file % size_of_block);
    int64_t len_of_last_block = len_of_file % size_of_block;
    if (len_of_last_block == 0) {
        len_of_last_block = size_of_block;
    }

    vector<int64_t> cnt(256);
    for (int64_t i = 0; i < cnt_of_blocks; ++i) {
        int size = (i == cnt_of_blocks - 1 ? len_of_last_block : size_of_block);
        fin.read((char*)buf.data(), size);
        for (int j = 0; j < size; ++j) {
            ++cnt[buf[j]];
        }
    }
    fin.seekg(0, fin.beg);
    huffman huff;
    huff.build_tree(cnt);
    for (int64_t i = 0; i < cnt_of_blocks; ++i) {
        int64_t size = (i == cnt_of_blocks - 1 ? len_of_last_block : size_of_block);
        fin.read((char*)buf.data(), size);
        try {
            huff.encode(buf.data(), size, (i == cnt_of_blocks - 1));
        } catch (std::invalid_argument const& e) {
            throw e;
        }
        fout.write((char*)huff.get_buffer(), huff.get_size_of_buffer());
    }
    return len_of_file;
}

int64_t decode(const char* input, const char* output) {
    ifstream fin;
    fin.open(input, std::ios_base::binary);
    if (!fin) {
        throw std::invalid_argument("can not open input file");
    }
    ofstream fout;
    fout.open(output, std::ios_base::binary);
    if (!fout) {
        throw std::invalid_argument("can not open output file");
    }
    vector<u_char> buf(size_of_block);
    fin.seekg(0, fin.end);
    int64_t len_of_file = fin.tellg();
    fin.seekg(0, fin.beg);
    int64_t cnt_of_blocks = len_of_file / size_of_block + (bool)(len_of_file % size_of_block);
    int64_t len_of_last_block = len_of_file % size_of_block;
    if (len_of_last_block == 0) {
        len_of_last_block = size_of_block;
    }
    huffman huff;
    for (int64_t i = 0; i < cnt_of_blocks; ++i) {
        int size = (i == cnt_of_blocks - 1 ? len_of_last_block : size_of_block);
        fin.read((char*)buf.data(), size);
        try {
            huff.decode(buf.data(), size, (i == cnt_of_blocks - 1));
        } catch (std::invalid_argument const& e) {
            throw e;
        }
        fout.write((char*)huff.get_buffer(), huff.get_size_of_buffer());
    }
    return len_of_file;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "format: encode|decode input output" << endl;
        return 0;
    }
    cout.precision(5);
    clock_t timer = clock();
    if (strcmp(argv[1], "encode") == 0) {
        try {
            int64_t len_of_file = encode(argv[2], argv[3]);
            float time = (float)(clock() - timer) / CLOCKS_PER_SEC;
            double speed = (double) len_of_file / 1024 / 1024 / time;
            std::cout << "encode was finished in " << time << "s (" << speed << " Mb/s)" << std::endl;
        } catch(std::invalid_argument const& e) {
            cerr << "can not encode file: " << e.what() << endl;
        }
        return 0;
    }
    if (strcmp(argv[1], "decode") == 0) {
        try {
            int64_t len_of_file = decode(argv[2], argv[3]);
            float time = (float) (clock() - timer) / CLOCKS_PER_SEC;
            double speed = (double) len_of_file / 1024 / 1024 / time;
            std::cout << "decode was finished in " << time << "s (" << speed << " Mb/s)" << std::endl;
        } catch (std::invalid_argument const& e) {
            cerr << "can not decode file: " << e.what() << endl;
        }
        return 0;
    }
    cout << "format: encode|decode input output" << endl;
    return 0;
}
