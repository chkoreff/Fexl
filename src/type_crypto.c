#include <str.h>
#include <value.h>

#include <basic.h>
#include <crypto.h>
#include <type_crypto.h>
#include <type_num.h>
#include <type_str.h>

static value type_random_bytes(value f)
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qstr(str_random_bytes(get_ulong(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value type_random_nonce(value f)
	{
	return Qstr(str_random_nonce());
	(void)f;
	}

static value type_random_secret_key(value f)
	{
	return Qstr(str_random_secret_key());
	(void)f;
	}

static value type_nacl_box_public(value f)
	{ return op_str(f,str_nacl_box_public); }

static value type_nacl_box_prepare(value f)
	{ return op_str2(f,str_nacl_box_prepare); }

static value type_nacl_box_seal(value f)
	{ return op_str3(f,str_nacl_box_seal); }

static value type_nacl_box_open(value f)
	{ return op_str3(f,str_nacl_box_open); }

static value type_nacl_sign_public(value f)
	{ return op_str(f,str_nacl_sign_public); }

static value type_nacl_sign_seal(value f)
	{ return op_str3(f,str_nacl_sign_seal); }

static value type_nacl_sign_open(value f)
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	{
	value x = arg(f->L->L->R);
	value y = arg(f->L->R);
	value z = arg(f->R);
	if (x->T == type_str && y->T == type_str && z->T == type_str)
		{
		string text = x->v_ptr;
		string public_key = y->v_ptr;
		string signature = z->v_ptr;
		f = boolean(str_nacl_sign_open(text,public_key,signature));
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	drop(z);
	return f;
	}
	}

static value type_sha256(value f) { return op_str(f,str_sha256); }
static value type_sha512(value f) { return op_str(f,str_sha512); }
static value type_pack64(value f) { return op_str(f,str_pack64); }
static value type_unpack64(value f) { return op_str(f,str_unpack64); }

static value type_hmac_sha512(value f)
	{ return op_str2(f,str_hmac_sha512); }
static value type_hmac_sha256(value f)
	{ return op_str2(f,str_hmac_sha256); }

void define_crypto(void)
	{
	define("random_bytes",Q(type_random_bytes));
	define("random_nonce",Q0(type_random_nonce));
	define("random_secret_key",Q0(type_random_secret_key));
	define("nacl_box_public",Q(type_nacl_box_public));
	define("nacl_box_prepare",Q(type_nacl_box_prepare));
	define("nacl_box_seal",Q(type_nacl_box_seal));
	define("nacl_box_open",Q(type_nacl_box_open));
	define("nacl_sign_public",Q(type_nacl_sign_public));
	define("nacl_sign_seal",Q(type_nacl_sign_seal));
	define("nacl_sign_open",Q(type_nacl_sign_open));
	define("sha256",Q(type_sha256));
	define("sha512",Q(type_sha512));
	define("pack64",Q(type_pack64));
	define("unpack64",Q(type_unpack64));
	define("hmac_sha512",Q(type_hmac_sha512));
	define("hmac_sha256",Q(type_hmac_sha256));
	}

void beg_crypto(void)
	{
	}

void end_crypto(void)
	{
	close_random();
	}
