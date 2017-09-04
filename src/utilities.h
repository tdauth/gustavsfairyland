#ifndef UTILITIES_H
#define UTILITIES_H

#include <random>

/**
 * Returns a random number between \p min and \p max (inclusive bounds).
 */
int randomNumber(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(min, max);

	return distr(eng);
}

#endif