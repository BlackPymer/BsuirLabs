#include "kitchen/oven.hpp"
#include <chrono>
#include <thread>

Oven::Oven() : _target_temperature(0), _temperature(20), _load(0), _is_preheated(false)
{
}

void Oven::Preheat(int target_temperature)
{
    _target_temperature = target_temperature;
    _temperature = target_temperature;
    _is_preheated = true;
}

int Oven::GetTemperature() const { return _temperature; }
void Oven::SetLoad(int load) { _load = load; }
int Oven::GetLoad() const { return _load; }
bool Oven::IsReady() const { return _is_preheated && _load > 0; }

void Oven::Bake(int seconds, std::function<void()> onDone)
{
    std::thread([this, seconds, onDone]()
                {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
        _load = 0;
        onDone(); })
        .detach();
}