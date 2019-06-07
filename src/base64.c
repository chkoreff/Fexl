#include <stdint.h>

#include <types.h>

#include <base64.h>

/* Reference: https://en.wikipedia.org/wiki/Base64 */
/* Reference: https://tools.ietf.org/html/rfc4648#section-10 */

/* Unpack a 6-bit sextet (0-63) to a printable base64 character. */
static const char *unpack_ch =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Pack a printable base64 character to a 6-bit sextet (0-63). */
static u8 pack_ch(u8 ch)
	{
	if (ch >= 'A' && ch <= 'Z') return ch - 'A';
	if (ch >= 'a' && ch <='z') return ch - 'a' + 26;
	if (ch >= '0' && ch <='9') return ch - '0' + 52;
	if (ch == '+') return 62;
	if (ch == '/') return 63;
	return 0;
	}

/* Unpack n bytes of raw data to base 64 characters.  The "in" array holds the
raw data.  The "out" array holds the NUL-terminated result, and its size in
bytes must be at least:  4*(n/3) + (n%3 == 0 ? 0 : 4) + 1.
*/
void unpack64(u8 *out, const u8 *in, u64 n)
	{
	const u64 n_group = n / 3;
	const u8 n_remain = n % 3;
	u64 i;

	for (i = 0; i < n_group; i++)
		{
		u8 c0 = in[3*i+0];
		u8 c1 = in[3*i+1];
		u8 c2 = in[3*i+2];
		out[4*i+0] = unpack_ch[c0 >> 2];
		out[4*i+1] = unpack_ch[((c0 & 3) << 4) | (c1 >> 4)];
		out[4*i+2] = unpack_ch[((c1 & 15) << 2) | (c2 >> 6)];
		out[4*i+3] = unpack_ch[c2 & 63];
		}

	if (n_remain == 0)
		out[4*i+0] = 0;
	else
		{
		u8 c0 = in[3*i+0];
		out[4*i+0] = unpack_ch[c0 >> 2];
		if (n_remain == 1)
			{
			out[4*i+1] = unpack_ch[(c0 & 3) << 4];
			out[4*i+2] = '=';
			}
		else
			{
			u8 c1 = in[3*i+1];
			out[4*i+1] = unpack_ch[((c0 & 3) << 4) | (c1 >> 4)];
			out[4*i+2] = unpack_ch[(c1 & 15) << 2];
			}
		out[4*i+3] = '=';
		out[4*i+4] = 0;
		}
	}

/* Pack n bytes of base 64 characters to raw data.  The "in" array holds the
base 64 characters.  The "out" array holds the NUL-terminated result, and its
size in bytes must be at least:  3*(n/4) + (n%4 == 0 ? 0 : n%4-1) + 1.

Note that n does NOT include any trailing padding characters ('=').  The caller
should decrement n as needed to ignore padding.
*/
void pack64(u8 *out, const u8 *in, u64 n)
	{
	const u64 n_group = n / 4;
	const u8 n_remain = n % 4;
	u64 i;

	for (i = 0; i < n_group; i++)
		{
		u8 c0 = pack_ch(in[4*i+0]);
		u8 c1 = pack_ch(in[4*i+1]);
		u8 c2 = pack_ch(in[4*i+2]);
		u8 c3 = pack_ch(in[4*i+3]);
		out[3*i+0] = (c0 << 2) | (c1 >> 4);
		out[3*i+1] = ((c1 & 15) << 4) | (c2 >> 2);
		out[3*i+2] = ((c2 & 3) << 6) | (c3 & 63);
		}

	if (n_remain <= 1)
		out[3*i+0] = 0;
	else
		{
		u8 c0 = pack_ch(in[4*i+0]);
		u8 c1 = pack_ch(in[4*i+1]);
		out[3*i+0] = (c0 << 2) | (c1 >> 4);
		if (n_remain == 2)
			out[3*i+1] = 0;
		else
			{
			u8 c2 = pack_ch(in[4*i+2]);
			out[3*i+1] = ((c1 & 15) << 4) | (c2 >> 2);
			out[3*i+2] = 0;
			}
		}
	}
