// Reference https://nacl.cr.yp.to/
// Reference https://tweetnacl.cr.yp.to/software.html

#include <stdint.h>

#include <str.h>
#include <types.h>

#include <base64.h>
#include <crypto.h>
#include <die.h>
#include <memory.h>
#include <nacl.h>
#include <sha256.h>
#include <sha512.h>
#include <stdio.h> // fopen
#include <string.h> // memcpy memset

static FILE *fh_random;

void close_random(void)
	{
	if (fh_random)
		{
		fclose(fh_random);
		fh_random = 0;
		}
	}

static void read_random_bytes(u8 *result, unsigned long num_bytes)
	{
	if (!fh_random)
		{
		fh_random = fopen("/dev/urandom","r");
		if (!fh_random)
			die("could not open random byte source");
		}

	{
	size_t num_read = fread(result, 1, num_bytes, fh_random);
	if (num_read < num_bytes)
		die("out of random bytes");
	}
	}

// Return a string of n random bytes.
string str_random_bytes(unsigned long num_bytes)
	{
	string str = str_new(num_bytes);
	read_random_bytes((u8 *)str->data,num_bytes);
	return str;
	}

// Return a string of 24 random bytes.
string str_random_nonce(void)
	{
	return str_random_bytes(24);
	}

// Return a string of 32 random bytes.
string str_random_secret_key(void)
	{
	return str_random_bytes(32);
	}

static string op_public(void op(u8 *pk, const u8 *sk), string secret_key)
	{
	if (secret_key->len != 32) return 0;
	{
	string public_key = str_new(32);
	op(
		(u8 *)public_key->data,
		(const u8 *)secret_key->data
		);
	return public_key;
	}
	}

// Map the 32-byte secret_key to the corresponding 32-byte public box key.
string str_nacl_box_public(string secret_key)
	{ return op_public(nacl_box_public,secret_key); }

// Combine the 32-byte public and secret key into a 32-byte composite key which
// is used with str_nacl_box_seal and str_nacl_box_open.
//
// When sending, use the recipient's public key and the sender's secret key.
// When receiving, use the sender's public key and the recipient's secret key.
string str_nacl_box_prepare(string public_key, string secret_key)
	{
	if (public_key->len != 32) return 0;
	if (secret_key->len != 32) return 0;
	{
	string result = str_new(32);
	nacl_box_prepare
		(
		(u8 *)result->data,
		(const u8 *)public_key->data,
		(const u8 *)secret_key->data
		);
	return result;
	}
	}

static string op_box_pad
	(
	int op(u8 *c, const u8 *m, u64 d, const u8 *n, const u8 *k),
	const int num_pad_text,
	const int num_pad_result,
	string text,
	string nonce,
	string key
	)
	{
	if (nonce->len != 24) return 0;
	if (key->len != 32) return 0;
	{
	unsigned long buf_len = text->len + num_pad_text;
	char *buf_text = new_memory(buf_len);
	char *buf_result = new_memory(buf_len);
	string result;
	int code;

	memset(buf_text,0,num_pad_text);
	memcpy(buf_text + num_pad_text, text->data, text->len);

	code = op(
		(u8 *)buf_result,
		(const u8 *)buf_text,
		buf_len,
		(const u8 *)nonce->data,
		(const u8 *)key->data
		);

	if (code == -1)
		result = 0;
	else
		result = str_new_data(
			buf_result + num_pad_result,
			buf_len - num_pad_result);

	free_memory(buf_text,buf_len);
	free_memory(buf_result,buf_len);
	return result;
	}
	}

// Encrypt the plain text.
string str_nacl_box_seal(string text, string nonce, string key)
	{ return op_box_pad(nacl_box_seal,32,16,text,nonce,key); }

// Decrypt the crypt text.
string str_nacl_box_open(string crypt_text, string nonce, string key)
	{ return op_box_pad(nacl_box_open,16,32,crypt_text,nonce,key); }

// Map the 32-byte secret_key to the corresponding 32-byte public sign key.
string str_nacl_sign_public(string secret_key)
	{ return op_public(nacl_sign_public,secret_key); }

// Return the 64-byte signature of the text using the 32-byte public_key and
// 32-byte secret_key.
string str_nacl_sign_seal(string text, string public_key, string secret_key)
	{
	string result;
	if (public_key->len == 32 && secret_key->len == 32)
		{
		unsigned long n = text->len + 64;
		char *sm = new_memory(n);
		nacl_sign_seal
			(
			(u8 *)sm,
			(const u8 *)text->data, text->len,
			(const u8 *)public_key->data,
			(const u8 *)secret_key->data
			);
		result = str_new_data(sm,64);
		free_memory(sm,n);
		}
	else
		result = 0;
	return result;
	}

// Validate the signature on the text and return 1 if valid or 0 if invalid.
int str_nacl_sign_open(string text, string public_key, string signature)
	{
	int ok;
	if (public_key->len == 32 && signature->len == 64)
		{
		unsigned long n = text->len + 64;
		u8 *sm = new_memory(n);
		u8 *m = new_memory(n);
		int code;

		memcpy(sm,signature->data,64);
		memcpy(sm+64,text->data,text->len);

		code = nacl_sign_open((u8 *)m, (const u8 *)sm, n,
			(const u8 *)public_key->data);
		free_memory(sm,n);
		free_memory(m,n);
		ok = (code == 0);
		}
	else
		ok = 0;
	return ok;
	}

// Return the 32 byte SHA-256 hash of the text.
string str_sha256(string text)
	{
	string hash = str_new(32);
	sha256((u8 *)hash->data, (const u8 *)text->data, text->len);
	return hash;
	}

// Return the 64 byte SHA-512 hash of the text.
string str_sha512(string text)
	{
	string hash = str_new(64);
	sha512((u8 *)hash->data, (const u8 *)text->data, text->len);
	return hash;
	}

// Pack a base 64 format string into raw bytes.
string str_pack64(string text)
	{
	u64 n = text->len;
	u64 n_out;
	string out;

	if (text->data[n-1] == '=') n--;
	if (text->data[n-1] == '=') n--;
	n_out = 3*(n/4) + (n%4 == 0 ? 0 : n%4-1);

	out = str_new(n_out);
	pack64((u8 *)out->data,(const u8 *)text->data,n);
	return out;
	}

// Unpack raw bytes into a base 64 format string.
string str_unpack64(string text)
	{
	u64 n = text->len;
	u64 n_out = 4*(n/3) + (n%3 == 0 ? 0 : 4);
	string out = str_new(n_out);
	unpack64((u8 *)out->data,(const u8 *)text->data,text->len);
	return out;
	}

// Compute the HMAC value of the text using the key, using the given hash
// function and blocksize.  If the key is longer than blocksize, only the first
// blocksize bytes of the key are used.  If you want to use a key longer than
// blocksize bytes, you should hash it first.
static string str_hmac
	(
	void (*hash)(u8 digest[], const u8 *data, u64 n_data_byte),
	unsigned long blocksize,
	string text,
	string key
	)
	{
	unsigned long outsize = blocksize / 2;
	string mac = str_new(outsize);

	unsigned int len_ipad = blocksize + text->len;
	unsigned char *buf_ipad = new_memory(len_ipad);
	unsigned char buf_opad[blocksize+outsize];

	unsigned int key_len = key->len;

	unsigned int i;
	unsigned int fill;

	if (key_len > blocksize)
		key_len = blocksize;

	fill = blocksize - key_len;

	for (i = 0; i < key_len; i++)
		{
		buf_ipad[i] = key->data[i] ^ 0x36;
		buf_opad[i] = key->data[i] ^ 0x5c;
		}

	memset(buf_ipad + key_len, 0x36, fill);
	memset(buf_opad + key_len, 0x5c, fill);

	memcpy(buf_ipad+blocksize, text->data, text->len);
	hash(buf_opad+blocksize, buf_ipad, len_ipad);
	hash((u8 *)mac->data, buf_opad, blocksize+outsize);

	free_memory(buf_ipad,len_ipad);
	return mac;
	}

// Compute the HMAC-SHA512 value of the text using the key.
string str_hmac_sha512(string text, string key)
	{
	return str_hmac(sha512,128,text,key);
	}

// Compute the HMAC-SHA256 value of the text using the key.
string str_hmac_sha256(string text, string key)
	{
	return str_hmac(sha256,64,text,key);
	}
