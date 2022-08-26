#pragma once

#include <sstream>

struct OutArchive {
    std::stringstream ss;

    template <typename T>
    void packSingleArg(const T &value) {
        ss << value << "\n";
    }

    template <typename... Args>
    void pack(Args... args) {
        ((this->packSingleArg(args)), ...);
    }

    template <typename T>
    void write(const T &value) {
        packSingleArg(value);
    }
};
