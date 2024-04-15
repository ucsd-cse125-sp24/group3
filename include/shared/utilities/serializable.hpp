#pragma once

#include <string>

/**
 *  An interface for anything we wish to serialize into some format and send
 *  across the network.
 */
class Serializable {
public:
    // Maybe we want to change the return type of this to not be string?
    [[nodiscard]] virtual std::string serialize() const = 0;
};

