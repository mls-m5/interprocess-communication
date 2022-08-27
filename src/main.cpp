
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

int main(int argc, char *argv[]) {
    auto settings = Settings{argc, argv};

    if (settings.isClient && settings.isServer) {
        int processNum = fork();
        settings.isServer = processNum;
        settings.isClient = !processNum;
    }

    if (settings.isServer) {
        // Server side
        auto host = FunctionHost{};

        auto udpServer = UdpServer{settings.port, [&host](std::string data) {
                                       std::cout << "received data: " << data
                                                 << std::endl;

                                       auto arch = InArchive{data};
                                       host.handle(arch);
                                   }};

        // Register function on host
        // This is what needs to be done for every function on the server and
        // client, and the functions needs to match
        host.registerFunction("testFunction", testFunction);
        host.registerFunction("exit", std::exit);
        host.registerFunction("lambda", [](int x, int y) {
            std::cout << "lambda:\n";
            std::cout << "   x = " << x << ", y = " << y << std::endl;
        });
        udpServer.start();
    }
    else if (settings.isClient) {

        // Client side
        auto udpClient = UdpClient{settings.address, settings.port};

        auto client = FunctionClient{[&udpClient](OutArchive &arch) {
            // This is where network code would go in
            auto inArch = InArchive{arch.ss.str()};
            udpClient.send(inArch.ss.str());
        }};

        auto testFunctionHandle =
            client.registerFunction<decltype(testFunction)>("testFunction");
        auto exitHandle = client.registerFunction<decltype(exit)>("exit");
        auto lambdaHandle = client.registerFunction<void(int, int)>("lambda");

        // Test call function
        testFunctionHandle(3, "hello there");
        lambdaHandle(3, 4);

        exitHandle(0); // Just testing to bind to `exit()` to stop server
    }

    return 0;
}
