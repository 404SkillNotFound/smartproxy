#pragma once

#include <string>

struct HttpRequest
{
    std::string method;
    std::string target;
    std::string version;
};

HttpRequest parseRequest(const std::string& request);