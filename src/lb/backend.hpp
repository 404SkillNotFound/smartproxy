/*
This represents one backend server.
Backend (example)
├── host = 127.0.0.1
├── port = 5001
└── name = backend1
*/

#pragma once

#include <atomic>
#include <string>
#include <utility>

struct Backend
{
    std::string host;
    std::string port;
    std::string name;

    std::atomic<int> active_connections{0};
    bool healthy{true};

    /*
    These constructors handle how Backend objects are created and moved.
    The three-argument constructor allows a backend to be initialized with
    its host, port, and name. The move constructor is needed because
    Backend contains an atomic value, which cannot be moved automatically.
    It tells C++ how to move the strings and preserve the current
    active connection count and health status when a Backend is moved.
    */

    Backend() = default;

    Backend(std::string h, std::string p, std::string n)
        : host(std::move(h)), port(std::move(p)), name(std::move(n))
    {}

    Backend(Backend&& other)
        : host(std::move(other.host))
        , port(std::move(other.port))
        , name(std::move(other.name))
        , active_connections(other.active_connections.load())
        , healthy(other.healthy)
    {}
};