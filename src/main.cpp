#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

// This is what will actually be called
int testFunction(float x, std::string str) {
    std::cout << str;
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
};

// Should probably not be a global variable in the future
auto funcMap =
    std::unordered_map<std::string, std::function<void(InArchive &)>>{};

// Register host std::function
template <typename R, typename... Args>
void registerFunction(std::string name, std::function<R(Args...)> f) {
    std::cout << "register function";
    ((std::cout << ',' << typeDescription<Args>()), ...) << std::endl;

    auto callback = [f](InArchive &archive) {
        auto argsTuple = archive.unpack<Args...>();
        std::apply(f, argsTuple);
    };

    funcMap.insert({name, callback});
}

// Register any type that can be converted to std::function
template <typename F>
void registerFunction(std::string name, F f) {
    registerFunction(std::move(name), std::function{f});
}

int main(int argc, char *argv[]) {
    registerFunction("testFunction", testFunction);

    // Test
    for (auto &it : funcMap) {
        std::cout << it.first << std::endl;
    }

    // This should be replaced with automatic handling
    auto archive = InArchive{"2.\nhello"};

    funcMap["testFunction"](archive);

    return 0;
}
