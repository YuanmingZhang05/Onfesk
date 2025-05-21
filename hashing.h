#include<mkl.h>
#include"mmh3.h"
double uniform_hash(uint32_t key, uint32_t seed); // generate a uniform random variable
double exponential_hash(uint32_t key, uint32_t seed); // generate an exponential random variable
int uniformint_hash(uint32_t key, uint32_t seed, int low, int high); // uniformly select a int variable from (int low, int high)
double gamma21_hash(uint32_t key, uint32_t seed); // generate a Gamma random variable
