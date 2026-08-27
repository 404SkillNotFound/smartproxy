/*
This represents one backend server.
Backend (example)
├── host = 127.0.0.1
├── port = 5001
└── name = backend1
*/

#pragma once

#include <string>
#include <atomic>

struct Backend
{
    std::string host;
    std::string port;
    std::string name;
    
    std::atomic<int> active_connections{0};
};