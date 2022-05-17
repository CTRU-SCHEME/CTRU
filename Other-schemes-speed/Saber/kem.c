#include "SABER_params.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "SABER_indcpa.h"
#include "api.h"
#include "verify.h"
#include "rng.h"
#include "fips202.h"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
  size_t i;
  indcpa_kem_keypair(pk, sk);
  for (i = 0; i < SABER_INDCPA_PUBLICKEYBYTES; i++)
    sk[i + SABER_INDCPA_SECRETKEYBYTES] = pk[i];

  randombytes(sk + SABER_SECRETKEYBYTES - SABER_KEYBYTES, SABER_KEYBYTES);
  return 0;
}

int crypto_kem_enc(unsigned char *ct,
                   unsigned char *ss,
                   const unsigned char *pk)
{
  uint8_t buf[2 * SABER_KEYBYTES];
  uint8_t kr[2 * SABER_KEYBYTES];
  uint8_t m[SABER_KEYBYTES + 33];
  int i;

  randombytes(buf, SABER_KEYBYTES);
  sha3_256(buf, buf, SABER_KEYBYTES);

  for (i = 0; i < 33; ++i)
    m[i] = pk[i];
  for (i = 0; i < SABER_KEYBYTES; ++i)
    m[i + 33] = buf[i];

  sha3_512(kr, m, 33 + SABER_KEYBYTES);

  indcpa_kem_enc(buf, kr + SABER_KEYBYTES, pk, ct);

  for (i = 0; i < SABER_KEYBYTES; ++i)
    ss[i] = kr[i];

  return 0;
}

int crypto_kem_dec(unsigned char *ss,
                   const unsigned char *ct,
                   const unsigned char *sk)
{
  size_t i;
  uint8_t buf[2 * SABER_KEYBYTES];
  uint8_t m[SABER_KEYBYTES + 33];
  uint8_t kr[2 * SABER_KEYBYTES], kr2[2 * SABER_KEYBYTES];
  // uint8_t cmp[CRYPTO_CIPHERTEXTBYTES];
  const uint8_t *pk = sk + SABER_INDCPA_SECRETKEYBYTES;
  uint8_t ct2[CRYPTO_CIPHERTEXTBYTES + SABER_KEYBYTES + 33];
  int16_t t;
  int32_t fail;

  indcpa_kem_dec(sk, ct, buf);

  for (i = 0; i < 33; ++i)
    m[i] = pk[i];
  for (i = 0; i < SABER_KEYBYTES; ++i)
    m[i + 33] = buf[i];

  sha3_512(kr, m, 33 + SABER_KEYBYTES);

  indcpa_kem_enc(buf, kr + SABER_KEYBYTES, pk, ct2);

  t = 0;
  for (i = 0; i < CRYPTO_CIPHERTEXTBYTES; ++i)
    t |= ct[i] ^ ct2[i];

  fail = (uint16_t)t;
  fail = (-fail) >> 31;

  for (i = 0; i < 33; ++i)
    ct2[i] = pk[i];
  for (i = 0; i < SABER_KEYBYTES; ++i)
    ct2[i + 33] = sk[i + SABER_SECRETKEYBYTES - SABER_KEYBYTES];
  for (i = 0; i < CRYPTO_CIPHERTEXTBYTES; ++i)
    ct2[i + 33 + SABER_KEYBYTES] = ct[i];

  sha3_512(kr2, ct2, CRYPTO_CIPHERTEXTBYTES + SABER_KEYBYTES + 33);

  if (fail)
  {
    for (i = 0; i < SABER_KEYBYTES; ++i)
      ss[i] = kr2[i];
  }
  else
  {
    for (i = 0; i < SABER_KEYBYTES; ++i)
      ss[i] = kr[i] & ~(-fail);
  }

  return 0;
}
