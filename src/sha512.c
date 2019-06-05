#include <stdint.h>

#include <types.h>

#include <sha512.h>

/* Reference: https://en.wikipedia.org/wiki/SHA-2 */

static u64 R(u64 x, unsigned int n) { return (x >> n) | (x << (64 - n)); }
static u64 Ch(u64 x, u64 y, u64 z) { return (x & y) ^ (~x & z); }
static u64 Maj(u64 x, u64 y, u64 z) { return (x & y) ^ (x & z) ^ (y & z); }
static u64 Sigma0(u64 x) { return R(x,28) ^ R(x,34) ^ R(x,39); }
static u64 Sigma1(u64 x) { return R(x,14) ^ R(x,18) ^ R(x,41); }
static u64 sigma0(u64 x) { return R(x, 1) ^ R(x, 8) ^ (x >> 7); }
static u64 sigma1(u64 x) { return R(x,19) ^ R(x,61) ^ (x >> 6); }

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

/* Initialize array of round constants: first 64 bits of the fractional parts
of the cube roots of the first 80 primes 2..409. */
static const u64 k[80] =
	{
	0x428a2f98d728ae22, 0x7137449123ef65cd,
	0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
	0x3956c25bf348b538, 0x59f111f1b605d019,
	0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
	0xd807aa98a3030242, 0x12835b0145706fbe,
	0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
	0x72be5d74f27b896f, 0x80deb1fe3b1696b1,
	0x9bdc06a725c71235, 0xc19bf174cf692694,
	0xe49b69c19ef14ad2, 0xefbe4786384f25e3,
	0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
	0x2de92c6f592b0275, 0x4a7484aa6ea6e483,
	0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
	0x983e5152ee66dfab, 0xa831c66d2db43210,
	0xb00327c898fb213f, 0xbf597fc7beef0ee4,
	0xc6e00bf33da88fc2, 0xd5a79147930aa725,
	0x06ca6351e003826f, 0x142929670a0e6e70,
	0x27b70a8546d22ffc, 0x2e1b21385c26c926,
	0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
	0x650a73548baf63de, 0x766a0abb3c77b2a8,
	0x81c2c92e47edaee6, 0x92722c851482353b,
	0xa2bfe8a14cf10364, 0xa81a664bbc423001,
	0xc24b8b70d0f89791, 0xc76c51a30654be30,
	0xd192e819d6ef5218, 0xd69906245565a910,
	0xf40e35855771202a, 0x106aa07032bbd1b8,
	0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
	0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
	0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb,
	0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
	0x748f82ee5defb2fc, 0x78a5636f43172f60,
	0x84c87814a1f0ab72, 0x8cc702081a6439ec,
	0x90befffa23631e28, 0xa4506cebde82bde9,
	0xbef9a3f7b2c67915, 0xc67178f2e372532b,
	0xca273eceea26619c, 0xd186b8c721c0c207,
	0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
	0x06f067aa72176fba, 0x0a637dc5a2c898a6,
	0x113f9804bef90dae, 0x1b710b35131c471b,
	0x28db77f523047d84, 0x32caab7b40c72493,
	0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
	0x4cc5d4becb3e42b6, 0x597f299cfc657e2a,
	0x5fcb6fab3ad6faec, 0x6c44198c4a475817
	};

/* Process a 128-byte (1024-bit) chunk of the message. */
static void process_chunk(const u8 chunk[128], u64 h[8], u64 w[80])
	{
	u64 a[8];
	unsigned int i;

	/* Copy 1024-bit chunk into first 16 words of schedule. */
	for (i = 0; i < 16; i++)
		w[i] = roll_bytes(chunk+i*8,8);

	/* Extend the first 16 words into the remaining 64 words of schedule. */
	for (i = 16; i < 80; i++)
		{
		u64 s0 = sigma0(w[i-15]);
		u64 s1 = sigma1(w[i-2]);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
		}

	/* Initialize state to current hash value. */
	for (i = 0; i < 8; i++)
		a[i] = h[i];

	/* Compress the state. */
	for (i = 0; i < 80; i++)
		{
		u64 S1 = Sigma1(a[4]);
		u64 ch = Ch(a[4],a[5],a[6]);
		u64 T = a[7] + S1 + ch + k[i] + w[i];
		u64 S0 = Sigma0(a[0]);
		u64 maj = Maj(a[0],a[1],a[2]);
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

void sha512(u8 digest[64], const u8 *data, u64 n_data_byte)
	{
	/* Total number of data bits */
	const u64 n_data_bit = 8*n_data_byte;

	/* Total number of 128-byte (1024-bit) chunks */
	const u64 n_chunk = 1 + (n_data_bit + 1 + 128) / 1024;

	/* Total number of bytes */
	const u64 n_total_byte = 128*n_chunk;

	/* Number of full 128-byte data chunks */
	const u64 n_data_chunk = n_data_byte / 128;

	/* Number of data bytes remaining after any full chunks. */
	const u64 n_remain =  n_data_byte % 128;

	/* Number of zero padding bytes */
	const u64 n_pad = n_total_byte - n_data_byte - 17;

	u8 x[256]; /* Holds extra data after any full 128 byte blocks. */
	u64 i;

	/* Initialize hash values: first 64 bits of the fractional parts of the
	square roots of the first 8 primes 2..19. */
	u64 h[8] =
		{
		0x6a09e667f3bcc908, 0xbb67ae8584caa73b,
		0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1,
		0x510e527fade682d1, 0x9b05688c2b3e6c1f,
		0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
		};

	u64 w[80]; /* message schedule array */

	/* Process any full 128-byte data chunks. */
	for (i = 0; i < n_data_chunk; i++)
		process_chunk(data+128*i,h,w);

	/* Add any data bytes which remain after any full blocks. */
	for (i = 0; i < n_remain; i++)
		x[i] = data[128*n_data_chunk+i];

	/* Add padding. */
	x[n_remain] = 0x80;
	for (i = n_remain+1; i < n_remain+n_pad+1; i++)
		x[i] = 0;

	/* Append n_data_bit as a 128-bit big-ending integer. */
	fill_bytes(x+i,n_data_bit,16);

	/* Note that (n_chunk-n_data_chunk) is either 1 or 2. */
	for (i = 0; i < n_chunk-n_data_chunk; i++)
		process_chunk(x+128*i,h,w);

	/* Flatten 64-bit words to bytes. */
	for (i = 0; i < 8; i++)
		fill_bytes(digest+8*i,h[i],8);
	}
