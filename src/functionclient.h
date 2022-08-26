#pragma once

#include "outarchive.h"
#include <functional>

template <typename OutArchive>
class FunctionClientT {
    std::function<void(OutArchive &arch)> callback;

    /// Register host std::function
    template <typename R, typename... Args>
    std::function<void(Args...)> registerFunction(std::string name,
                                                  R (*f)(Args...)) {
        return [this, name](Args... args) -> void {
            auto arch = OutArchive{};
            arch.write(name);
            arch.pack(args...);
            this->callback(arch);
        };
    }

public:
    FunctionClientT(std::function<void(OutArchive &arch)> f)
        : callback{f} {}

    /// Register any type that can be converted to std::function
    template <typename F>
    auto registerFunction(std::string name) {
        return registerFunction(name, (F *){});
    }
};

using FunctionClient = FunctionClientT<OutArchive>;
