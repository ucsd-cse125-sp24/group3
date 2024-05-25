#include "shared/utilities/time.hpp"

#include <chrono>

long long getMsSinceEpoch() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}
