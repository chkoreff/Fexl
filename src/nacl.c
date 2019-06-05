#include <stdint.h>

#include <types.h>

#include <nacl.h>
#include <sha512.h>

typedef i64 gf[16];

static const u8 _0[16];
static const u8 _9[32] = {9};
static const gf gf0;
static const gf gf1 = {1};
static const gf _121665 = {0xDB41,1};

static const gf D =
	{
	0x78a3, 0x1359, 0x4dca, 0x75eb, 0xd8ab, 0x4141, 0x0a4d, 0x0070,
	0xe898, 0x7779, 0x4079, 0x8cc7, 0xfe73, 0x2b6f, 0x6cee, 0x5203
	};

static const gf D2 =
	{
	0xf159, 0x26b2, 0x9b94, 0xebd6, 0xb156, 0x8283, 0x149a, 0x00e0,
	0xd130, 0xeef3, 0x80f2, 0x198e, 0xfce7, 0x56df, 0xd9dc, 0x2406
	};

static const gf X =
	{
	0xd51a, 0x8f25, 0x2d60, 0xc956, 0xa7b2, 0x9525, 0xc760, 0x692c,
	0xdc5c, 0xfdd6, 0xe231, 0xc0a4, 0x53fe, 0xcd6e, 0x36d3, 0x2169
	};

static const gf Y =
	{
	0x6658, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666,
	0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666
	};

static const gf I =
	{
	0xa0b0, 0x4a0e, 0x1b27, 0xc4ee, 0xe478, 0xad2f, 0x1806, 0x2f43,
	0xd7a7, 0x3dfb, 0x0099, 0x2b4d, 0xdf0b, 0x4fc1, 0x2480, 0x2b83
	};

static u32 L32(u32 x, int c)
	{
	return (x << c) | ((x&0xffffffff) >> (32 - c));
	}

static u32 ld32(const u8 *x)
	{
	u32 u = x[3];
	u = (u<<8)|x[2];
	u = (u<<8)|x[1];
	return (u<<8)|x[0];
	}

static void st32(u8 *x, u32 u)
	{
	int i;
	for (i = 0; i < 4; ++i)
		{
		x[i] = u;
		u >>= 8;
		}
	}

static int vn(const u8 *x, const u8 *y, int n)
	{
	u32 d = 0;
	int i;
	for (i = 0; i < n; ++i)
		d |= x[i]^y[i];

	return (1 & ((d - 1) >> 8)) - 1;
	}

static int verify_16(const u8 *x, const u8 *y)
	{
	return vn(x,y,16);
	}

static int verify_32(const u8 *x, const u8 *y)
	{
	return vn(x,y,32);
	}

static void core(u8 *out, const u8 *in, const u8 *k, const u8 *c, int h)
	{
	u32 w[16], x[16], y[16], t[4];
	int i,j,m;

	for (i = 0; i < 4; ++i)
		{
		x[5*i] = ld32(c+4*i);
		x[1+i] = ld32(k+4*i);
		x[6+i] = ld32(in+4*i);
		x[11+i] = ld32(k+16+4*i);
		}

	for (i = 0; i < 16; ++i)
		y[i] = x[i];

	for (i = 0; i < 20; ++i)
		{
		for (j = 0; j < 4; ++j)
			{
			for (m = 0; m < 4; ++m)
				t[m] = x[(5*j+4*m)%16];

			t[1] ^= L32(t[0]+t[3], 7);
			t[2] ^= L32(t[1]+t[0], 9);
			t[3] ^= L32(t[2]+t[1],13);
			t[0] ^= L32(t[3]+t[2],18);
			for (m = 0; m < 4; ++m)
				w[4*j+(j+m)%4] = t[m];
			}
		for (m = 0; m < 16; ++m)
			x[m] = w[m];
		}

	if (h)
		{
		for (i = 0; i < 16; ++i)
			x[i] += y[i];
		for (i = 0; i < 4; ++i)
			{
			x[5*i] -= ld32(c+4*i);
			x[6+i] -= ld32(in+4*i);
			}
		for (i = 0; i < 4; ++i)
			{
			st32(out+4*i,x[5*i]);
			st32(out+16+4*i,x[6+i]);
			}
		}
	else
		for (i = 0; i < 16; ++i)
			st32(out + 4 * i,x[i] + y[i]);
	}

static void core_salsa20(u8 *out, const u8 *in, const u8 *k, const u8 *c)
	{
	core(out,in,k,c,0);
	}

static void core_hsalsa20(u8 *out, const u8 *in, const u8 *k, const u8 *c)
	{
	core(out,in,k,c,1);
	}

static const u8 sigma[16] = "expand 32-byte k";

static void stream_salsa20_xor(u8 *c, const u8 *m, u64 b, const u8 *n,
	const u8 *k)
	{
	u8 z[16],x[64];
	u32 u,i;

	if (!b) return;

	for (i = 0; i < 16; ++i)
		z[i] = 0;
	for (i = 0; i < 8; ++i)
		z[i] = n[i];

	while (b >= 64)
		{
		core_salsa20(x,z,k,sigma);
		for (i = 0; i < 64; ++i)
			c[i] = (m?m[i]:0) ^ x[i];
		u = 1;
		for (i = 8; i < 16; ++i)
			{
			u += (u32) z[i];
			z[i] = u;
			u >>= 8;
			}
		b -= 64;
		c += 64;
		if (m) m += 64;
		}

	if (b)
		{
		core_salsa20(x,z,k,sigma);
		for (i = 0; i < b; ++i)
			c[i] = (m?m[i]:0) ^ x[i];
		}
	}

static void stream_salsa20(u8 *c, u64 d, const u8 *n, const u8 *k)
	{
	stream_salsa20_xor(c,0,d,n,k);
	}

static void stream(u8 *c, u64 d, const u8 *n, const u8 *k)
	{
	u8 s[32];
	core_hsalsa20(s,n,k,sigma);
	stream_salsa20(c,d,n+16,s);
	}

static void stream_xor(u8 *c, const u8 *m, u64 d, const u8 *n,
	const u8 *k)
	{
	u8 s[32];
	core_hsalsa20(s,n,k,sigma);
	stream_salsa20_xor(c,m,d,n+16,s);
	}

static void add1305(u32 *h, const u32 *c)
	{
	u32 j,u = 0;
	for (j = 0; j < 17; ++j)
		{
		u += h[j] + c[j];
		h[j] = u & 255;
		u >>= 8;
		}
	}

static const u32 minusp[17] =
	{5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 252};

static void onetimeauth(u8 *out, const u8 *m, u64 n, const u8 *k)
	{
	u32 s,i,j,u,x[17],r[17],h[17],c[17],g[17];

	for (j = 0; j < 17; ++j)
		r[j]=h[j]=0;

	for (j = 0; j < 16; ++j)
		r[j]=k[j];

	r[3]&=15;
	r[4]&=252;
	r[7]&=15;
	r[8]&=252;
	r[11]&=15;
	r[12]&=252;
	r[15]&=15;

	while (n > 0)
		{
		for (j = 0; j < 17; ++j)
			c[j] = 0;
		for (j = 0; (j < 16) && (j < n); ++j)
			c[j] = m[j];

		c[j] = 1;
		m += j; n -= j;
		add1305(h,c);

		for (i = 0; i < 17; ++i)
			{
			x[i] = 0;
			for (j = 0; j < 17; ++j)
				x[i] += h[j] * ((j <= i) ? r[i - j] : 320 * r[i + 17 - j]);
			}
		for (i = 0; i < 17; ++i)
			h[i] = x[i];
		u = 0;

		for (j = 0; j < 16; ++j)
			{
			u += h[j];
			h[j] = u & 255;
			u >>= 8;
			}
		u += h[16]; h[16] = u & 3;
		u = 5 * (u >> 2);

		for (j = 0; j < 16; ++j)
			{
			u += h[j];
			h[j] = u & 255;
			u >>= 8;
			}
		u += h[16]; h[16] = u;
		}

	for (j = 0; j < 17; ++j)
		g[j] = h[j];

	add1305(h,minusp);
	s = -(h[16] >> 7);
	for (j = 0; j < 17; ++j)
		h[j] ^= s & (g[j] ^ h[j]);

	for (j = 0; j < 16; ++j)
		c[j] = k[j + 16];

	c[16] = 0;
	add1305(h,c);

	for (j = 0; j < 16; ++j)
		out[j] = h[j];
	}

static int onetimeauth_verify(const u8 *h, const u8 *m, u64 n,
	const u8 *k)
	{
	u8 x[16];
	onetimeauth(x,m,n,k);
	return verify_16(h,x);
	}

static void set25519(gf r, const gf a)
	{
	int i;
	for (i = 0; i < 16; ++i)
		r[i]=a[i];
	}

static void car25519(gf o)
	{
	int i;
	i64 c;
	for (i = 0; i < 16; ++i)
		{
		o[i]+=(1LL<<16);
		c=o[i]>>16;
		o[(i+1)*(i<15)]+=c-1+37*(c-1)*(i==15);
		o[i]-=c<<16;
		}
	}

static void sel25519(gf p, gf q, int b)
	{
	int i;
	i64 t,c=~(b-1);
	for (i = 0; i < 16; ++i)
		{
		t= c&(p[i]^q[i]);
		p[i]^=t;
		q[i]^=t;
		}
	}

static void pack25519(u8 *o, const gf n)
	{
	int i,j,b;
	gf m,t;
	for (i = 0; i < 16; ++i)
		t[i]=n[i];

	car25519(t);
	car25519(t);
	car25519(t);
	for (j = 0; j < 2; ++j)
		{
		m[0]=t[0]-0xffed;
		for(i=1; i<15; i++)
			{
			m[i]=t[i]-0xffff-((m[i-1]>>16)&1);
			m[i-1]&=0xffff;
			}
		m[15]=t[15]-0x7fff-((m[14]>>16)&1);
		b=(m[15]>>16)&1;
		m[14]&=0xffff;
		sel25519(t,m,1-b);
		}
	for (i = 0;i < 16;++i)
		{
		o[2*i]=t[i]&0xff;
		o[2*i+1]=t[i]>>8;
		}
	}

static int neq25519(const gf a, const gf b)
	{
	u8 c[32],d[32];
	pack25519(c,a);
	pack25519(d,b);
	return verify_32(c,d);
	}

static u8 par25519(const gf a)
	{
	u8 d[32];
	pack25519(d,a);
	return d[0]&1;
	}

static void unpack25519(gf o, const u8 *n)
	{
	int i;
	for (i = 0; i < 16; ++i)
		o[i] = n[2*i]+((i64)n[2*i+1]<<8);
	o[15]&=0x7fff;
	}

static void A(gf o, const gf a, const gf b)
	{
	int i;
	for (i = 0; i < 16; ++i)
		o[i] = a[i]+b[i];
	}

static void Z(gf o, const gf a, const gf b)
	{
	int i;
	for (i = 0; i < 16; ++i)
		o[i] = a[i]-b[i];
	}

static void M(gf o, const gf a, const gf b)
	{
	i64 t[31];
	int i,j;
	for (i = 0; i < 31; ++i)
		t[i] = 0;

	for (i = 0; i < 16; ++i)
		for (j = 0; j < 16; ++j)
			t[i+j] += a[i]*b[j];

	for (i = 0; i < 15; ++i)
		t[i] += 38*t[i+16];

	for (i = 0; i < 16; ++i)
		o[i] = t[i];

	car25519(o);
	car25519(o);
	}

static void S(gf o, const gf a)
	{
	M(o,a,a);
	}

static void inv25519(gf o, const gf i)
	{
	gf c;
	int a;
	for (a = 0; a < 16; ++a)
		c[a] = i[a];

	for(a=253; a>=0; a--)
		{
		S(c,c);
		if (a != 2 && a != 4)
			M(c,c,i);
		}
	for (a = 0; a < 16; ++a)
		o[a] = c[a];
	}

static void pow2523(gf o, const gf i)
	{
	gf c;
	int a;
	for (a = 0; a < 16; ++a)
		c[a] = i[a];

	for (a=250; a>=0; a--)
		{
		S(c,c);
		if (a != 1)
			M(c,c,i);
		}
	for (a = 0; a < 16; ++a)
		o[a] = c[a];
	}

static void curve_scalarmult(u8 *q, const u8 *n, const u8 *p)
	{
	u8 z[32];
	i64 x[80],r;
	int i;
	gf a,b,c,d,e,f;

	for (i = 0; i < 31; ++i)
		z[i]=n[i];

	z[31] = (n[31]&127)|64;
	z[0] &= 248;
	unpack25519(x,p);

	for (i = 0; i < 16; ++i)
		{
		b[i] = x[i];
		d[i] = a[i] = c[i] = 0;
		}
	a[0] = d[0] = 1;
	for (i=254; i>=0; --i)
		{
		r = (z[i>>3]>>(i&7))&1;
		sel25519(a,b,r);
		sel25519(c,d,r);
		A(e,a,c);
		Z(a,a,c);
		A(c,b,d);
		Z(b,b,d);
		S(d,e);
		S(f,a);
		M(a,c,a);
		M(c,b,e);
		A(e,a,c);
		Z(a,a,c);
		S(b,a);
		Z(c,d,f);
		M(a,c,_121665);
		A(a,a,d);
		M(c,c,a);
		M(a,d,f);
		M(d,b,x);
		S(b,e);
		sel25519(a,b,r);
		sel25519(c,d,r);
		}
	for (i = 0; i < 16; ++i)
		{
		x[i+16] = a[i];
		x[i+32] = c[i];
		x[i+48] = b[i];
		x[i+64] = d[i];
		}
	inv25519(x+32,x+32);
	M(x+16,x+16,x+32);
	pack25519(q,x+16);
	}

void nacl_box_public(u8 *pk, const u8 *sk)
	{
	curve_scalarmult(pk,sk,_9);
	}

void nacl_box_prepare(u8 *k, const u8 *pk, const u8 *sk)
	{
	u8 s[32];
	curve_scalarmult(s,sk,pk);
	core_hsalsa20(k,_0,s,sigma);
	}

int nacl_box_seal(u8 *c, const u8 *m, u64 d, const u8 *n, const u8 *k)
	{
	int i;
	if (d < 32) return -1;
	stream_xor(c,m,d,n,k);
	onetimeauth(c + 16,c + 32,d - 32,c);
	for (i = 0; i < 16; ++i)
		c[i] = 0;
	return 0;
	}

int nacl_box_open(u8 *m, const u8 *c, u64 d, const u8 *n, const u8 *k)
	{
	int i;
	u8 x[32];
	if (d < 32) return -1;
	stream(x,32,n,k);
	if (onetimeauth_verify(c + 16,c + 32,d - 32,x) != 0)
		return -1;
	stream_xor(m,c,d,n,k);
	for (i = 0; i < 32; ++i)
		m[i] = 0;
	return 0;
	}

static void add(gf p[4], gf q[4])
	{
	gf a,b,c,d,t,e,f,g,h;

	Z(a, p[1], p[0]);
	Z(t, q[1], q[0]);
	M(a, a, t);
	A(b, p[0], p[1]);
	A(t, q[0], q[1]);
	M(b, b, t);
	M(c, p[3], q[3]);
	M(c, c, D2);
	M(d, p[2], q[2]);
	A(d, d, d);
	Z(e, b, a);
	Z(f, d, c);
	A(g, d, c);
	A(h, b, a);

	M(p[0], e, f);
	M(p[1], h, g);
	M(p[2], g, f);
	M(p[3], e, h);
	}

static void cswap(gf p[4], gf q[4], u8 b)
	{
	int i;
	for (i = 0; i < 4; ++i)
		sel25519(p[i],q[i],b);
	}

static void pack(u8 *r, gf p[4])
	{
	gf tx, ty, zi;
	inv25519(zi, p[2]);
	M(tx, p[0], zi);
	M(ty, p[1], zi);
	pack25519(r, ty);
	r[31] ^= par25519(tx) << 7;
	}

static void scalarmult(gf p[4], gf q[4], const u8 *s)
	{
	int i;
	set25519(p[0],gf0);
	set25519(p[1],gf1);
	set25519(p[2],gf1);
	set25519(p[3],gf0);
	for (i = 255; i >= 0; --i)
		{
		u8 b = (s[i/8]>>(i&7))&1;
		cswap(p,q,b);
		add(q,p);
		add(p,p);
		cswap(p,q,b);
		}
	}

static void scalarbase(gf p[4], const u8 *s)
	{
	gf q[4];
	set25519(q[0],X);
	set25519(q[1],Y);
	set25519(q[2],gf1);
	M(q[3],X,Y);
	scalarmult(p,q,s);
	}

void nacl_sign_public(u8 *pk, const u8 *sk)
	{
	u8 d[64];
	gf p[4];

	sha512(d, sk, 32);
	d[0] &= 248;
	d[31] &= 127;
	d[31] |= 64;

	scalarbase(p,d);
	pack(pk,p);
	}

static const u64 L[32] =
	{
	0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
	0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0x10
	};

static void modL(u8 *r, i64 x[64])
	{
	i64 carry;
	int i,j;

	for (i = 63; i >= 32; --i)
		{
		carry = 0;
		for (j = i - 32; j < i - 12; ++j)
			{
			x[j] += carry - 16 * x[i] * L[j - (i - 32)];
			carry = (x[j] + 128) >> 8;
			x[j] -= carry << 8;
			}
		x[j] += carry;
		x[i] = 0;
		}

	carry = 0;
	for (j = 0; j < 32; ++j)
		{
		x[j] += carry - (x[31] >> 4) * L[j];
		carry = x[j] >> 8;
		x[j] &= 255;
		}

	for (j = 0; j < 32; ++j)
		x[j] -= carry * L[j];

	for (i = 0; i < 32; ++i)
		{
		x[i+1] += x[i] >> 8;
		r[i] = x[i] & 255;
		}
	}

static void reduce(u8 *r)
	{
	i64 x[64];
	int i;

	for (i = 0; i < 64; ++i)
		x[i] = (u64) r[i];
	for (i = 0; i < 64; ++i)
		r[i] = 0;
	modL(r,x);
	}

void nacl_sign_seal(u8 *sm, const u8 *m, u64 n, const u8 *pk, const u8 *sk)
	{
	u8 d[64],h[64],r[64];
	u64 i;
	i64 x[64];
	gf p[4];

	sha512(d, sk, 32);

	d[0] &= 248;
	d[31] &= 127;
	d[31] |= 64;

	for (i = 0; i < n; ++i)
		sm[64 + i] = m[i];
	for (i = 0; i < 32; ++i)
		sm[32 + i] = d[32 + i];

	sha512(r, sm+32, n+32);
	reduce(r);
	scalarbase(p,r);
	pack(sm,p);

	for (i = 0; i < 32; ++i)
		sm[i+32] = pk[i];

	sha512(h,sm,n + 64);
	reduce(h);

	for (i = 0; i < 64; ++i)
		x[i] = 0;
	for (i = 0; i < 32; ++i)
		x[i] = (u64) r[i];

	for (i = 0; i < 32; ++i)
		{
		int j;
		for (j = 0; j < 32; ++j)
			x[i+j] += h[i] * (u64) d[j];
		}

	modL(sm + 32,x);
	}

static int unpackneg(gf r[4], const u8 p[32])
	{
	gf t, chk, num, den, den2, den4, den6;

	set25519(r[2],gf1);
	unpack25519(r[1],p);
	S(num,r[1]);
	M(den,num,D);
	Z(num,num,r[2]);
	A(den,r[2],den);

	S(den2,den);
	S(den4,den2);
	M(den6,den4,den2);
	M(t,den6,num);
	M(t,t,den);

	pow2523(t,t);
	M(t,t,num);
	M(t,t,den);
	M(t,t,den);
	M(r[0],t,den);

	S(chk,r[0]);
	M(chk,chk,den);
	if (neq25519(chk, num))
		M(r[0],r[0],I);

	S(chk,r[0]);
	M(chk,chk,den);
	if (neq25519(chk, num))
		return -1;

	if (par25519(r[0]) == (p[31]>>7))
		Z(r[0],gf0,r[0]);

	M(r[3],r[0],r[1]);
	return 0;
	}

int nacl_sign_open(u8 *m, const u8 *sm, u64 n, const u8 *pk)
	{
	u64 i;
	u8 t[32],h[64];
	gf p[4],q[4];

	if (n < 64) return -1;
	if (unpackneg(q,pk)) return -1;

	for (i = 0; i < n; ++i)
		m[i] = sm[i];
	for (i = 0; i < 32; ++i)
		m[i+32] = pk[i];

	sha512(h,m,n);
	reduce(h);
	scalarmult(p,q,h);

	scalarbase(q,sm + 32);
	add(p,q);
	pack(t,p);

	if (verify_32(sm, t))
		return -1;

	return 0;
	}
