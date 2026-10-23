#pragma once
#include <stdint.h>
#define INPUT 0
#define A0 14
#define A5 19
#define ADC_11db 3
extern int fakeValue, readCalls, setBits, attenuationPin, lastInputPin;
extern int values[40], valuesCount, valuesIndex;
inline void pinMode(uint8_t pin, int) { lastInputPin=pin; }
inline int analogRead(uint8_t) { ++readCalls; return valuesIndex<valuesCount?values[valuesIndex++]:fakeValue; }
inline void analogReadResolution(int bits) { setBits=bits; }
inline void analogSetPinAttenuation(uint8_t pin,int) { attenuationPin=pin; }
