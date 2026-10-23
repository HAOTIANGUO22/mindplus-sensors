#include "MindPlusPotentiometer.h"

MindPlusPotentiometer::MindPlusPotentiometer()
    : _pin(0), _samples(8), _low(0), _high(maximum()), _ready(false), _reverse(false) {}

uint16_t MindPlusPotentiometer::maximum() {
#if defined(ESP32) || defined(ESP_PLATFORM)
    return 4095;
#else
    return 1023;
#endif
}

bool MindPlusPotentiometer::begin(int16_t pin) {
    _ready = false;
#if defined(ESP32) || defined(ESP_PLATFORM)
    // mPython exposed ADC1 inputs: P0=33, P1=32, P2=35, P3=34.
    // Exclude onboard microphone/light inputs and ADC2/Wi-Fi conflicts.
    if (pin != 33 && pin != 32 && pin != 35 && pin != 34) return false;
#else
    // Uno: require Arduino A0..A5 pin constants, not digital pins 0..5.
    if (pin < A0 || pin > A5) return false;
#endif
    _pin = static_cast<uint8_t>(pin);
    _samples = 8;
    _reverse = false;
    resetCalibration();
    pinMode(_pin, INPUT); // No pull-up: this is an analog voltage input.
#if defined(ESP32) || defined(ESP_PLATFORM)
    analogReadResolution(12);
    analogSetPinAttenuation(_pin, ADC_11db);
#endif
    _ready = true;
    return true;
}

uint16_t MindPlusPotentiometer::raw() {
    if (!_ready) return 0;
#if defined(ESP32) || defined(ESP_PLATFORM)
    // Mind+ and other libraries can change these shared ADC settings.
    analogReadResolution(12);
    analogSetPinAttenuation(_pin, ADC_11db);
#endif
    (void)analogRead(_pin); // Discard first conversion after channel switching.
    uint32_t sum = 0;
    for (uint8_t i = 0; i < _samples; ++i) {
        int value = analogRead(_pin);
        if (value < 0) value = 0;
        if (value > maximum()) value = maximum();
        sum += static_cast<uint16_t>(value);
    }
    return static_cast<uint16_t>((sum + _samples / 2) / _samples);
}

double MindPlusPotentiometer::fraction() {
    uint16_t value = raw();
    if (value < _low) value = _low;
    if (value > _high) value = _high;
    double result = static_cast<double>(value - _low) / (_high - _low);
    return _reverse ? 1.0 - result : result;
}

uint8_t MindPlusPotentiometer::percent() {
    if (!_ready) return 0;
    return static_cast<uint8_t>(fraction() * 100.0 + 0.5);
}

double MindPlusPotentiometer::scaled(double outputMin, double outputMax) {
    if (!_ready) return outputMin;
    return outputMin + (outputMax - outputMin) * fraction();
}

bool MindPlusPotentiometer::setSamples(int16_t samples) {
    if (samples != 1 && samples != 4 && samples != 8 && samples != 16 && samples != 32) return false;
    _samples = static_cast<uint8_t>(samples);
    return true;
}

bool MindPlusPotentiometer::calibrate(int32_t low, int32_t high) {
    if (low < 0 || high > maximum() || low >= high) return false;
    _low = static_cast<uint16_t>(low);
    _high = static_cast<uint16_t>(high);
    return true;
}

void MindPlusPotentiometer::resetCalibration() {
    _low = 0;
    _high = maximum();
}
