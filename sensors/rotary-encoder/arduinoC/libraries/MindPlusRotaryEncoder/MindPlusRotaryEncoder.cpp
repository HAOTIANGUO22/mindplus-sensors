#include "MindPlusRotaryEncoder.h"
#include <stdint.h>
#ifdef MP_ENCODER_ESP32
#include <soc/gpio_struct.h>
#define MP_LOCK portENTER_CRITICAL(&_mux)
#define MP_UNLOCK portEXIT_CRITICAL(&_mux)
#elif defined(__AVR__)
#include <avr/interrupt.h>
#define MP_LOCK uint8_t savedSreg = SREG; cli()
#define MP_UNLOCK SREG = savedSreg
#else
// Host-test backend. Production targets are AVR Uno and classic ESP32.
#define MP_LOCK noInterrupts()
#define MP_UNLOCK interrupts()
#endif

MindPlusRotaryEncoder* MindPlusRotaryEncoder::_active = 0;

MindPlusRotaryEncoder::MindPlusRotaryEncoder()
    : _pinA(0), _pinB(0), _button(-1), _ready(false), _rawPressed(false),
      _stablePressed(false), _click(false), _buttonChanged(0), _previous(0),
      _transitions(4), _partial(0), _direction(0), _reverse(false), _count(0), _delta(0) {}

bool MindPlusRotaryEncoder::begin(uint8_t a, uint8_t b, int16_t button) {
    end();
    if (_active && _active != this) return false;
    if (a == b || button == a || button == b || button < -1) return false;
#ifdef MP_ENCODER_ESP32
    // Only exposed, internally pull-up capable mPython pins; excludes flash pins,
    // input-only pins, and built-in I2C/display pins.
    const uint8_t allowed[] = {33, 32, 16, 17, 26, 25, 18, 19, 21, 5};
    bool okA = false, okB = false, okButton = button == -1;
    for (uint8_t i = 0; i < sizeof(allowed); ++i) {
        if (a == allowed[i]) okA = true;
        if (b == allowed[i]) okB = true;
        if (button == allowed[i]) okButton = true;
    }
    if (!okA || !okB || !okButton) return false;
#else
    if (!((a == 2 && b == 3) || (a == 3 && b == 2))) return false;
    if (button != -1 && (button < 4 || button > 19)) return false;
#endif
    if (digitalPinToInterrupt(a) < 0 || digitalPinToInterrupt(b) < 0) return false;
    _pinA = a; _pinB = b; _button = button;
    pinMode(a, INPUT_PULLUP); pinMode(b, INPUT_PULLUP);
    if (button >= 0) pinMode(button, INPUT_PULLUP);
    _previous = (readPin(a) << 1) | readPin(b);
    _count = 0; _delta = 0; _partial = 0; _direction = 0;
    _transitions = 4; _reverse = false;
    _rawPressed = button >= 0 && readPin(button) == LOW;
    _stablePressed = false; _click = false; _buttonChanged = millis();
    _active = this; _ready = true;
    attachInterrupt(digitalPinToInterrupt(a), onEdge, CHANGE);
    attachInterrupt(digitalPinToInterrupt(b), onEdge, CHANGE);
    return true;
}

void MindPlusRotaryEncoder::end() {
    if (_ready) {
        detachInterrupt(digitalPinToInterrupt(_pinA));
        detachInterrupt(digitalPinToInterrupt(_pinB));
    }
    if (_active == this) _active = 0;
    _ready = false;
    _stablePressed = false; _click = false;
}

uint8_t MP_ENCODER_ISR MindPlusRotaryEncoder::readPin(uint8_t pin) {
#ifdef MP_ENCODER_ESP32
    // Mind+ 1.x digitalRead() calls pinMode(INPUT), which disables pull-ups.
    // Read the hardware directly; also safe while the ESP32 flash cache is off.
    return pin < 32 ? ((GPIO.in >> pin) & 1) : ((GPIO.in1.val >> (pin - 32)) & 1);
#else
    return digitalRead(pin) == HIGH;
#endif
}

void MP_ENCODER_ISR MindPlusRotaryEncoder::onEdge() {
    if (_active) _active->sample();
}

void MP_ENCODER_ISR MindPlusRotaryEncoder::sample() {
#ifdef MP_ENCODER_ESP32
    portENTER_CRITICAL_ISR(&_mux);
#endif
    const uint8_t now = (readPin(_pinA) << 1) | readPin(_pinB);
    const uint8_t change = _previous ^ now;
    if (change == 3) {
        // Both bits changed: missed/invalid transition. Resync, never invent steps.
        _partial = 0;
    } else if (change != 0) {
        // Gray-code cycle 11 -> 01 -> 00 -> 10 -> 11 is positive.
        const int8_t step = (((_previous >> 1) ^ (now & 1)) != 0) ? -1 : 1;
        _partial += step;
        if (_partial >= _transitions || _partial <= -static_cast<int8_t>(_transitions)) {
            int8_t move = _partial > 0 ? 1 : -1;
            if (_reverse) move = -move;
            _partial = 0;
            // Saturate instead of invoking signed-overflow undefined behavior.
            if ((move > 0 && _count < INT32_MAX) || (move < 0 && _count > INT32_MIN)) _count += move;
            if ((move > 0 && _delta < INT32_MAX) || (move < 0 && _delta > INT32_MIN)) _delta += move;
            _direction = move;
        }
    }
    _previous = now;
#ifdef MP_ENCODER_ESP32
    portEXIT_CRITICAL_ISR(&_mux);
#endif
}

int32_t MindPlusRotaryEncoder::read() {
    MP_LOCK; int32_t result = _count; MP_UNLOCK; return result;
}
void MindPlusRotaryEncoder::write(int32_t value) {
    MP_LOCK; _count = value; _delta = 0; _direction = 0; _partial = 0; MP_UNLOCK;
}
int32_t MindPlusRotaryEncoder::takeDelta() {
    MP_LOCK; int32_t result = _delta; _delta = 0; MP_UNLOCK; return result;
}
int8_t MindPlusRotaryEncoder::direction() {
    MP_LOCK; int8_t result = _direction; MP_UNLOCK; return result;
}
void MindPlusRotaryEncoder::setReverse(bool reverse) {
    MP_LOCK; _reverse = reverse; _partial = 0; _direction = 0; MP_UNLOCK;
}
void MindPlusRotaryEncoder::setTransitions(uint8_t transitions) {
    if (transitions != 1 && transitions != 2 && transitions != 4) return;
    MP_LOCK; _transitions = transitions; _partial = 0; MP_UNLOCK;
}

void MindPlusRotaryEncoder::update() {
    if (!_ready || _button < 0) return;
    const bool raw = readPin(_button) == LOW;
    const uint32_t now = millis();
    if (raw != _rawPressed) { _rawPressed = raw; _buttonChanged = now; }
    if (raw != _stablePressed && static_cast<uint32_t>(now - _buttonChanged) >= 20) {
        _stablePressed = raw;
        if (raw) _click = true;
    }
}
bool MindPlusRotaryEncoder::pressed() { update(); return _stablePressed; }
bool MindPlusRotaryEncoder::clicked() { update(); bool result = _click; _click = false; return result; }
