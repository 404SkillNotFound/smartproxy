#include "parser.hpp"

// Parse the HTTP request line
//
//      GET        /        HTTP/1.1
//       │         │            │
//       ▼         ▼            ▼
//    method     target      version
//
// Extract these three components from the raw request and
// store them in the HttpRequest structure.

HttpRequest parseRequest(const std::string& request)
{
    size_t lineEnd = request.find("\r\n");
    std::string requestLine = request.substr(0, lineEnd);

    size_t firstSpace = requestLine.find(' ');
    size_t secondSpace = requestLine.find(' ', firstSpace + 1);

    HttpRequest parsedRequest;

    parsedRequest.method = requestLine.substr(0, firstSpace);

    parsedRequest.target = requestLine.substr(
        firstSpace + 1,
        secondSpace - firstSpace - 1
    );

    parsedRequest.version = requestLine.substr(secondSpace + 1);

    return parsedRequest;
}