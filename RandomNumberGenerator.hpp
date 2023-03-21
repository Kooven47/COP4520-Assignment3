#ifndef RANDOM_NUMBER_GENERATOR_HPP
#define RANDOM_NUMBER_GENERATOR_HPP

#include <random>

namespace RandomNumberGenerator
{
    // Random integer generator, inclusive of min and max
    int generateRandomNumber(int min, int max)
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(min, max);

        return dist(mt);
    }
}

#endif
