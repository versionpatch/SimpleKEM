#include <gmpxx.h>
#include <gmp.h>
#include <iostream>
#include <vector>
#include <array>
#include <sstream>

#include "shake128.h"


static std::array<uint64_t, 24> RC = 
   {0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000, 0x000000000000808B,
    0x0000000080000001, 0x8000000080008081, 0x8000000000008009, 0x000000000000008A, 0x0000000000000088,
    0x0000000080008009, 0x000000008000000A, 0x000000008000808B, 0x800000000000008B, 0x8000000000008089,
    0x8000000000008003, 0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
    0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008  };

typedef std::array<uint64_t, 25> state_array;
#define state_lane(state, x, y) state[5*y + x]


//Returns y such that y[i] = x[(i - k) % 64], k >= -1024. x[0] is the least significant bit.
uint64_t cyclic_shift(uint64_t x, int k)
{
    k = (k + 1024) % 64;
    return (x << k) + (x >> ((64 - k) % 64));
}

state_array theta(const state_array &src)
{
    std::array<uint64_t, 5> C,D;
    state_array A_prime;

    for (int x = 0;x < 5;x++)
        C[x] = 0;
    
    for (int y = 0;y < 5;y++) { for (int x = 0;x < 5;x++) {
            C[x] ^= state_lane(src, x, y);
    }}

    for (int x = 0;x < 5;x++) {
        D[x] = C[(x + 4) % 5] ^ cyclic_shift(C[(x + 1) % 5], 1);
    }
    
    for (int y = 0;y < 5;y++) {for (int x = 0;x < 5;x++) {
        state_lane(A_prime, x, y) = state_lane(src, x, y) ^ D[x];
    }}

    return A_prime;
}

state_array rho(const state_array &src)
{
    state_array A_prime;
    int x = 1,y = 0;
    state_lane(A_prime, 0, 0) = state_lane(src, 0, 0);
    for (int t = 0;t < 24;t++)
    {
        state_lane(A_prime, x, y) = cyclic_shift(state_lane(src, x, y), (t+1)*(t+2)/2);
        int temp = x;
        x = y;
        y = (2*temp + 3*y) % 5;
    }
    return A_prime;
}

state_array pi(const state_array &src)
{
    state_array A_prime;
    for (int y = 0;y < 5;y++) {for (int x = 0;x < 5;x++) {
        state_lane(A_prime, x, y) = state_lane(src, (x + 3*y) % 5, x);
    }}
    return A_prime;
}

state_array chi(const state_array &src)
{
    state_array A_prime;
    for (int y = 0;y < 5;y++) {for (int x = 0;x < 5;x++) {
        state_lane(A_prime, x, y) = state_lane(src, x, y) 
                                       ^((state_lane(src, (x+1) % 5, y) ^ ~0)
                                        & (state_lane(src, (x+2) % 5, y)));
    }}
    return A_prime;
}

state_array iota(const state_array &src, int ir)
{
    state_array A_prime;
    for (int y = 0;y < 5;y++) { for (int x = 0;x < 5;x++) {
        state_lane(A_prime, x, y) = state_lane(src, x, y);
    }}
    state_lane(A_prime, 0, 0) ^= RC[ir];
    return A_prime;
}

void shake_round(state_array &A)
{
    for (int i = 0;i < 24;i++)
    {
        A = theta(A);
        A = rho(A);
        A = pi(A);
        A = chi(A);
        A = iota(A, i);
    }
}

std::string shake128::hash_to_string(const std::vector<uint64_t> &hash, int size)
{
    auto out = std::string("");
    size = size / 2;
    for (int i = 0;i < hash.size() && i < size / 8;i++)
    {
        for (int j = 0;j < 8;j++)
        {
            char byte_str[3];
            sprintf(byte_str, "%02x", static_cast<uint8_t>(hash[i] >> j*8));
            out += byte_str;
        }
    }
    return out;
}



std::vector<uint8_t> shake128::hash_to_bytes(const std::vector<uint64_t> &hash)
{
    auto out = std::vector<uint8_t>(hash.size() * 8);
    for (int i = 0;i < hash.size();i++)
    {
        for (int j = 0;j < 8;j++)
        {
            out[i*8 + j] = static_cast<uint8_t>(hash[i] >> j*8);
        }
    }
    return out;
}
std::vector<uint64_t> shake128::hash(std::vector<uint8_t> &data, int size)
{
    int c = 256;
    int b = 1600;
    int r = b - c;
    int block_size = r / 8;
    
    data.push_back(0x1F);
    while (data.size() % (block_size) != 0)
    {
        data.push_back(0);
    }
    data[data.size() - 1] |= 0x80;
    
    state_array S;
    for (int i = 0;i < 25;i++)
        S[i] = 0;
    
    //Absorption
    for (int i = 0;i < data.size() / (block_size);i++)
    {
        for (int j = 0;j < block_size / 8;j++)
        {
            uint64_t P = 0;
            for (int k = 0;k < 8;k++)
            {
                uint64_t d0 = (uint64_t)((data[i * block_size + j*8 + (k)])) << (k*8);
                P |= d0;
            }
            S[j] ^= P;
        }
        shake_round(S);
    }

    //Squeezing1
    std::vector<uint64_t> output;
    while (output.size() * 8 < size)
    {
        for (int i = 0;i < r/64;i++)
        {
            output.push_back(S[i]);
            if (output.size() * 8 >= size)
                return output;
        }
        shake_round(S);
    }
    
    return output;
}

std::vector<uint64_t> shake128::hash(const mpz_class &x, int size)
{
    auto num_bits = mpz_sizeinbase(x.get_mpz_t(), 2);
    auto x_bytes = std::vector<uint8_t>(num_bits/8 + 1);
    mpz_export(x_bytes.data(), NULL, -1, sizeof(uint8_t) , -1, 0, x.get_mpz_t());
    return hash(x_bytes, size);
}

std::string shake128::hash_into_string(std::vector<uint8_t> &data, int size)
{
    return hash_to_string(hash(data, size), size * 2);
}