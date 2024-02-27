#include <gmpxx.h>
#include <iostream>
#include <ctime>
#include <fstream>


#include "utils.h"
#include "x25519.h"
#include "shake128.h"

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Incorrect input format." << '\n';
        return -1;
    }

    auto out = std::ofstream(argv[1], std::ios_base::openmode::_S_bin);
    if (!out.is_open())
    {
        std::cerr << "Error opening file." << '\n';
        return -1;
    }

    gmp_randclass rand{gmp_randinit_default};
    rand.seed(time(NULL));

    mpz_class sk = rand.get_z_bits(256);
    auto q = mpz_class(9);

    x25519::apply_mask(sk);
    auto pk = x25519::montgomery(sk, q);    
    std::cout << utils::num_str(pk, 64) << "\n";

    

    auto s = static_cast<mpz_class>(rand.get_z_bits(255));
    auto hash = shake128::hash(pk, 32);
    out << utils::num_str(sk, 64) << "\n";
    out << utils::num_str(s, 64) << "\n";
    out << utils::num_str(pk, 64) << "\n";
    out << shake128::hash_to_string(hash, 64) << '\n';

}