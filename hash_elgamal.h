#ifndef _HASH_ELGAMAL_H
#define _HASH_ELGAMAL_H

#include <iostream>
#include <vector>
#include <array>
#include <gmpxx.h>

namespace elgamal
{
    std::pair<mpz_class, mpz_class> encrypt(const mpz_class &m, const mpz_class &pk, const mpz_class &r);
    mpz_class decrypt(const mpz_class &u, const mpz_class &c, const mpz_class &sk);
}

#endif