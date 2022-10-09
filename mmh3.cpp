#include"mmh3.h"
uint32_t rol(uint32_t x, uint32_t n)
{
	return (x << n) | (x >> (32 - n));
}
uint32_t mmh3(uint32_t key, int seed)
{
	uint32_t c1 = 0xcc9e2d51, c2 = 0x1b873593, r1 = 15, r2 = 13, m = 5, n = 0xe6546b64, hash = seed, k = key;
	k *= c1;
	k = rol(k, r1);
	k *= c2;
	hash ^= k;
	hash = rol(hash, r2);
	hash = hash * m + n;
	hash ^= 4;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);
	return hash;
}