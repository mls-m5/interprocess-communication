#pragma once

#include <iostream>
#include <string>
#include <vector>

struct Settings {
    bool isServer = true;
    bool isClient = true;
    std::string address;
    uint32_t port = 1234;

    static constexpr auto helpstr = R"_(
usage:
  -h          print this text
  --client    only start client part
  --server    only start server part
  --addr      remote address (standard: localhost)
  --port      port           (standard: 1234)
)_";

    Settings(int argc, char **argv) {
        auto args = std::vector<std::string>(argv + 1, argv + argc);

        for (size_t i = 0; i < args.size(); ++i) {
            auto arg = args.at(i);

            if (arg == "-h" || arg == "--help") {
                std::cout << helpstr << std::endl;
                std::exit(0);
            }
            else if (arg == "--client") {
                isClient = true;
                isServer = false;
            }
            else if (arg == "--server") {
                isServer = true;
                isClient = false;
            }
            else if (arg == "--addr") {
                address = args.at(++i);
            }
            else if (arg == "--port") {
                port = std::stoi(args.at(++i));
            }
        }
    }
};
