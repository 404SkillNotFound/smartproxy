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
    std::vector<Backend *> eligible_backends;

    for (Backend &backend : backends)
    {
        if (backend.circuit_state == CircuitState::OPEN)
        {
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed_duration = current_time - backend.circuit_opened_at;

            if (elapsed_duration >= std::chrono::seconds(30))
            {
                backend.circuit_state = CircuitState::HALF_OPEN;
            }
        }

        if (backend.circuit_state == CircuitState::CLOSED)
        {
            eligible_backends.push_back(&backend);
        }
        else if(backend.circuit_state == CircuitState::HALF_OPEN && !backend.half_open_request_in_progress){
            backend.half_open_request_in_progress = true;
            eligible_backends.push_back(&backend);
        }
    }

    auto least_it = std::min_element(
        eligible_backends.begin(),
        eligible_backends.end(),
        [](const Backend *a, const Backend *b)
        {
            

            // .load() gets the current integer value from the atomic counter
            return a->active_connections.load() < b->active_connections.load();
        });

    return **least_it;
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