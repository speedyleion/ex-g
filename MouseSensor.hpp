#include <Arduino.h>
#include <SPI.h>
// MouseSensor 
class MouseSensor{
public:
  MouseSensor(int8_t cs, uint16_t dpi, int8_t sck = -1, int8_t cipo = -1, int8_t copi = -1);
  SPISettings _settings;
  int8_t _cs;
private:
  void initPmw();
  uint8_t read(uint8_t reg);
  void write(uint8_t reg, uint8_t value);
};
