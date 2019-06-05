#include <stdint.h>

#include <types.h>

#include <sha256.h>

/* Reference: https://en.wikipedia.org/wiki/SHA-2 */

static u32 R(u32 x, unsigned int n) { return (x >> n) | (x << (32-n)); }
static u32 Ch(u32 x, u32 y, u32 z) { return (x & y) ^ (~x & z); }
static u32 Maj(u32 x, u32 y, u32 z) { return (x & y) ^ (x & z) ^ (y & z); }
static u32 Sigma0(u32 x) { return R(x, 2) ^ R(x,13) ^ R(x,22); }
static u32 Sigma1(u32 x) { return R(x, 6) ^ R(x,11) ^ R(x,25); }
static u32 sigma0(u32 x) { return R(x, 7) ^ R(x,18) ^ (x >> 3); }
static u32 sigma1(u32 x) { return R(x,17) ^ R(x,19) ^ (x >> 10); }

/* Roll up n bytes from string x into a word in big-endian order. */
static u64 roll_bytes(const u8 *x, unsigned int n)
	{
	unsigned int i;
	u64 u = 0;
	for (i = 0; i < n; i++)
		u = (u<<8) | x[i];
	return u;
	}

/* Fill n bytes from word u into string x in big-endian order. */
static void fill_bytes(u8 *x, u64 u, unsigned int n)
	{
	unsigned int i;
	for (i = 0; i < n; i++)
		{
		x[n-1-i] = u;
		u >>= 8;
		}
	}

/* Initialize array of round constants: first 32 bits of the fractional parts
of the cube roots of the first 64 primes 2..311. */
static const u32 k[64] =
	{
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
	0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,

	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
	0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,

	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
	0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,

	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
	0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,

	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
	0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,

	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
	0xd192e819,0xd6990624,0xf40e3585,0x106aa070,

	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
	0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,

	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
	0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
	};

/* Process a 64-byte (512-bit) chunk of the message. */
static void process_chunk(const u8 chunk[64], u32 h[8], u32 w[64])
	{
	u32 a[8];
	unsigned int i;

	/* Copy 512-bit chunk into first 16 words of schedule. */
	for (i = 0; i < 16; i++)
		w[i] = roll_bytes((const u8 *)chunk+i*4,4);

	/* Extend the first 16 words into the remaining 48 words of schedule. */
	for (i = 16; i < 64; i++)
		{
		u32 s0 = sigma0(w[i-15]);
		u32 s1 = sigma1(w[i-2]);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
		}

	/* Initialize state to current hash value. */
	for (i = 0; i < 8; i++)
		a[i] = h[i];

	/* Compress the state. */
	for (i = 0; i < 64; i++)
		{
		u32 S1 = Sigma1(a[4]);
		u32 ch = Ch(a[4],a[5],a[6]);
		u32 T = a[7] + S1 + ch + k[i] + w[i];
		u32 S0 = Sigma0(a[0]);
		u32 maj = Maj(a[0],a[1],a[2]);
		unsigned int j;
		/* Shift state to the right. */
		for (j = 7; j > 0; j--)
			a[j] = a[j-1];

		a[4] += T;
		a[0] = T + S0 + maj;
		}

	/* Add compressed state to current hash. */
	for (i = 0; i < 8; i++)
		h[i] += a[i];
	}

/*
Note that the SHA256 specification allows a maximum of 2^61 data bytes.  That's
because the length in bits is included in the padding at the end, and that can
be at most 2^64.
*/
void sha256(u8 digest[32], const u8 *data, u64 n_data_byte)
	{
	/* Total number of data bits */
	const u64 n_data_bit = 8*n_data_byte;

	/* Total number of 64-byte (512-bit) chunks */
	const u64 n_chunk = 1 + (n_data_bit + 1 + 64) / 512;

	/* Total number of bytes */
	const u64 n_total_byte = 64*n_chunk;

	/* Number of full 64-byte data chunks */
	const u64 n_data_chunk = n_data_byte / 64;

	/* Number of data bytes remaining after any full chunks. */
	const u64 n_remain =  n_data_byte % 64;

	/* Number of zero padding bytes */
	const u64 n_pad = n_total_byte - n_data_byte - 9;

	u8 x[128]; /* Holds extra data after any full 64 byte blocks. */
	u64 i;

	/* Initialize hash values: first 32 bits of the fractional parts of the
	square roots of the first 8 primes 2..19. */
	u32 h[8] =
		{
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
		};

	u32 w[64]; /* message schedule array */

	/* Process any full 64-byte data chunks. */
	for (i = 0; i < n_data_chunk; i++)
		process_chunk(data+64*i,h,w);

	/* Add any data bytes which remain after any full blocks. */
	for (i = 0; i < n_remain; i++)
		x[i] = (u8)data[64*n_data_chunk+i];

	/* Add padding. */
	x[n_remain] = 0x80;
	for (i = n_remain+1; i < n_remain+n_pad+1; i++)
		x[i] = 0;

	/* Append n_data_bit as a 64-bit big-ending integer. */
	fill_bytes(x+i,n_data_bit,8);

	/* Note that (n_chunk-n_data_chunk) is either 1 or 2. */
	for (i = 0; i < n_chunk-n_data_chunk; i++)
		process_chunk(x+64*i,h,w);

	/* Flatten 32-bit words to bytes. */
	for (i = 0; i < 8; i++)
		fill_bytes(digest+4*i,h[i],4);
	}
