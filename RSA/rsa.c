#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

//TODO: fix decryption for bigger primes
#define LOWER 1000
#define UPPER 10000

static uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t mod){
	return (a * b) % mod;
}

static uint64_t pow_mod(uint64_t base, uint64_t exp, uint64_t mod) {
	uint64_t res = 1;
	base %= mod;
	while(exp) {
		// if controls if exp is odd
		if (exp & 1) res = mul_mod(res, base, mod);
		base = mul_mod(base,base,mod);
		exp >>= 1;
	}
	return res;
}

//Miller-Rabin
static int miller_rabin_once(uint32_t a, uint32_t n, uint32_t d, int s) {
	uint64_t x = pow_mod(a,d,n);
	if(x == 1 || x  == n-1) return 1;
	for(int r = 1; r<s; ++r){
		x = mul_mod(x,x,n);
		if (x == n-1) return 1;
	}
	return 0;
}

// Verifies if a given number is prime
int is_prime(uint32_t n){
	if (n <= 3) return n > 1;
	if (n % 2 == 0) return 0;

	uint32_t d = n - 1;
	int s = 0;
	while ((d & 1) == 0) {
		d >>= 1;
		s++;
	}

    uint32_t bases[] = {2,7,61};
    for (size_t i = 0; i < sizeof(bases)/sizeof(bases[0]); ++i) {
        uint32_t a = bases[i];
        if (a >= n) continue;
        if (!miller_rabin_once(a, n, d, s)) return 0;
    }
    return 1;
}

uint32_t rand_range(uint32_t low, uint32_t high){
	if (high <= low + 1) {
		fprintf(stderr, "rand_range: invalid range (%u, %u)", low, high);
		return low;
	}
	uint64_t span = (uint64_t)high - low;

	uint64_t r = ((uint64_t) rand() << 15) | rand();
	return (uint32_t)(low + (r % span));
}


uint32_t generate_prime(uint32_t low, uint32_t high) {
	while(1) { 
		uint32_t p = rand_range(low, high);
	        if (p % 2 == 0) p++;

		if(is_prime(p)) return p; 
	}
}


uint64_t gcd_u64(uint64_t a, uint64_t b){
	while(b) {
		uint64_t t = a % b;
		a = b;
		b = t;
	}
	return a;
}

static int64_t egcd(int64_t a, int64_t b, int64_t *x, int64_t *y){
    if (a == 0) {
        *x = 0;
        *y = 1;
        return b;
    }
    int64_t x1 = 0, y1 = 0;
    int64_t g = egcd(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1;
    *y = x1;
    return g;
}

// returns modular inverse of a modulo m, or 0 if none exists
int64_t modinv(int64_t a, int64_t m) {
    if (m <= 1) return 0;
    a %= m;
    if (a < 0) a += m;
    int64_t x = 0, y = 0;
    int64_t g = egcd(a, m, &x, &y);
    if (g != 1) return 0; // inverse doesn't exist
    int64_t res = x % m;
    if (res < 0) res += m;
    return res;
}

uint64_t encrypt_byte(uint8_t m, uint64_t e, uint64_t n) {
	return pow_mod((uint64_t)m,e,n);
}

uint8_t decrypt_byte(uint64_t c, uint64_t d, uint64_t n){
	uint64_t m = pow_mod(c,d,n);
	return (uint8_t)(m & 0xFF);
}

int main(int argc, char **argv){
	srand((unsigned)time(NULL));


	uint32_t p = 0, q = 0;
	do {
		q = generate_prime(LOWER,UPPER);
		p = generate_prime(LOWER,UPPER);
	} while (p == q);

	uint64_t n = (uint64_t)p * (uint64_t)q;
	uint64_t phi = (uint64_t)(p-1) * (uint64_t)(q - 1);

	//Public key e
	uint64_t e = 65537;
	if (gcd_u64(e,phi) != 1) {
		e = 3;
		while (e < phi && gcd_u64(e, phi) != 1) e += 2;
	}
	// Private key d = inverse of e (mod phi)
	int64_t d_temp = modinv((int64_t)e, (int64_t)phi);
	if (d_temp <= 0) {
		fprintf(stderr, "Error: impossible to calculate the modular inverse.\n");
		return 1;
	}
	uint64_t d = (uint64_t)d_temp;

	printf("p = %u\nq = %u\nn = %llu\nphi = %llu\ne = %llu\nd = %llu\n\n",
			p,q, (unsigned long long)n, (unsigned long long)phi,
			(unsigned long long)e, (unsigned long long)d);

	if (argc < 2) {
		printf("Use: %s \"message to encrypt\"\n", argv[0]);
		return 0;
	}
	
	for(int j = 1; j < argc; j++){
	const char *msg = argv[j];
	size_t L = strlen(msg);

	//encryption: for each byte -> number
	printf("Ciphertext (numbers):\n");
	uint64_t *cipher = malloc(sizeof(uint64_t) * L);
	if (!cipher) { perror("malloc"); return 1; }

	for (size_t i = 0; i < L; i++) {
		uint8_t mbyte = (uint8_t)msg[i];
		cipher[i] = encrypt_byte(mbyte, e, n);
		printf("%llu ", (unsigned long long)cipher[i]);
	}
	printf("\n\n");
	//decryption
	printf("Decrypted message:\n");
	char *recovered = malloc(sizeof(char)*(L+1));
	if (!recovered) { perror("malloc"); free(cipher); return 1; }

	for (size_t i = 0; i < L; i++){
		recovered[i] = (char)decrypt_byte(cipher[i], d, n);
	}
	recovered[L] = '\0';
	printf("%s\n\n", recovered);
	

	free(cipher);
	free(recovered);
	}
	return 0;
}
