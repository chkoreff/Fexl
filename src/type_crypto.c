#include <str.h>
#include <value.h>

#include <basic.h>
#include <crypto.h>
#include <type_crypto.h>
#include <type_num.h>
#include <type_str.h>

value type_random_bytes(value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_num)
		f = Qstr(str_random_bytes(get_ulong(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	(void)fun;
	}

value type_random_nonce(value fun, value f)
	{
	return Qstr(str_random_nonce());
	(void)fun;
	(void)f;
	}

value type_random_secret_key(value fun, value f)
	{
	return Qstr(str_random_secret_key());
	(void)fun;
	(void)f;
	}

value type_nacl_box_public(value fun, value f)
	{ return op_str(fun,f,str_nacl_box_public); }

value type_nacl_box_prepare(value fun, value f)
	{ return op_str2(fun,f,str_nacl_box_prepare); }

value type_nacl_box_seal(value fun, value f)
	{ return op_str3(fun,f,str_nacl_box_seal); }

value type_nacl_box_open(value fun, value f)
	{ return op_str3(fun,f,str_nacl_box_open); }

value type_nacl_sign_public(value fun, value f)
	{ return op_str(fun,f,str_nacl_sign_public); }

value type_nacl_sign_seal(value fun, value f)
	{ return op_str3(fun,f,str_nacl_sign_seal); }

value type_nacl_sign_open(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	if (fun->L->L == 0) return keep(fun,f);
	{
	value x = arg(fun->L->R);
	value y = arg(fun->R);
	value z = arg(f->R);
	if (x->T == type_str && y->T == type_str && z->T == type_str)
		{
		string text = get_str(x);
		string public_key = get_str(y);
		string signature = get_str(z);
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

value type_sha256(value fun, value f) { return op_str(fun,f,str_sha256); }
value type_sha512(value fun, value f) { return op_str(fun,f,str_sha512); }
value type_pack64(value fun, value f) { return op_str(fun,f,str_pack64); }
value type_unpack64(value fun, value f) { return op_str(fun,f,str_unpack64); }

value type_hmac_sha512(value fun, value f)
	{ return op_str2(fun,f,str_hmac_sha512); }
value type_hmac_sha256(value fun, value f)
	{ return op_str2(fun,f,str_hmac_sha256); }
