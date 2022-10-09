#include<mkl.h>
#include"mmh3.h"
double uniform_hash(uint32_t key, uint32_t seed);
double exponential_hash(uint32_t key, uint32_t seed);
int uniformint_hash(uint32_t key, uint32_t seed, int low, int high);
double gamma21_hash(uint32_t key, uint32_t seed);