#include <algorithm>

#include "balancer.hpp"

void Balancer::addBackend(const Backend &b)
{
    backends.emplace_back();
    backends.back().host = b.host;
    backends.back().port = b.port;
    backends.back().name = b.name;
}

Backend &Balancer::selectBackend()
{
    size_t index = counter.fetch_add(1) % backends.size();
    return backends[index];
}

Backend &Balancer::selectLeastConn()
{
    auto least_it = std::min_element(
        backends.begin(),
        backends.end(),
        [](const Backend &a, const Backend &b)
        {
            if (!a.healthy)
                return false; // a is worse
            if (!b.healthy)
                return true; // b is worse, prefer a

            // .load() gets the current integer value from the atomic counter
            return a.active_connections.load() < b.active_connections.load();
        });

    return *least_it;
}

void Balancer::startHealthChecker()
{
    std::thread healthThread([this]()
                             {
        while (true)
        {
            for (Backend& backend : backends)
            {
                backend.healthy = isBackendAlive(backend);
            }

            std::this_thread::sleep_for(std::chrono::seconds(5));
        } });

    // continue running in the background.
    healthThread.detach();
}