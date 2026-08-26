/*
This represents one backend server.
Backend (example)
├── host = 127.0.0.1
├── port = 5001
└── name = backend1
*/

#pragma once

#include <string>

struct Backend
{
    std::string host;
    std::string port;
    std::string name;
};