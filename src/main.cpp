
#include "functionclient.h"
#include "functionhost.h"
#include "settings.h"
#include <iostream>

// This is what will actually be called
int testFunction(float x, std::string str) {
    std::cout << "in testFunction:"
              << "\n  str = ";
    std::cout << str << std::endl;
    return static_cast<int>(x);
}

int main(int argc, char *argv[]) {
    auto settings = Settings{argc, argv};

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
