#include "balancer.hpp"

void Balancer::addBackend(Backend b)
{
    backends.push_back(b);
}

Backend Balancer::selectBackend()
{
    size_t index = counter.fetch_add(1) % backends.size();
    return backends[index];
}