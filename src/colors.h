#pragma once

#include <iostream>
#include <string>

enum Color {
    RESET = 0,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34
};

std::ostream& operator<<(std::ostream& os, const Color color) {
    os << "\e[" << std::to_string(color) << "m";
    return os;
}
