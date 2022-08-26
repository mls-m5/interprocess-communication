
#include "functionhost.h"
#include <iostream>

// This is what will actually be called
int testFunction(float x, std::string str) {
    std::cout << "in testFunction:"
              << "\n  str = ";
    std::cout << str << std::endl;
    return static_cast<int>(x);
}

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

int main(int argc, char *argv[]) {
    auto host = FunctionHost{};

    host.registerFunction("testFunction", testFunction);

    auto client = FunctionClient{[&host](OutArchive &arch) {
        // This is where network code would go in
        auto inArch = InArchive{arch.ss.str()};
        host.handleArchive(inArch);
    }};

    auto testFunctionRemoteHandle =
        client.registerFunction<decltype(testFunction)>("testFunction");

    testFunctionRemoteHandle(3, "hello there");

    return 0;
}
