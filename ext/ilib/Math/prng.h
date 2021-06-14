#pragma once
#include "ilib/types.h"

struct PRNG {
	uint64 seed;
	uint64 curseed;

	PRNG(int);
	PRNG(uint64);
	uint64 gen();

private:
	PRNG(const PRNG&);
};
