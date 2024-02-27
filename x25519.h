#ifndef _X25519_H
#define _X25519_H

#include <gmpxx.h>

namespace x25519
{
    mpz_class montgomery(mpz_class m, mpz_class u);
    void apply_mask(mpz_class &m);
}


#endif