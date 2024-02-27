#ifndef _SHAKE128_H
#define _SHAKE128_H

#include <gmpxx.h>
#include <iostream>
#include <vector>
#include <array>

namespace shake128
{
    //WARNING : pads the passed array, size will be ceiled to next multiple of 8
    std::vector<uint64_t> hash(std::vector<uint8_t> &data, int size);
    std::vector<uint64_t> hash(const mpz_class &x, int size);

    std::string hash_to_string(const std::vector<uint64_t> &hash, int size);
    std::string hash_into_string(std::vector<uint8_t> &data, int size);
    
    std::vector<uint8_t> hash_to_bytes(const std::vector<uint64_t> &hash);
}

#endif