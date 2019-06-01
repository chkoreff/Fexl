#include <num.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <crypto.h>
#include <standard.h>
#include <type_crypto.h>
#include <type_num.h>
#include <type_str.h>

value type_random_bytes(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qstr(random_bytes(get_ulong(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_random_nonce(value f)
	{
	(void)f;
	return Qstr(random_nonce());
	}

value type_random_secret_key(value f)
	{
	(void)f;
	return Qstr(random_secret_key());
	}

value type_crypto_box_public(value f)
	{ return op_str(f,crypto_box_public); }

value type_crypto_box_prepare(value f)
	{ return op_str2(f,crypto_box_prepare); }

value type_crypto_box_seal(value f)
	{ return op_str3(f,crypto_box_seal); }

value type_crypto_box_open(value f)
	{ return op_str3(f,crypto_box_open); }

value type_crypto_sign_public(value f)
	{ return op_str(f,crypto_sign_public); }

value type_crypto_sign_seal(value f)
	{ return op_str3(f,crypto_sign_seal); }

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
		f = boolean(crypto_sign_open(text,public_key,signature));
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

value type_sha512(value f) { return op_str(f,sha512); }
