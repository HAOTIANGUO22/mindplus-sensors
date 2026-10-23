#ifndef MINDPLUS_POTENTIOMETER_H
#define MINDPLUS_POTENTIOMETER_H
#include <Arduino.h>

// Use from the Arduino main task. No interrupts, timers or heap allocations.
class MindPlusPotentiometer {
public:
    MindPlusPotentiometer();
    bool begin(int16_t pin);
    bool ready() const { return _ready; }
    uint16_t raw();
    uint8_t percent();
    double scaled(double outputMin, double outputMax);
    static uint16_t maximum();
    bool setSamples(int16_t samples);
    bool calibrate(int32_t low, int32_t high);
    void resetCalibration();
    void setReverse(bool reverse) { _reverse = reverse; }
private:
    double fraction();
    uint8_t _pin, _samples;
    uint16_t _low, _high;
    bool _ready, _reverse;
};
#endif
