
# RSA Encryption in C

This project is a simple implementation of the RSA cryptosystem in C, capable of encrypting and decrypting messages **byte by byte**. It includes prime number generation, modular arithmetic, and key generation using standard RSA formulas.

---

## Features

* **Prime generation:** Generates random prime numbers within a specified range using a deterministic version of the **Miller-Rabin primality test**.
* **Key generation:** Computes public and private RSA keys (`e`, `d`) from generated primes `p` and `q`.
* **Modular arithmetic functions:** Includes safe modular multiplication (`mul_mod`) and modular exponentiation (`pow_mod`) for handling large numbers.
* **Byte-level encryption and decryption:** Encrypts messages one byte at a time, suitable for small messages or learning purposes.
* **Custom random range function:** Ensures uniform random selection within given bounds.

---

## Dependencies

* Standard C libraries: `stdio.h`, `stdlib.h`, `stdint.h`, `time.h`, `string.h`.
* Works on any standard C compiler.

---

## How It Works

1. **Prime Number Generation:**

   * Random numbers are generated within a range (`LOWER` and `UPPER`) using `rand_range`.
   * Primality is checked using `is_prime()`, which internally uses the **Miller-Rabin test** with deterministic bases `{2, 7, 61}` for 32-bit integers.

2. **RSA Key Generation:**

   * Two distinct primes `p` and `q` are generated.
   * Computes `n = p * q` and Euler's totient `phi = (p-1)*(q-1)`.
   * Chooses public exponent `e` (default `65537`) ensuring `gcd(e, phi) = 1`.
   * Calculates the private exponent `d` as the modular inverse of `e` modulo `phi`.

3. **Encryption & Decryption:**

   * Encryption: `c = m^e % n` for each byte `m`.
   * Decryption: `m = c^d % n` for each ciphertext number `c`.
   * Modular exponentiation is performed efficiently using **exponentiation by squaring**.

---

## Usage

Compile the program with any C compiler:

```bash
gcc rsa.c -o rsa -Wall -W -O2
```

Run the program with a message as a command line argument:

```bash
./rsa "Hello World"
```

Example output:

```
p = 1237
q = 1543
n = 1910791
phi = 1910016
e = 65537
d = 276097

Ciphertext (numbers):
123456 789012 345678 ...

Decrypted message:
Hello World
```

---

## File Overview

* `rsa.c` : Main program implementing RSA encryption and decryption.
* Key functions:

  * `mul_mod`, `pow_mod` → safe modular arithmetic.
  * `miller_rabin_once`, `is_prime` → primality testing.
  * `rand_range`, `generate_prime` → random prime generation.
  * `gcd_u64`, `egcd`, `modinv` → number theory helpers for key generation.
  * `encrypt_byte`, `decrypt_byte` → RSA encryption/decryption per byte.

---

## Notes

* This implementation is for **educational purposes** and **small messages**.
* It **does not handle large primes** securely (limits defined by `LOWER` and `UPPER`).
* Decryption may fail for very large numbers because encryption is done **byte by byte**, not on full integer blocks.
* Avoid using this code for real cryptographic purposes in production.

---

## References

* [RSA Cryptosystem](https://en.wikipedia.org/wiki/RSA_%28cryptosystem%29)
* [Miller-Rabin Primality Test](https://en.wikipedia.org/wiki/M)
