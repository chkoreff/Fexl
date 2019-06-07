extern void close_random(void);
extern string str_random_bytes(unsigned long num_bytes);
extern string str_random_nonce(void);
extern string str_random_secret_key(void);
extern string str_nacl_box_public(string secret_key);
extern string str_nacl_box_prepare(string public_key, string secret_key);
extern string str_nacl_box_seal(string text, string nonce, string key);
extern string str_nacl_box_open(string crypt_text, string nonce, string key);
extern string str_nacl_sign_public(string secret_key);
extern string str_nacl_sign_seal(string text, string public_key,
	string secret_key);
extern int str_nacl_sign_open(string text, string public_key, string signature);
extern string str_sha256(string text);
extern string str_sha512(string text);
extern string str_pack64(string text);
extern string str_unpack64(string text);
extern string str_hmac_sha512(string text, string key);
