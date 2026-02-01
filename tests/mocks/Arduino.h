#ifndef ARDUINO_H_MOCK
#define ARDUINO_H_MOCK

#include <ostream>
#include <vector>

#define OUTPUT 1
#define HIGH 1
#define LOW 0

struct GpioEvent {
  int pin;
  int value;

  bool operator==(const GpioEvent &other) const {
    return pin == other.pin && value == other.value;
  }

  friend std::ostream &operator<<(std::ostream &os, const GpioEvent &event) {
    return os << "{pin=" << event.pin << ", "
              << (event.value == HIGH ? "HIGH" : "LOW") << "}";
  }
};

class ArduinoMock {
public:
  void digitalWrite(int pin, int value) { _gpioEvents.push_back({pin, value}); }
  void clearEvents() { _gpioEvents.clear(); }
  const std::vector<GpioEvent> &getGpioEvents() const { return _gpioEvents; }

private:
  std::vector<GpioEvent> _gpioEvents;
};

extern ArduinoMock Arduino;

void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);
void delay(int ms);
void delayMicroseconds(int us);

#endif // ARDUINO_H_MOCK
