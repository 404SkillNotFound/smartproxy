/*
                 Balancer -> has two jobs == addBackend() & selectBackend()
                    │
          ┌─────────┼─────────┐
          │         │         │
          ▼         ▼         ▼
       Backend    Backend    Backend
         5001       5002       5003
*/

#pragma once

#include <atomic>
#include <algorithm>
#include <vector>
#include "backend.hpp"

class Balancer
{
    std::vector<Backend> backends;
    std::atomic<size_t> counter{0};

public:
    void addBackend(const Backend& b);
    Backend& selectBackend();
    Backend& selectLeastConn();
};