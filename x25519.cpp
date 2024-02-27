#include <gmpxx.h>

#include "x25519.h"


static const mpz_class prime = (mpz_class(1) << 255) - 19;
static const mpz_class Adiv4 = 121665;


static const mpz_class m_clamp_mask("0x0ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff8");
static const mpz_class m_or_mask(   "0x4000000000000000000000000000000000000000000000000000000000000000");
static const mpz_class u_clamp_mask("0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");



//if b is non zero, swap e1 and e2
void conditional_swap(mpz_class b, mpz_class &e1, mpz_class &e2)
{
    mpz_class swap_1 = e1 * (1 - b) + e2 * b;
    mpz_class swap_2 = e1 * (b) + e2 * (1 - b);
    e1 = swap_1;
    e2 = swap_2;
}


void x25519::apply_mask(mpz_class &m)
{
    m = m & m_clamp_mask;
    m = m | m_or_mask;
}

mpz_class x25519::montgomery(mpz_class m, mpz_class u)
{     
    m = m & m_clamp_mask;
    m = m | m_or_mask;
    u = u & u_clamp_mask;

    mpz_class x_1 = u;
    mpz_class x_2 = 1;
    mpz_class z_2 = 0;
    mpz_class x_3 = u;
    mpz_class z_3 = 1;
    mpz_class k_t = 0;
    mpz_class swap = 0;
    for (int i = 254;i >= 0;i--)
    {
        k_t =  (m >> i) & 1;
        swap ^= k_t;
        conditional_swap(swap, x_2, x_3);
        conditional_swap(swap, z_2, z_3);
        swap = k_t;

        mpz_class A = (x_2 + z_2) % prime;
        mpz_class AA = (A * A) % prime;
        mpz_class B = (x_2 - z_2) % prime;
        mpz_class BB = (B * B) % prime;
        mpz_class E = (AA - BB) % prime;
        mpz_class C = (x_3 + z_3) % prime;
        mpz_class D = (x_3 - z_3) % prime;
        mpz_class DA = (D * A) % prime;
        mpz_class CB = (C * B) % prime;
        x_3 = ((DA + CB) * (DA + CB)) % prime;
        z_3 = (x_1 * (DA - CB)*(DA - CB)) % prime;
        x_2 = (AA * BB) % prime;
        z_2 = (E * (AA + Adiv4 * E)) % prime;
    }
    conditional_swap(swap, x_2, x_3);
    conditional_swap(swap, z_2, z_3);

    mpz_class prime_minus_two = prime - 2;
    mpz_powm(z_2.get_mpz_t(), z_2.get_mpz_t(), prime_minus_two.get_mpz_t(), prime.get_mpz_t());
    mpz_class to_return = (x_2 * z_2) % prime;
    
    return to_return;
}