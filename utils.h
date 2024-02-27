#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <vector>
#include <array>
#include <gmpxx.h>

namespace utils
{
    inline std::string reverse_endianness(const std::string &s)
    {
        std::string output;
        for (int i = 0;i < s.length()/2;i++)
        {
            int rev_idx = (s.length() / 2) - 1 - i;
            output += s[rev_idx*2];
            output += s[rev_idx*2 + 1];
        }
        return output;
    }

    inline std::string format_hex(const std::string &s)
    {
        std::string output;
        for (int i = 0;i < s.length()/2 - 1;i++)
        {
            output += s[i * 2];
            output += s[i * 2 + 1];
            output += ":";
        }

        output += s[s.length() - 2];
        output += s[s.length() - 1];

        return output;
    }

    inline bool read_hex(const std::string &input, mpz_class &output, size_t size = 64)
    {
        if (input.length() != size)
            return false;
        return (output.set_str(input, 16) == 0);
    }

    inline bool read_hex_le(const std::string &input, mpz_class &output, size_t size = 64)
    {
        auto rev = reverse_endianness(input);
        return read_hex(rev, output, size);
    }

    inline std::vector<uint8_t> num_to_bytes(const mpz_class &x, size_t min_bytes)
    {
        auto num_bits = mpz_sizeinbase(x.get_mpz_t(), 2);
        size_t size = min_bytes;
        if ((num_bits + 7)/8 > min_bytes)
            size = (num_bits + 7)/8;
        auto x_bytes = std::vector<uint8_t>(size);
        mpz_export(x_bytes.data(), NULL, -1, sizeof(uint8_t) , -1, 0, x.get_mpz_t());
        return x_bytes;
    }
    
    inline std::string num_str_le(const mpz_class &x)
    {
        return reverse_endianness(x.get_str(16));
    }
    
    inline std::string num_str(const mpz_class &x)
    {
        return x.get_str(16);
    }

    inline std::string num_str(const mpz_class &x, int min_chars)
    {
        auto conv = x.get_str(16);
        if (conv.length() >= min_chars)
            return conv;
        auto zeros = std::string(min_chars - conv.length(), '0');
        return zeros + conv;
    }

    template <typename ...T>
    void concat_into(std::vector<uint8_t> &output) {}
    template <typename ...T>
    void concat_into(std::vector<uint8_t> &output, const std::vector<uint8_t> &param, const T&... args)
    {
        output.insert(output.end(), param.begin(), param.end());
        concat_into(output, args...);
    }
    template <typename ...T>
    std::vector<uint8_t> concat(const T&... args)
    {
        auto output = std::vector<uint8_t>();
        concat_into(output, args...);
        return output;
    }
}


#endif