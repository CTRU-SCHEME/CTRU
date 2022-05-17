# CTRU


## The portable C implementation
See CTRU-codes-ref. 

The target parameter set is ($n=768,q=3457,q_2=2^{10},\Psi=B_2$).

Run the portable C codes:

```
cd CTRU-codes-ref
make
./test_kem768
./test_speed768
```

## The Core-SVP security and error probability
See CTRU-security&error. 

They are gotten by the same methodology and scripts provided by Kyber and NTRU in NIST PQC Round 3.

The test codes are given in CTRU-security&error/CTRU.py

Run the codes in CTRU-security&error:

```
python3 CTRU.py
```


## The refined gate-count estimate
See CTRU-gate-count. 

The refined gate-count estimate is  developed from the scripts provided by Kyber and NTRU Prime in NIST PQC Round 3 (see https://github.com/lducas/leaky-LWE-Estimator/tree/NIST-round3  for details) and the paper ''Dana Dachman-Soled, Léo Ducas, Huijing Gong, Mélissa Rossi: LWE with Side Information: Attacks and Concrete Security Estimation. CRYPTO (2) 2020: 329-358'' (see https://eprint.iacr.org/2020/292.pdf).


## Test other schemes
See Other-schemes-speed.

The codes of Kyber, Saber, NTRU-HRSS and SNTRU Prime are taken from https://csrc.nist.gov/projects/post-quantum-cryptography/round-3-submissions.

The codes of NTTRU are taken from https://eprint.iacr.org/2019/040 and https://github.com/gregorseiler/NTTRU.

Run the codes :
```
cd Other-schemes-speed
./run.sh
```


