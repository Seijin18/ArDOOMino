#pragma once

#include <stdint.h>

#ifdef ARDUINO

#include <Arduino.h>

constexpr float SENSITIVITY  = 5.0f;
// Keypad sends Q/E once per press; x persists until space. Keep low for controllable turn.
constexpr int16_t KEYPAD_TURN_RATE = 18;

inline uint64_t getTime() {
    return micros();
}

#else

#include <chrono>

constexpr float SENSITIVITY  = 4.0f;

uint64_t getTime() {
    auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()/1000;
}

#endif