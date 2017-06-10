#include <iostream>
#include "huffman.h"
#include <fstream>
#include <string.h>
#include <vector>
#include <ctime>

using namespace std;

size_t const size_of_block = 4096;

long long encode(const char* input, const char* output) {
    FILE* inputFile = fopen(input, "rb");
    if (!inputFile) {
        throw std::invalid_argument("can not open input file");
    }
    FILE* outputFile = fopen(output, "wb");
    if (!outputFile) {
        throw std::invalid_argument("can not open output file");
    }
    unsigned char* buf = static_cast<unsigned char*> (operator new(size_of_block));
    vector<long long> cnt(256);
    size_t size;
    size_t cnt_of_blocks = 0;
    while ((size = fread(buf, 1, size_of_block, inputFile)) == size_of_block) {
        for (size_t j = 0; j < size_of_block; ++j) {
            ++cnt[buf[j]];
        }
        ++cnt_of_blocks;
    }
    for (size_t j = 0; j < size; ++j) {
        ++cnt[buf[j]];
    }
    long long len_of_file = (long long) cnt_of_blocks * size_of_block + size;
    if (size) {
        ++cnt_of_blocks;
    }
    rewind(inputFile);

    huffman huff;
    huff.build_tree(cnt);
    for (size_t i = 0; i < cnt_of_blocks; ++i) {
        size = fread(buf, 1, size_of_block, inputFile);
        try {
            huff.encode(buf, size, (i == cnt_of_blocks - 1));
        } catch (std::invalid_argument const& e) {
            operator delete(buf);
            throw e;
        }
        fwrite((char*)huff.get_buffer(), 1, huff.get_size_of_buffer(), outputFile);
    }
    fclose(inputFile);
    fclose(outputFile);
    operator delete(buf);
    return len_of_file;
}

long long decode(const char* input, const char* output) {
    FILE* inputFile = fopen(input, "rb");
    if (!inputFile) {
        throw std::invalid_argument("can not open input file");
    }
    FILE* outputFile = fopen(output, "wb");
    if (!outputFile) {
        throw std::invalid_argument("can not open output file");
    }
    unsigned char* buf = static_cast<unsigned char*> (operator new(size_of_block));
    size_t size;
    size_t cnt_of_blocks = 0;
    while ((size = fread(buf, 1, size_of_block, inputFile)) == size_of_block) {
        ++cnt_of_blocks;
    }
    long long len_of_file = (long long)cnt_of_blocks * size_of_block + size;
    if (size) {
        ++cnt_of_blocks;
    }
    rewind(inputFile);

    huffman huff;
    for (size_t i = 0; i < cnt_of_blocks; ++i) {
        size = fread(buf, 1, size_of_block, inputFile);
        try {
            huff.decode(buf, size, (i == cnt_of_blocks - 1));
        } catch (std::invalid_argument const& e) {
            operator delete(buf);
            throw e;
        }
        fwrite((char*)huff.get_buffer(), 1, huff.get_size_of_buffer(), outputFile);
    }
    fclose(inputFile);
    fclose(outputFile);
    operator delete(buf);
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
            long long len_of_file = encode(argv[2], argv[3]);
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
            long long len_of_file = decode(argv[2], argv[3]);
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
