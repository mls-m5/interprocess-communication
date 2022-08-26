
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

    auto host = FunctionHost{};

    host.registerFunction("testFunction", testFunction);

    auto udpClient = UdpClient{settings.address, settings.port};
    auto udpServer = UdpServer{settings.port, [&host](std::string data) {
                                   std::cout << "received data: " << data
                                             << std::endl;

                                   auto arch = InArchive{data};
                                   host.handleArchive(arch);
                               }};

    auto client = FunctionClient{[&udpClient](OutArchive &arch) {
        // This is where network code would go in
        auto inArch = InArchive{arch.ss.str()};
        udpClient.send(inArch.ss.str());
    }};

    auto testFunctionRemoteHandle =
        client.registerFunction<decltype(testFunction)>("testFunction");

    testFunctionRemoteHandle(3, "hello there");

    udpServer.start();

    return 0;
}
