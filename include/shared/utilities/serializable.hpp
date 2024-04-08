#ifndef SHARED_UTILITIES_SERIALIZABLE_HPP_
#define SHARED_UTILITIES_SERIALIZABLE_HPP_

#include <string>

/**
 *  An interface for anything we wish to serialize into some format and send
 *  across the network.
 */
class Serializable {
public:
    // Maybe we want to change the return type of this to not be string?
    virtual std::string serialize() const = 0;
};

#endif  // SHARED_UTILITIES_SERIALIZABLE_HPP_
