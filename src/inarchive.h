#pragma once

#include <sstream>
#include <string>
#include <tuple>

template <typename Type>
std::string typeDescription() {
    return typeid(Type).name();
}

// Specialiced type, to avoid more complicated names
template <>
std::string typeDescription<std::string>() {
    return "string";
}

class InArchive {
    std::stringstream ss;

    // This could be implemented differently. raw TCP would differ from url etc
    template <typename T>
    void unpackSingleArg(T &value) {
        ss >> value;
    }

public:
    InArchive(const std::string &value)
        : ss{value} {}

    template <typename... Args>
    std::tuple<Args...> unpack() {
        auto ret = std::tuple<Args...>{};

        std::apply([this](auto &...x) { ((this->unpackSingleArg(x)), ...); },
                   ret);

        return ret;
    }

    template <typename T>
    T read() {
        auto ret = T{};
        unpackSingleArg(ret);
        return ret;
    }

    template <typename T>
    void writeBinary(const T &data) {
        ss.write(data.data(), data.size());
    }
};
