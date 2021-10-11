#include "Hash.hpp"

namespace hash
{
	void		init(SHA256 *sha)
	{
		sha->datalen = 0;
		sha->hash[0] = 0x6a09e667;
		sha->hash[1] = 0xbb67ae85;
		sha->hash[2] = 0x3c6ef372;
		sha->hash[3] = 0xa54ff53a;
		sha->hash[4] = 0x510e527f;
		sha->hash[5] = 0x9b05688c;
		sha->hash[6] = 0x1f83d9ab;
		sha->hash[7] = 0x5be0cd19;
	}

	uint		rightRotate(uint n, size_t count)
	{
		return (n >> count) | (n << (32 - count));
	}

	void		transform(SHA256 *sha)
	{
		uint	w[64];
		for (size_t i = 0; i < 16; ++i)
		{
			size_t	j = i * 4;
			w[i] = sha->data[j] << 24;
			w[i] |= sha->data[j + 1] << 16;
			w[i] |= sha->data[j + 2] << 8;
			w[i] |= sha->data[j + 3];
		}
		for (size_t i = 16; i < 64; ++i)
		{
			uint	s0, s1;
			s0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^ (w[i - 15] >> 3);
			s1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^ (w[i - 2] >> 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		uint	a, b, c, d, e, f, g, h;
		a = sha->hash[0];
		b = sha->hash[1];
		c = sha->hash[2];
		d = sha->hash[3];
		e = sha->hash[4];
		f = sha->hash[5];
		g = sha->hash[6];
		h = sha->hash[7];
		for (size_t i = 0; i < 64; ++i)
		{
			uint	s0, s1, ch, temp1, maj, temp2;
			s1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
			ch = (e & f) ^ ((~e) & g);
			temp1 = h + s1 + ch + k[i] + w[i];
			s0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
			maj = (a & b) ^ (a & c) ^ (b & c);
			temp2 = s0 + maj;
			h = g;
			g = f;
			f = e;
			e = d + temp1;
			d = c;
			c = b;
			b = a;
			a = temp1 + temp2;
		}
		sha->hash[0] += a;
		sha->hash[1] += b;
		sha->hash[2] += c;
		sha->hash[3] += d;
		sha->hash[4] += e;
		sha->hash[5] += f;
		sha->hash[6] += g;
		sha->hash[7] += h;
	}

	void		chunkLoop(SHA256 *sha, const unsigned char data[], size_t len)
	{
		for (size_t i = 0; i < len; ++i)
		{
			sha->data[sha->datalen] = data[i];
			++sha->datalen;
			if (sha->datalen == 64)
			{
				transform(sha);
				sha->datalen = 0;
			}
		}
	}

	void		final(SHA256 *sha, size_t len)
	{
		sha->data[sha->datalen] = 0b10000000;
		++sha->datalen;
		if (sha->datalen >= 56)
		{
			while (sha->datalen < 64)
			{
				sha->data[sha->datalen] = 0;
				++sha->datalen;
			}
			transform(sha);
			sha->datalen = 0;
		}
		while (sha->datalen < 56)
		{
			sha->data[sha->datalen] = 0;
			++sha->datalen;
		}
		len *= 8;
		for (size_t i = 63; i > 55; --i)
		{
			sha->data[i] = len & 0xff;
			len = len >> 8;
		}
		transform(sha);
	}
	
	std::string	toString(SHA256 *sha)
	{
		std::stringstream	ss;
		for (size_t i = 0; i < 8; i++)
			ss << std::setfill('0') << std::setw(8) << std::hex << sha->hash[i];
		return (ss.str());
	}

	std::string	hash(const std::string &str)
	{
		SHA256	sha;
		init(&sha);
		chunkLoop(&sha, reinterpret_cast<const unsigned char*>(str.c_str()), str.size());
		final(&sha, str.size());
		return (toString(&sha));
	}
}
