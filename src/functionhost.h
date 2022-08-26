#pragma once

#include "inarchive.h"
#include <functional>
#include <unordered_map>

template <typename InArchive>
struct FunctionHostT {
    std::unordered_map<std::string, std::function<void(InArchive &)>> funcMap;

    /// Register host std::function
    template <typename R, typename... Args>
    void registerFunction(std::string name, std::function<R(Args...)> f) {
        auto callback = [f](InArchive &archive) {
            auto argsTuple = archive.template unpack<Args...>();
            std::apply(f, argsTuple);
        };

        this->funcMap.insert({name, callback});
    }

    /// Register any type that can be converted to std::function
    template <typename F>
    void registerFunction(std::string name, F f) {
        registerFunction(std::move(name), std::function{f});
    }

    void handleArchive(InArchive &archive) {
        auto name = archive.template read<std::string>();
        if (auto f = funcMap.find(name); f != funcMap.end()) {
            f->second(archive);
        }
        else {
            throw std::runtime_error{"function " + name +
                                     " from archive was not found"};
        }
    }
};

using FunctionHost = FunctionHostT<InArchive>;
