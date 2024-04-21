#pragma once

#include "shared/utilities/serialize_macro.hpp"

#include <vector>
#include <unordered_set>

/**
 * @brief SmartVector is a wrapper for the std::vector class that adds objects
 * either to the end of the wrapped vector if it is full or places them in gaps.
 * @tparam T Type that the wrapped vector will store.
 */
template<typename T>
class SmartVector {
public:
	/**
	 * @brief Creates a SmartVector instance.
	 */
	SmartVector() {}

	/**
	 * @brief Pushes the given object to the end of the wrapped vector if it is
	 * currently full, or to an existing empty index.
	 * @return Index at which the given object is inserted in the wrapped
	 * vector.
	 */
	size_t push(T object) {
		//	See if there's an empty index
		size_t index;
		auto it = freelist.begin();
		if (it != freelist.end()) {
			index = *it;
			freelist.erase(index);

			wrapped_vector[index] = object;
		}
		else {
			index = wrapped_vector.size();
			wrapped_vector.push_back(object);
		}

		return index;
	}

	/**
	 * @brief Adds an empty object to the end of the wrapped vector (by adding
	 * the new index to the freelist.
	 * @return Index at which the empty object is added (last index in the
	 * wrapped vector)
	 */
	size_t pushEmpty() {
		T emptyObject;
		size_t index = wrapped_vector.size();
		freelist.insert(index);
		wrapped_vector.push_back(emptyObject);

		return index;
	}

	/**
	 * @brief Attempts to remove the given object at the given index of the
	 * wrapped vector.
	 * @return true if successfully removed the object and false otherwise.
	 */
	bool remove(size_t index) {
		//	Check that index is in bounds
		if (index < wrapped_vector.size()) {
			//	Add index to freelist
			freelist.insert(index);

			return true;
		}

		return false;
	}

	/**
	 * @brief Returns a pointer to the object stored at the given index if it
	 * exists, and returns nullptr otherwise.
	 * @return Pointer to the object stored at the given index if it exists and
	 * nullptr otherwise.
	 */
	const T* get(size_t index) const {
		//	Check that the index is in bounds
		if (index >= wrapped_vector.size())
		{
			//	Index is out of bounds
			return nullptr;
		}
		//	Check that index isn't free
		if (freelist.find(index) != freelist.end()) {
			//	Index is free
			return nullptr;
		}

		//	Return pointer to object stored at the given index
		return &wrapped_vector[index];
	}

	// https://stackoverflow.com/questions/38790352/how-can-you-return-a-non-const-reference-to-an-element-in-a-stdvector-data-mem
	T* get(size_t index) {
		//	Check that the index is in bounds
		if (index >= wrapped_vector.size())
		{
			//	Index is out of bounds
			return nullptr;
		}
		//	Check that index isn't free
		if (freelist.find(index) != freelist.end()) {
			//	Index is free
			return nullptr;
		}

		//	Return pointer to object stored at the given index
		return &wrapped_vector[index];
	}

	/**
	 * @brief Returns the number of elements allocated in the wrapped vector
	 * (including gaps)
	 * @return Number of elements in the wrapped vector including gaps.
	 */
	size_t size() const {
		return wrapped_vector.size();
	}

	/**
	 * @brief Returns the number of elements in the wrapped vector (not
	 * including gaps)
	 * @return Number of elements in the wrapped vector not including gaps.
	 */
	size_t numElements() const {
		return wrapped_vector.size() - freelist.size();
	}

	DEF_SERIALIZE(Archive& ar, const unsigned int version) {
		ar& wrapped_vector& freelist;
	}

private:
	std::vector<T> wrapped_vector;
	std::unordered_set<size_t> freelist;
};