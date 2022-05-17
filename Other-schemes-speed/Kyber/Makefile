CC=/usr/bin/gcc
CFLAGS += -O3 -march=native -fomit-frame-pointer
LDFLAGS=-lcrypto

SOURCES= cbd.c fips202.c indcpa.c kem.c ntt.c poly.c polyvec.c test_speed.c reduce.c rng.c verify.c symmetric-shake.c cpucycles.c speed_print.c
HEADERS= api.h cbd.h fips202.h indcpa.h ntt.h params.h poly.h polyvec.h reduce.h rng.h verify.h symmetric.h cpucycles.h speed_print.h

all:  test_speed768 

test_speed768: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) -DKYBER_K=3 -o $@ $(SOURCES) $(LDFLAGS)

.PHONY: clean

clean:
	-rm test_speed768

