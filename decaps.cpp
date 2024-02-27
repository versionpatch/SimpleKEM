#include <gmpxx.h>
#include <iostream>
#include <ctime>
#include <fstream>

#include "utils.h"
#include "x25519.h"
#include "shake128.h"
#include "hash_elgamal.h"

int main(int argc, const char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Incorrect input format." << '\n';
        return -1;
    }

    auto u = mpz_class();
    auto c = mpz_class();

    auto u_str = std::string(argv[1]);
    auto c_str = std::string(argv[2]);
    if (u_str.length() != 64 || c_str.length() != 64)
    {
        std::cerr << "Ciphertext must be two 64 hexadecimal digits numbers" << std::endl;
        return -1;
    }
    if (!utils::read_hex(u_str, u) || !utils::read_hex(c_str, c))
    {
        std::cerr << "Ciphertext must be two 64 hexadecimal digits numbers" << std::endl;
        return -1;
    }

    auto pk_file = std::ifstream(argv[3]);
    if (!pk_file.is_open())
    {
        std::cerr << "Error opening key file" << std::endl;
        return -1;
    }

    
    auto sk = mpz_class();
    auto s = mpz_class();
    auto pk = mpz_class();
    auto hash = std::vector<uint8_t>();
    auto fields = std::vector<mpz_class*>{&sk, &s, &pk};

    for (int i = 0;i < 3;i++)
    {
        auto txt = std::string("");
        pk_file >> txt;
        if (txt.length() != 64)
        {
            std::cerr << "Incorrect key file." << std::endl;
            return -1;
        }
        if (!utils::read_hex(txt, *fields[i]))
        {
            std::cerr << "Incorrect key file." << std::endl;
            return -1;
        }
    }
    //read hash as array of bytes
    auto txt = std::string("");
    pk_file >> txt;
    if (txt.length() != 64)
    {
        std::cerr << "Incorrect key file." << std::endl;
        return -1;
    }
    for (int i = 0;i < 32;i++)
    {
        std::string byte = txt.substr(i*2,2);
        try
        {
            hash.push_back(std::stoi(byte, nullptr, 16));
        }
        catch(const std::exception& e)
        {
            std::cerr << "Incorrect key file. Error while parsing the hash." << std::endl;
            return -1;
        }
    }

    //decrypting el-gamal
    auto m = elgamal::decrypt(u, c, sk);

    //acquire r and k
    auto pkm = utils::concat(hash, utils::num_to_bytes(m, 32));
    auto rk = shake128::hash_into_string(pkm, 64);

    auto r = mpz_class(rk.substr(0, 64), 16);
    auto k = mpz_class(rk.substr(64, 64), 16);

    //get K 
    auto uck = utils::concat(utils::num_to_bytes(u, 32), utils::num_to_bytes(c, 32), utils::num_to_bytes(k, 32));
    auto ucs = utils::concat(utils::num_to_bytes(u, 32), utils::num_to_bytes(c, 32), utils::num_to_bytes(s, 32));
    auto K0_str = shake128::hash_into_string(uck, 16);
    auto K1_str = shake128::hash_into_string(ucs, 16);

    //reencrypt to test
    auto [u_new, c_new] = elgamal::encrypt(m, pk, r); 

    //constant time swap
    auto k_str_num = mpz_class(K0_str, 16);
    auto k1_str_num = mpz_class(K1_str, 16);
    auto b = static_cast<size_t>(c_new == c && u_new == u);
    auto result = b*k_str_num + (1-b)*k1_str_num;
    std::cout << "K = " << utils::num_str(result, 32) << std::endl;
    

}