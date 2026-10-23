#pragma once
#include <stdint.h>
#define HIGH 1
#define LOW 0
#define INPUT_PULLUP 2
#define CHANGE 3
extern uint8_t fakePins[256];
extern uint32_t fakeMillis;
extern void (*fakeInterrupts[2])();
inline int digitalPinToInterrupt(uint8_t pin) { return pin == 2 ? 0 : pin == 3 ? 1 : -1; }
inline void pinMode(uint8_t, uint8_t) {}
inline int digitalRead(uint8_t pin) { return fakePins[pin]; }
inline uint32_t millis() { return fakeMillis; }
inline void noInterrupts() {}
inline void interrupts() {}
inline void attachInterrupt(int pin, void (*fn)(), int) { fakeInterrupts[pin] = fn; }
inline void detachInterrupt(int pin) { fakeInterrupts[pin] = 0; }
