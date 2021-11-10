struct bn
	{
	u32 sign;
	u32 nsd;
	u32 len;
	u32 vec[0];
	};

extern struct bn *bn_new(u64 n);
extern void bn_free(struct bn *x1);
extern int bn_eq0(const struct bn *x);
extern int bn_cmp(const struct bn *x, const struct bn *y);
extern struct bn *bn_add(const struct bn *x, const struct bn *y);
extern struct bn *bn_sub(const struct bn *x, const struct bn *y);
extern struct bn *bn_mul(const struct bn *x, const struct bn *y);
extern void bn_div
	(
	const struct bn *x,
	const struct bn *y,
	struct bn **qp,
	struct bn **rp
	);
extern struct bn *bn_from_dec(const char *s);
extern string bn_to_dec(const struct bn *x);
