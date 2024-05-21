#include "shared/game/status.hpp"

std::string getStatusString(Status status, int ticks_remaining) {
    std::string str;

    switch (status) {
        case Status::Slimed:
            str = "Slimed!";
            break;
        case Status::Frozen:
            str = "Frozen!";
            break;
    }

    // TODO also display information about the time remaining

    return str;
}