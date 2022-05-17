#include <stddef.h>
#include <stdint.h>
#include "kem.h"
#include "params.h"
#include "rng.h"
#include "symmetric.h"
#include "verify.h"
#include "indcpa.h"

/*************************************************
 * Name:        crypto_kem_keypair
 *
 * Description: Generates public and private key
 *              for CCA-secure Kyber key encapsulation mechanism
 *
 * Arguments:   - unsigned char *pk: pointer to output public key
 *                (an already allocated array of CRYPTO_PUBLICKEYBYTES bytes)
 *              - unsigned char *sk: pointer to output private key
 *                (an already allocated array of CRYPTO_SECRETKEYBYTES bytes)
 *
 * Returns 0 (success)
 **************************************************/
int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
  size_t i;
  indcpa_keypair(pk, sk);
  for (i = 0; i < KYBER_INDCPA_PUBLICKEYBYTES; i++)
    sk[i + KYBER_INDCPA_SECRETKEYBYTES] = pk[i];

  randombytes(sk + KYBER_SECRETKEYBYTES - KYBER_SYMBYTES, KYBER_SYMBYTES);
  return 0;
}

/*************************************************
 * Name:        crypto_kem_enc
 *
 * Description: Generates cipher text and shared
 *              secret for given public key
 *
 * Arguments:   - unsigned char *ct: pointer to output cipher text
 *                (an already allocated array of CRYPTO_CIPHERTEXTBYTES bytes)
 *              - unsigned char *ss: pointer to output shared secret
 *                (an already allocated array of CRYPTO_BYTES bytes)
 *              - const unsigned char *pk: pointer to input public key
 *                (an already allocated array of CRYPTO_PUBLICKEYBYTES bytes)
 *
 * Returns 0 (success)
 **************************************************/
int crypto_kem_enc(unsigned char *ct,
                   unsigned char *ss,
                   const unsigned char *pk)
{
  uint8_t buf[2 * KYBER_SYMBYTES];
  uint8_t kr[2 * KYBER_SYMBYTES];
  uint8_t m[KYBER_SYMBYTES + 33];
  int i;

  randombytes(buf, KYBER_SYMBYTES);
  hash_h(buf, buf, KYBER_SYMBYTES);

  for (i = 0; i < 33; ++i)
    m[i] = pk[i];
  for (i = 0; i < KYBER_SYMBYTES; ++i)
    m[i + 33] = buf[i];

  hash_g(kr, m, 33 + KYBER_SYMBYTES);

  indcpa_enc(ct, buf, pk, kr + KYBER_SYMBYTES);

  for (i = 0; i < KYBER_SYMBYTES; ++i)
    ss[i] = kr[i];

  return 0;
}

/*************************************************
 * Name:        crypto_kem_dec
 *
 * Description: Generates shared secret for given
 *              cipher text and private key
 *
 * Arguments:   - unsigned char *ss: pointer to output shared secret
 *                (an already allocated array of CRYPTO_BYTES bytes)
 *              - const unsigned char *ct: pointer to input cipher text
 *                (an already allocated array of CRYPTO_CIPHERTEXTBYTES bytes)
 *              - const unsigned char *sk: pointer to input private key
 *                (an already allocated array of CRYPTO_SECRETKEYBYTES bytes)
 *
 * Returns 0.
 *
 * On failure, ss will contain a pseudo-random value.
 **************************************************/
int crypto_kem_dec(unsigned char *ss,
                   const unsigned char *ct,
                   const unsigned char *sk)
{
  size_t i;
  uint8_t buf[2 * KYBER_SYMBYTES];
  uint8_t m[KYBER_SYMBYTES + 33];
  uint8_t kr[2 * KYBER_SYMBYTES], kr2[2 * KYBER_SYMBYTES];
  uint8_t cmp[KYBER_CIPHERTEXTBYTES];
  const uint8_t *pk = sk + KYBER_INDCPA_SECRETKEYBYTES;
  uint8_t ct2[KYBER_CIPHERTEXTBYTES + KYBER_SYMBYTES + 33];
  int16_t t;
  int32_t fail;

  indcpa_dec(buf, ct, sk);

  for (i = 0; i < 33; ++i)
    m[i] = pk[i];
  for (i = 0; i < KYBER_SYMBYTES; ++i)
    m[i + 33] = buf[i];

  hash_g(kr, m, 33 + KYBER_SYMBYTES);

  indcpa_enc(ct2, buf, pk, kr + KYBER_SYMBYTES);

  t = 0;
  for (i = 0; i < KYBER_CIPHERTEXTBYTES; ++i)
    t |= ct[i] ^ ct2[i];

  fail = (uint16_t)t;
  fail = (-fail) >> 31;

  for (i = 0; i < 33; ++i)
    ct2[i] = pk[i];
  for (i = 0; i < KYBER_SYMBYTES; ++i)
    ct2[i + 33] = sk[i + KYBER_SECRETKEYBYTES - KYBER_SYMBYTES];
  for (i = 0; i < KYBER_CIPHERTEXTBYTES; ++i)
    ct2[i + 33 + KYBER_SYMBYTES] = ct[i];

  hash_g(kr2, ct2, KYBER_CIPHERTEXTBYTES + KYBER_SYMBYTES + 33);

  if (fail)
  {
    for (i = 0; i < KYBER_SYMBYTES; ++i)
      ss[i] = kr2[i];
  }
  else
  {
    for (i = 0; i < KYBER_SYMBYTES; ++i)
      ss[i] = kr[i] & ~(-fail);
  }

  return 0;
}
