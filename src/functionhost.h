#pragma once

#include "inarchive.h"
#include <functional>
#include <unordered_map>

struct FunctionHost {
    // Should probably not be a global variable in the future
    std::unordered_map<std::string, std::function<void(InArchive &)>> funcMap;

    /// Register host std::function
    template <typename R, typename... Args>
    void registerFunction(std::string name, std::function<R(Args...)> f) {
        //        std::cout << "register function";
        //        ((std::cout << ',' << typeDescription<Args>()), ...) <<
        //        std::endl;

        auto callback = [f](InArchive &archive) {
            auto argsTuple = archive.unpack<Args...>();
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
        auto name = archive.read<std::string>();
        if (auto f = funcMap.find(name); f != funcMap.end()) {
            f->second(archive);
        }
        else {
            throw std::runtime_error{"function " + name +
                                     " from archive was not found"};
        }
    }
};
