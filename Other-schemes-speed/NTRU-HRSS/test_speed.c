#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "api.h"
#include "params.h"
#include "cpucycles.h"
#include "speed.h"
#include "kem.h"

#define NTESTS 10000

uint64_t t[NTESTS];

int main()
{
  printf("\n");
  printf("NTRU HRSS-%d-%d\n\n", NTRU_N, NTRU_Q);

  unsigned int i, j;
  unsigned char pk[CRYPTO_PUBLICKEYBYTES] = {0};
  unsigned char sk[CRYPTO_SECRETKEYBYTES] = {0};
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES] = {0};
  unsigned char key[CRYPTO_BYTES] = {0};
  unsigned char key1[CRYPTO_BYTES] = {0}, key2[CRYPTO_BYTES] = {0};

  for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_keypair(pk, sk);
  }
  print_results("NTRU HRSS_keypair: ", t, NTESTS);

  for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_enc(ct, key, pk);
  }
  print_results("NTRU HRSS_encaps: ", t, NTESTS);

  for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_dec(key, ct, sk);
  }
  print_results("NTRU HRSS_decaps: ", t, NTESTS);

  for (i = 0; i < 100; i++)
  {
    crypto_kem_keypair(pk, sk);
    crypto_kem_enc(ct, key1, pk);
    crypto_kem_dec(key2, ct, sk);

    for (j = 0; j < CRYPTO_BYTES; j++)
      if (key1[j] != key2[j])
      {
        printf("Round %d. Failure: Keys dont match: %hhx != %hhx!\n", i, key1[j], key2[j]);
        return 0;
      }
  }
  printf("NTRU HRSS-%d-%d-KEM is correct!\n", NTRU_N, NTRU_Q);

  return 0;
}
