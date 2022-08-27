
#include "functionclient.h"
#include "functionhost.h"
#include "settings.h"
#include "udp.h"
#include <iostream>

// This is what will actually be called
int testFunction(float x, std::string str) {
    std::cout << "in testFunction:"
              << "\n  str = ";
    std::cout << str << std::endl;
    return static_cast<int>(x);
}

void runServer(const Settings &settings) {
    // Server side
    auto host = FunctionHost{};
    auto udpServer = UdpServer{settings.port, &host};

    // Register function on host
    // This is what needs to be done for every function on the server and
    // client, and the functions needs to match
    host.registerFunction("testFunction", testFunction);
    host.registerFunction("exit", std::exit);
    host.registerFunction("lambda", [](int x, int y) {
        std::cout << "lambda:\n";
        std::cout << "  x = " << x << ", y = " << y << std::endl;
    });
    udpServer.start();
}

void runClient(const Settings &settings) {

    // Client side
    auto udpClient = UdpClient{settings.address, settings.port};
    auto client = FunctionClient{&udpClient};

    // Register functions. Done at startup
    auto testFunctionHandle =
        client.registerFunction<decltype(testFunction)>("testFunction");
    auto exitHandle = client.registerFunction<decltype(exit)>("exit");
    auto lambdaHandle = client.registerFunction<void(int, int)>("lambda");

    // Test call function
    testFunctionHandle(3, "hello there");
    lambdaHandle(3, 4);
    exitHandle(0); // Just testing to bind to `exit()` to stop server
}

int main(int argc, char *argv[]) {
    auto settings = Settings{argc, argv};

    if (settings.isClient && settings.isServer) {
        int processNum = fork();
        settings.isServer = processNum;
        settings.isClient = !processNum;
    }

    if (settings.isServer) {
        runServer(settings);
    }
    else if (settings.isClient) {
        runClient(settings);
    }

    return 0;
}
