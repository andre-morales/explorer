#pragma once
#include <ilib/types.h>
class PRNG {
public:
	uint64 seed;
	uint64 curseed;

	PRNG(int);
	PRNG(uint64);
	uint64 gen();

private:
	PRNG(const PRNG&);
};
