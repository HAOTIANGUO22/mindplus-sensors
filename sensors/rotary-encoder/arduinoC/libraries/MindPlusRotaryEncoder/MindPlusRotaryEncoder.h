#ifndef MINDPLUS_ROTARY_ENCODER_H
#define MINDPLUS_ROTARY_ENCODER_H

#include <Arduino.h>
#if defined(ESP32) || defined(ESP_PLATFORM)
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#define MP_ENCODER_ESP32
#define MP_ENCODER_ISR IRAM_ATTR
#else
#define MP_ENCODER_ISR
#endif

// One encoder per sketch. Call methods from the Arduino loop task only.
class MindPlusRotaryEncoder {
public:
    MindPlusRotaryEncoder();
    bool begin(uint8_t pinA, uint8_t pinB, int16_t buttonPin = -1);
    void end();
    bool ready() const { return _ready; }
    int32_t read();
    void write(int32_t value);
    int32_t takeDelta(); // Net steps since the previous call; consumes the delta.
    int8_t direction(); // Last completed step: +1, -1, or 0 since reset.
    void setReverse(bool reverse);
    void setTransitions(uint8_t transitions); // 1, 2, 4; default 4.
    void update(); // Poll and debounce button; call frequently in loop.
    bool pressed();
    bool clicked(); // One true per debounced press; consumes the event.

private:
    MindPlusRotaryEncoder(const MindPlusRotaryEncoder&) = delete;
    MindPlusRotaryEncoder& operator=(const MindPlusRotaryEncoder&) = delete;
    static MindPlusRotaryEncoder* _active;
    static void MP_ENCODER_ISR onEdge();
    static uint8_t MP_ENCODER_ISR readPin(uint8_t pin);
    void MP_ENCODER_ISR sample();
    uint8_t _pinA, _pinB;
    int16_t _button;
    bool _ready, _rawPressed, _stablePressed, _click;
    uint32_t _buttonChanged;
    volatile uint8_t _previous, _transitions;
    volatile int8_t _partial, _direction;
    volatile bool _reverse;
    volatile int32_t _count, _delta;
#ifdef MP_ENCODER_ESP32
    portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;
#endif
};
#endif
