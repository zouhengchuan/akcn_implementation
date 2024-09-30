CC=/usr/bin/gcc
CFLAGS=-mavx2 -maes -mbmi2 -mpopcnt -O3 -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls -fPIC -std=c99
LDFLAGS=-L/usr/local/opt/openssl@1.1/lib -I/usr/local/opt/openssl@1.1/include -lcrypto -lm

SOURCES=cpapke.c fips202.c ntt.c poly.c reduce.c rng.c kem.c verify.c
HEADERS=cpapke.h fips202.h ntt.h poly.h reduce.h rng.h params.h api.h verify.h ds_benchmark.h

all: PQCgenKAT_kem speed768

PQCgenKAT_kem: $(HEADERS) $(SOURCES) PQCgenKAT_kem.c
	$(CC) $(CFLAGS) -o $@ $(SOURCES) PQCgenKAT_kem.c $(LDFLAGS)

speed768: $(HEADERS) $(SOURCES) cpucycles.h cpucycles.c speed_print.h speed_print.c test_speed.c
	$(CC) $(CFLAGS) -o $@ $(SOURCES) cpucycles.c speed_print.c test_speed.c $(LDFLAGS)

.PHONY: clean

clean:
	rm -rf PQCgenKAT_kem
	rm -rf PQCkemKAT_2400.req
	rm -rf speed768
