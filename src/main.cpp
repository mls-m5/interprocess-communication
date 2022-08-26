
#include "functionhost.h"
#include <iostream>

// This is what will actually be called
int testFunction(float x, std::string str) {
    std::cout << "in testFunction:"
              << "\n  str = ";
    std::cout << str << std::endl;
    return static_cast<int>(x);
}

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
