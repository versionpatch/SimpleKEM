#include <iostream>
#include <vector>
#include <array>
#include <gmpxx.h>

#include "x25519.h"
#include "utils.h"
#include "shake128.h"

#include "hash_elgamal.h"

static const mpz_class q = mpz_class(9);

std::pair<mpz_class, mpz_class> elgamal::encrypt(const mpz_class &m, const mpz_class &pk, const mpz_class &r)
{
    auto u = x25519::montgomery(r,q);
    auto v = x25519::montgomery(r,pk);
    auto uv = utils::concat(utils::num_to_bytes(u, 32), utils::num_to_bytes(v, 32));
    auto k = mpz_class(shake128::hash_into_string(uv, 32), 16);
    auto c = static_cast<mpz_class>(m ^ k);
    return std::make_pair(u, c);
}

mpz_class elgamal::decrypt(const mpz_class &u, const mpz_class &c, const mpz_class &sk)
{
    auto v = x25519::montgomery(sk, u);
    auto uv = utils::concat(utils::num_to_bytes(u, 32), utils::num_to_bytes(v, 32));
    auto k = mpz_class(shake128::hash_into_string(uv, 32), 16);
    auto m = static_cast<mpz_class>(c ^ k);
    return m;
}


