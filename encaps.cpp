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
    if (argc != 2)
    {
        std::cerr << "Incorrect input format." << '\n';
        return -1;
    }

    auto pk = mpz_class();
    auto input_str = std::string(argv[1]);

    if (input_str.length() != 64)
    {
        std::cerr << "Incorrect key format. Must be 64 hexadecimal digits." << '\n';
        return -1;
    }
    if (!utils::read_hex(argv[1], pk))
    {
        std::cerr << "Incorrect key format. Must be 64 hexadecimal digits." << '\n';
        return -1;
    }

    gmp_randclass rand{gmp_randinit_default};
    rand.seed(time(NULL));

    auto m = static_cast<mpz_class>(rand.get_z_bits(256));
    auto pk_hash = shake128::hash_to_bytes(shake128::hash(pk, 32));
    auto pkm = utils::concat(pk_hash, utils::num_to_bytes(m, 32));
    auto rk = shake128::hash_into_string(pkm, 64);

    auto r = mpz_class(rk.substr(0, 64), 16);
    auto k = mpz_class(rk.substr(64, 64), 16);

    auto [u,c] = elgamal::encrypt(m, pk, r);
    auto uck = utils::concat(utils::num_to_bytes(u, 32), utils::num_to_bytes(c, 32), utils::num_to_bytes(k, 32));
    auto K_str = shake128::hash_into_string(uck, 16);
    
    std::cout << "u = " << utils::num_str(u, 64) << '\n';
    std::cout << "c = " << utils::num_str(c, 64) << '\n';
    std::cout << "K = " << K_str << '\n';
}