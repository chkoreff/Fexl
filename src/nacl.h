extern void nacl_box_public(u8 *pk, const u8 *sk);
extern void nacl_box_prepare(u8 *k, const u8 *pk, const u8 *sk);
extern int nacl_box_seal(u8 *c, const u8 *m, u64 d, const u8 *n, const u8 *k);
extern int nacl_box_open(u8 *m, const u8 *c, u64 d, const u8 *n, const u8 *k);
extern void nacl_sign_public(u8 *pk, const u8 *sk);
extern void nacl_sign_seal(u8 *sm, const u8 *m, u64 n, const u8 *pk,
	const u8 *sk);
extern int nacl_sign_open(u8 *m, const u8 *sm, u64 mlen, const u8 *pk);
