#ifndef ARDUINO_H_MOCK
#define ARDUINO_H_MOCK

#include <cstdint>

#define OUTPUT 1
#define HIGH 1
#define LOW 0

void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
void delay(int ms);
void delayMicroseconds(int us);

#endif // ARDUINO_H_MOCK
