#include "Arduino.h"

ArduinoMock Arduino;

void pinMode(int, int) {}
void digitalWrite(int pin, int value) { Arduino.digitalWrite(pin, value); }
void delay(int) {}
void delayMicroseconds(int) {}
