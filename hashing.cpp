#include"hashing.h"
typedef struct tagInternalState
{
	uint64_t key;
	uint64_t seed;
}_InternalState;
thread_local _InternalState _internal_state;
void _set_state(uint32_t key, uint32_t seed)
{
	_internal_state.key = key;
	_internal_state.seed = seed + 1;
}
void _advance_state()
{
	_internal_state.seed = _internal_state.seed * 48271 % (((uint64_t)1 << 31) - 1);
}
uint32_t _hash_state()
{
	return mmh3(_internal_state.key, _internal_state.seed);
}
/*
Generate a random variable following the Uniform distribution in floating-point format.
*/
double uniform_hash(uint32_t key, uint32_t seed)
{
	VSLStreamStatePtr random_stream;
	_set_state(key, seed);
	double buf = (double)_hash_state() / UINT32_MAX;
	vsldNewAbstractStream(&random_stream, 1, &buf, 0., 1.,
		[](VSLStreamStatePtr stream, int* n, double dbuf[], int* nmin, int* nmax, int* idx)
		{
			_advance_state();
			*dbuf = (double)_hash_state() / UINT32_MAX;
			return 1;
		});
	double var;
	vdRngUniform(VSL_RNG_METHOD_UNIFORM_STD, random_stream, 1, &var, 0., 1.);
	vslDeleteStream(&random_stream);
	return var;
}
/*
Generate a random variable following the Exponential distribution in floating-point format.
*/
double exponential_hash(uint32_t key, uint32_t seed)
{
	VSLStreamStatePtr random_stream;
	_set_state(key, seed);
	double buf = (double)_hash_state() / UINT32_MAX;
	vsldNewAbstractStream(&random_stream, 1, &buf, 0., 1.,
		[](VSLStreamStatePtr stream, int* n, double dbuf[], int* nmin, int* nmax, int* idx)
		{
			_advance_state();
			*dbuf = (double)_hash_state() / UINT32_MAX;
			return 1;
		});
	double var;
	vdRngExponential(VSL_RNG_METHOD_EXPONENTIAL_ICDF, random_stream, 1, &var, 0., 1.);
	vslDeleteStream(&random_stream);
	return var;
}

/*
Generate a random variable following the Gamma(2,1) distribution in floating-point format.
*/
double gamma21_hash(uint32_t key, uint32_t seed)
{
	VSLStreamStatePtr random_stream;
	_set_state(key, seed);
	double buf = (double)_hash_state() / UINT32_MAX;
	vsldNewAbstractStream(&random_stream, 1, &buf, 0., 1.,
		[](VSLStreamStatePtr stream, int* n, double dbuf[], int* nmin, int* nmax, int* idx)
		{
			_advance_state();
			*dbuf = (double)_hash_state() / UINT32_MAX;
			return 1;
		});
	double var;
	vdRngGamma(VSL_RNG_METHOD_GAMMA_GNORM, random_stream, 1, &var, 2., 0., 1.);
	vslDeleteStream(&random_stream);
	return var;
}

/*
Generate a random variable following the Uniform distribution in int format.
*/
int uniformint_hash(uint32_t key, uint32_t seed, int low, int high)
{
	VSLStreamStatePtr random_stream;
	_set_state(key, seed);
	double buf = (double)_hash_state() / UINT32_MAX;
	vsldNewAbstractStream(&random_stream, 1, &buf, 0., 1.,
		[](VSLStreamStatePtr stream, int* n, double dbuf[], int* nmin, int* nmax, int* idx)
		{
			_advance_state();
			*dbuf = (double)_hash_state() / UINT32_MAX;
			return 1;
		});
	int var;
	viRngUniform(VSL_RNG_METHOD_UNIFORM_STD, random_stream, 1, &var, low, high);
	vslDeleteStream(&random_stream);
	return var;
}
