#include <num.h>
#include <str.h>
#include <types.h>
#include <value.h>

#include <basic.h>
#include <crypto.h>
#include <die.h>
#include <standard.h>

#include <memory.h>
#include <stdio.h>
#include <string.h> /* memset memcpy */
#include <type_crypto.h>
#include <type_hex.h>
#include <type_num.h>
#include <type_str.h>

static FILE *fh_random;

static void random_bytes(u8 *result, unsigned long num_bytes)
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

void close_random(void)
	{
	if (fh_random)
		{
		fclose(fh_random);
		fh_random = 0;
		}
	}

static string str_random_bytes(unsigned long num_bytes)
	{
	string str = str_new(num_bytes);
	random_bytes((u8 *)str->data,num_bytes);
	return str;
	}

static value op_public_key(value f, int op(u8 *pk, const u8 *sk))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		string secret_key = get_str(x);
		if (secret_key->len == 32)
			{
			string public_key = str_new(32);
			op(
				(u8 *)public_key->data,
				(const u8 *)secret_key->data
				);
			f = Qstr(public_key);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

static string str_box
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
		(u64)buf_len,
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

value op_box(
	value f,
	int op(u8 *c, const u8 *m, u64 d, const u8 *n, const u8 *k),
	const int num_pad_text,
	const int num_pad_result
	)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value text = arg(f->L->L->R);
	value nonce = arg(f->L->R);
	value key = arg(f->R);

	if (1
		&& text->T == type_str
		&& nonce->T == type_str
		&& key->T == type_str
		)
		{
		string result = str_box(op,num_pad_text,num_pad_result,
			get_str(text),
			get_str(nonce),
			get_str(key));

		if (result)
			f = Qstr(result);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);

	drop(text);
	drop(nonce);
	drop(key);
	return f;
	}
	}

/* (random_bytes n) Returns a string of n random bytes. */
value type_random_bytes(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qstr(str_random_bytes(get_ulong(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (random_nonce) Returns a string of 24 random bytes. */
value type_random_nonce(value f)
	{
	(void)f;
	return Qstr(str_random_bytes(24));
	}

/* (random_secret_key) Returns a string of 32 random bytes. */
value type_random_secret_key(value f)
	{
	(void)f;
	return Qstr(str_random_bytes(32));
	}

/* (crypto_box_public secret_key)
Map the 32-byte secret_key into the corresponding 32-byte public box key.
*/
value type_crypto_box_public(value f)
	{
	return op_public_key(f,crypto_box_public);
	}

/* (crypto_box_prepare public_key secret_key)
Combine the 32-byte public and secret key into a 32-byte composite key which is
used with crypto_box_seal and crypto_box_open.

When sending, use the recipient's public key and the sender's secret key.
When receiving, use the sender's public key and the recipient's secret key.
*/
value type_crypto_box_prepare(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		{
		string public_key = get_str(x);
		string secret_key = get_str(y);
		if (public_key->len == 32 && secret_key->len == 32)
			{
			string result = str_new(32);
			crypto_box_prepare
				(
				(u8 *)result->data,
				(const u8 *)public_key->data,
				(const u8 *)secret_key->data
				);
			f = Qstr(result);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (crypto_box_seal plain_text nonce key) Encrypt the plain text.  */
value type_crypto_box_seal(value f)
	{
	return op_box(f,crypto_box_seal,32,16);
	}

/* (crypto_box_open crypt_text nonce key) Encrypt the crypt text.  */
value type_crypto_box_open(value f)
	{
	return op_box(f,crypto_box_open,16,32);
	}

/* (crypto_sign_public secret_key)
Map the 32-byte secret_key into the corresponding 32-byte public sign key.
*/
value type_crypto_sign_public(value f)
	{
	return op_public_key(f,crypto_sign_public);
	}

/* (crypto_sign_seal text public_key secret_key)
Return the 64-byte signature of the text using the 32-byte public_key and
32-byte secret_key.
*/
value type_crypto_sign_seal(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	value y = arg(f->L->R);
	value z = arg(f->R);
	if (x->T == type_str && y->T == type_str && z->T == type_str)
		{
		string text = get_str(x);
		string public_key = get_str(y);
		string secret_key = get_str(z);
		if (public_key->len == 32 && secret_key->len == 32)
			{
			unsigned long n = text->len + 64;
			char *sm = new_memory(n);
			crypto_sign_seal
				(
				(u8 *)sm,
				(const u8 *)text->data,
				(u64)text->len,
				(const u8 *)public_key->data,
				(const u8 *)secret_key->data
				);
			f = Qstr(str_new_data(sm,64));
			free_memory(sm,n);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

/* (crypto_sign_open text public_key signature)
Validate the signature on the text and return T if valid or F if invalid.
Return void if anything is malformed.
*/
value type_crypto_sign_open(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value x = arg(f->L->L->R);
	value y = arg(f->L->R);
	value z = arg(f->R);
	if (x->T == type_str && y->T == type_str && z->T == type_str)
		{
		string text = get_str(x);
		string public_key = get_str(y);
		string signature = get_str(z);
		if (public_key->len == 32 && signature->len == 64)
			{
			unsigned long n = text->len + 64;
			u8 *sm = new_memory(n);
			u8 *m = new_memory(n);
			int code;

			memcpy(sm,signature->data,64);
			memcpy(sm+64,text->data,text->len);

			code = crypto_sign_open
				(
				(u8 *)m,
				(const u8 *)sm,
				(u64)n,
				(const u8 *)public_key->data
				);

			f = boolean(code == 0);

			free_memory(sm,n);
			free_memory(m,n);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

/* (crypto_hash text) Return the 64 byte SHA-512 hash of the text. */
value type_crypto_hash(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		string text = get_str(x);
		string hash = str_new(64);
		crypto_hash
			(
			(u8 *)hash->data,
			(const u8 *)text->data,
			(u64)text->len
			);
		f = Qstr(hash);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
