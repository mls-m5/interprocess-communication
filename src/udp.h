#pragma once

#include <boost/asio.hpp>

struct UdpServer {
    using udp = boost::asio::ip::udp;

    boost::asio::io_service service;
    udp::socket socket;
    std::function<void(std::string)> callback;

    template <typename T>
    void write(const T &value) {}

    UdpServer(uint32_t port, std::function<void(std::string)> callback)
        : socket{service,
                 udp::endpoint{udp::v4(), static_cast<unsigned short>(port)}}
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

            callback({buffer.data(), size});
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
