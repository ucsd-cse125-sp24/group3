#pragma once

// Moreso including these right now to test cmake is building everything correctly,
// but we probably will want these eventually

/**
 * Generate a random double between given min and max values.
 * @see https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
 * @param min inclusive lower end of range for random number 
 * @param min exclusive upper end of range for random number 
 * @return random double within the specified range
*/
double randomDouble(double min, double max);

/**
 * Generate a random integer between given min and max values.
 * @see https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
 * @param min inclusive lower end of range for random number 
 * @param min inclusive upper end of range for random number 
 * @return random integer within the specified range
*/
int randomInt(int min, int max);

