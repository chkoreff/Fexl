extern void close_random(void);
extern string random_bytes(unsigned long num_bytes);
extern string random_nonce(void);
extern string random_secret_key(void);
extern string crypto_box_public(string secret_key);
extern string crypto_box_prepare(string public_key, string secret_key);
extern string crypto_box_seal(string text, string nonce, string key);
extern string crypto_box_open(string crypt_text, string nonce, string key);
extern string crypto_sign_public(string secret_key);
extern string crypto_sign_seal(string text, string public_key,
	string secret_key);
extern int crypto_sign_open(string text, string public_key, string signature);
extern string sha512(string text);
