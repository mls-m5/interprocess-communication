
#include "functionclient.h"
#include "functionhost.h"
#include "settings.h"
#include <boost/asio.hpp>

#include <iostream>

// This is what will actually be called
int testFunction(float x, std::string str) {
    std::cout << "in testFunction:"
              << "\n  str = ";
    std::cout << str << std::endl;
    return static_cast<int>(x);
}

struct UdpServer {
    using udp = boost::asio::ip::udp;

    boost::asio::io_service service;
    udp::socket socket;
    std::function<void(std::string)> callback;

    template <typename T>
    void write(const T &value) {}

    UdpServer(int port, std::function<void(std::string)> callback)
        : socket{service, udp::v4(), port}
        , callback{callback} {}

    void start() {
        while (true) {
            auto buffer = std::array<char, 100>{};
            auto remoteEndpoint = udp::endpoint{};
            auto error = boost::system::error_code{};

            auto size = socket.receive_from(
                boost::asio::buffer(buffer), remoteEndpoint, 0, error);

            if (error && error != boost::asio::error::message_size) {
                throw boost::system::system_error{error};
            }

            callback({buffer.data(), buffer.size()});
        }
    }
};

struct UdpClient {
    using udp = boost::asio::ip::udp;

    boost::asio::io_service service;
    udp::socket socket{service};
    udp::endpoint endpoint;

    udp::endpoint getEndpoint(std::string name, uint32_t port) {
        udp::resolver resolver{service};
        auto query =
            udp::resolver::query(udp::v4(), name, std::to_string(port));
        return *resolver.resolve(query);
    }

    UdpClient(std::string name, uint32_t port)
        : endpoint{getEndpoint(name, port)} {
        socket.open(udp::v4());
    }

    template <typename T>
    void send(const T &data) {
        socket.send_to(boost::asio::buffer(data.data(), data.size()), endpoint);
    }
};

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
