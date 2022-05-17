#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "api.h"
#include "SABER_params.h"
#include "SABER_indcpa.h"
#include "poly.h"
#include "poly_mul.h"
#include "cpucycles.h"
#include "speed_print.h"

#define NTESTS 10000

uint64_t t[NTESTS];
uint8_t seed[SABER_SEEDBYTES] = {0};

int main()
{
  printf("\n");
  printf("Saber-%d-%d\n\n", SABER_N * SABER_L, 1 << SABER_EQ);

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
  print_results("Saber_keypair: ", t, NTESTS);

  for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_enc(ct, key, pk);
  }
  print_results("Saber_encaps: ", t, NTESTS);

  for (i = 0; i < NTESTS; i++)
  {
    t[i] = cpucycles();
    crypto_kem_dec(key, ct, sk);
  }
  print_results("Saber_decaps: ", t, NTESTS);

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
  printf("Saber-%d-%d-KEM is correct!\n", SABER_N * SABER_L, 1 << SABER_EQ);

  return 0;
}
