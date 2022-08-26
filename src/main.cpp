#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

// This is what will actually be called
int testFunction(float x, std::string str) {
    std::cout << "in testFunction:"
              << "\n  str = ";
    std::cout << str << std::endl;
    return static_cast<int>(x);
}

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
};

struct FunctionHost {
    // Should probably not be a global variable in the future
    std::unordered_map<std::string, std::function<void(InArchive &)>> funcMap;

    /// Register host std::function
    template <typename R, typename... Args>
    void registerFunction(std::string name, std::function<R(Args...)> f) {
        std::cout << "register function";
        ((std::cout << ',' << typeDescription<Args>()), ...) << std::endl;

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

int main(int argc, char *argv[]) {
    auto host = FunctionHost{};

    host.registerFunction("testFunction", testFunction);

    // Test
    for (auto &it : host.funcMap) {
        std::cout << it.first << std::endl;
    }

    // This should be replaced with automatic handling
    auto archive = InArchive{R"(
testFunction
2.
hello)"};

    host.handleArchive(archive);

    return 0;
}
