#pragma once
#include <functional>

class Oven
{
public:
    Oven();
    void Preheat(int target_temperature);
    int GetTemperature() const;
    void SetLoad(int load);
    int GetLoad() const;
    bool IsReady() const;
    void Bake(int seconds, std::function<void()> onDone);

private:
    int _target_temperature;
    int _temperature;
    int _load;
    bool _is_preheated;
};