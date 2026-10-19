#pragma once


class Battery {
public:
    void begin();
    float percentage();
private:
    static constexpr float MAX_VOLTAGE = 4.2;
    static constexpr float MIN_VOLTAGE = 3.0;
};
