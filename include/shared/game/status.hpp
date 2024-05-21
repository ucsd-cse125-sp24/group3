#pragma once

#include <string>

enum class Status {
    Slimed,
    Frozen
};

std::string getStatusString(Status status, int ticks_remaining);