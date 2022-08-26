
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

    // Server side
    auto host = FunctionHost{};

    auto udpServer = UdpServer{settings.port, [&host](std::string data) {
                                   std::cout << "received data: " << data
                                             << std::endl;

                                   auto arch = InArchive{data};
                                   host.handle(arch);
                               }};

    // Client side
    auto udpClient = UdpClient{settings.address, settings.port};

    auto client = FunctionClient{[&udpClient](OutArchive &arch) {
        // This is where network code would go in
        auto inArch = InArchive{arch.ss.str()};
        udpClient.send(inArch.ss.str());
    }};

    // Register function on host and client
    // This is what needs to be done for every functino on the server and
    // client
    host.registerFunction("testFunction", testFunction);

    auto testFunctionRemoteHandle =
        client.registerFunction<decltype(testFunction)>("testFunction");

    // Test call function
    testFunctionRemoteHandle(3, "hello there");

    host.registerFunction("exit", std::exit);
    auto exitHandle = client.registerFunction<decltype(exit)>("exit");

    exitHandle(0); // Just testing to bind to exit to stop server

    udpServer.start();

    return 0;
}
