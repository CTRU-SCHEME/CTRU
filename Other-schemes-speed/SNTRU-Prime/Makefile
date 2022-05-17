CC=/usr/bin/gcc
CFLAGS += -O3 -march=native -fomit-frame-pointer
LDFLAGS=-lcrypto

SOURCES= dec.c enc.c int32_sort.c keypair.c r3_mult.c r3_recip.c random32.c randomsmall.c randomweightw.c rq_mult.c rq_recip3.c rq_round3.c rq_rounded.c rq.c small.c swap.c test_speed.c cpucycles.c speed_print.c nist/rng.c sha512-hash.c verify.c sha512-blocks.c
HEADERS= crypto_kem_sntrup4591761.h crypto_kem.h int32_sort.h mod3.h modq.h params.h r3.h randombytes.h rq.h small.h swap.h cpucycles.h speed_print.h crypto_int32.h crypto_int8.h crypto_int16.h crypto_uint16.h crypto_uint32.h crypto_hash_sha512.h crypto_verify_32.h randombytes.h nist/rng.h sha512.h

test_speed: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

.PHONY: clean

clean:
	-rm test_speed

