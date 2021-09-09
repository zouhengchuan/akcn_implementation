CC = /usr/bin/gcc
LDFLAGS = -lcrypto
BENCHFLAGS = -O3 -fomit-frame-pointer -march=native -lm

SOURCES = kem.c cpapke.c fips202.c ntt.c poly.c precomp.c reduce.c rng.c verify.c 
HEADERS = api.h cpapke.h fips202.h ntt.h params.h poly.h reduce.h rng.h verify.h ds_benchmark.h

all: PQCgenKAT_kem benchmark

PQCgenKAT_kem: $(SOURCES) $(HEADERS) PQCgenKAT_kem.c
	$(CC) $(SOURCES) PQCgenKAT_kem.c -o $@ $(LDFLAGS)

benchmark: $(SOURCES) $(HEADERS) benchmark.c
	$(CC) $(SOURCES) benchmark.c -o $@ $(LDFLAGS) $(BENCHFLAGS)


.PHONY: clean

clean:
	-rm PQCgenKAT_kem
	-rm benchmark
