#pragma once
#include <boost/functional/hash.hpp>

//	Based on: https://www.boost.org/doc/libs/1_61_0/doc/html/hash/combine.html
//	https://stackoverflow.com/questions/39370214/how-to-properly-hash-a-pair-of-pointers
struct pair_hash {
	template<typename T>
	std::size_t operator()(std::pair<T, T> const& p) const {
		std::size_t seed = 0;
		boost::hash_combine(seed, p.first);
		boost::hash_combine(seed, p.second);

		return seed;
	}
};