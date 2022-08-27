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

    // This could be implemented differently. raw TCP would differ from url etc
    template <typename T>
    void unpackSingleArg(T &value) {
        ss >> value;
    }

public:
    std::stringstream ss;

    template <typename T>
    InArchive(const T &value)
        : ss{} {
        ss.write(value.data(), value.size());
    }

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
