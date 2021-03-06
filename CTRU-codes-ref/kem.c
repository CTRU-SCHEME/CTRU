#include <stddef.h>
#include "randombytes.h"
#include "crypto_stream.h"
#include "params.h"
#include "ctru.h"
#include "poly.h"
#include "sha2.h"
#include "pack.h"

static const unsigned char nonce[16] = {0};
#define hash_h(OUT, IN, INBYTES) sha512(OUT, IN, INBYTES);

int crypto_kem_keygen(unsigned char *pk,
                      unsigned char *sk)
{
  unsigned int i;
  unsigned char coins[CTRU_N];

  do
  {
    randombytes(coins, CTRU_SEEDBYTES);
    crypto_stream(coins, CTRU_N, nonce, coins);
  } while (ctru_keygen(pk, sk, coins));

  for (i = 0; i < CTRU_PKE_PUBLICKEYBYTES; ++i)
    sk[i + CTRU_PKE_SECRETKEYBYTES] = pk[i];

  randombytes(sk + CTRU_PKE_SECRETKEYBYTES + CTRU_PKE_PUBLICKEYBYTES, CTRU_SEEDBYTES);

  return 0;
}

int crypto_kem_encaps(unsigned char *ct,
                      unsigned char *k,
                      const unsigned char *pk)
{
  unsigned int i;
  unsigned char buf[CTRU_SHAREDKEYBYTES + CTRU_N], m[CTRU_PREFIXHASHBYTES + CTRU_MSGBYTES];

  randombytes(buf, CTRU_SEEDBYTES);
  crypto_stream(m + CTRU_PREFIXHASHBYTES, CTRU_MSGBYTES, nonce, buf);

  for (i = 0; i < CTRU_PREFIXHASHBYTES; ++i)
    m[i] = pk[i];

  hash_h(buf, m, CTRU_PREFIXHASHBYTES + CTRU_MSGBYTES);
  crypto_stream(buf + CTRU_SHAREDKEYBYTES, CTRU_N, nonce, buf + CTRU_SHAREDKEYBYTES);

  ctru_enc(ct, pk, m + CTRU_PREFIXHASHBYTES, buf + CTRU_SHAREDKEYBYTES);

  for (i = 0; i < CTRU_SHAREDKEYBYTES; ++i)
    k[i] = buf[i];

  return 0;
}

int crypto_kem_decaps(unsigned char *k,
                      const unsigned char *ct,
                      const unsigned char *sk)
{
  unsigned int i;
  unsigned char buf[CTRU_SHAREDKEYBYTES + CTRU_N], buf2[CTRU_SHAREDKEYBYTES * 2], m[CTRU_PREFIXHASHBYTES + CTRU_MSGBYTES];
  unsigned char ct2[CTRU_PKE_CIPHERTEXTBYTES + CTRU_SEEDBYTES + CTRU_PREFIXHASHBYTES];
  int16_t t;
  int32_t fail;

  ctru_dec(m + CTRU_PREFIXHASHBYTES, ct, sk);

  for (i = 0; i < CTRU_PREFIXHASHBYTES; ++i)
    m[i] = sk[i + CTRU_PKE_SECRETKEYBYTES];

  hash_h(buf, m, CTRU_PREFIXHASHBYTES + CTRU_MSGBYTES);
  crypto_stream(buf + CTRU_SHAREDKEYBYTES, CTRU_N, nonce, buf + CTRU_SHAREDKEYBYTES);

  ctru_enc(ct2, sk + CTRU_PKE_SECRETKEYBYTES, m + CTRU_PREFIXHASHBYTES, buf + CTRU_SHAREDKEYBYTES);

  t = 0;
  for (i = 0; i < CTRU_PKE_CIPHERTEXTBYTES; ++i)
    t |= ct[i] ^ ct2[i];

  fail = (uint16_t)t;
  fail = (-fail) >> 31;

  for (i = 0; i < CTRU_PREFIXHASHBYTES; ++i)
    ct2[i] = sk[i + CTRU_PKE_SECRETKEYBYTES];
  for (i = 0; i < CTRU_SEEDBYTES; ++i)
    ct2[i + CTRU_PREFIXHASHBYTES] = sk[i + CTRU_PKE_SECRETKEYBYTES + CTRU_PKE_PUBLICKEYBYTES];
  for (i = 0; i < CTRU_PKE_CIPHERTEXTBYTES; ++i)
    ct2[i + CTRU_PREFIXHASHBYTES + CTRU_SEEDBYTES] = ct[i];
  hash_h(buf2, ct2, CTRU_PKE_CIPHERTEXTBYTES + CTRU_SEEDBYTES + CTRU_PREFIXHASHBYTES);

  for (i = 0; i < CTRU_SHAREDKEYBYTES; ++i)
    k[i] = buf[i] ^ ((-fail) & (buf[i] ^ buf2[i]));

  return fail;
}
