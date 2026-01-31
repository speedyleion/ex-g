#include "MouseSensor.hpp"
#include <Arduino.h>
#include <SPI.h>


const int tWakeup = 55;
// This time was re-used from capture taken for OEM EX-G initializing
// PMW3320DB-TYDU
const int tPowerUpCs = 2;

// Time between commands, in µs
// 
// Based on https://www.espruino.com/datasheets/ADNS5050.pdf
// worst case is tsww at 30 µs, which is time from last bit of first byte to 
// last bit of second byte. At max speed of, 1,000,000 Hz 8 bits would take 8 µs
// leaving a pause of 22 µs.
//
// The data sheet also mentions 
//
//    SCLK to NCS Inactive, tSCLK-NCS, 20 µs from last SCLK rising edge to NCS
//    (for write operation) rising edge, for valid SDIO data transfer.
//
// This seems to imply that we need 20 μs before moving chip select back high,
// so we'll cheat and re-use this value of 22 (for now?)
const int tWus = 22;

const int IDLE_READ = 0x00;

const int PROD_ID = 0x00;
const int POWER_UP_RESET = 0x3A;
const int PERFORMANCE = 0x22;
const int RESOLUTION = 0x0D;
const int AXIS_CONTROL = 0x1A;
const int BURST_READ_FIRST = 0x42;
const int MOTION = 0x02;
const int DELTA_X = 0x03;
const int DELTA_Y = 0x04;


MouseSensor::MouseSensor(int8_t cs, uint16_t dpi, int8_t sck, int8_t cipo, int8_t copi) {
    SPI.begin(sck, cipo, copi);
    _settings = SPISettings(1000000, SPI_MSBFIRST, SPI_MODE3);
    _cs = cs;
    initPmw();
  }

void MouseSensor::initPmw() {
  // Drive High and then low from https://media.digikey.com/pdf/data%20sheets/avago%20pdfs/adns-3050.pdf
  digitalWrite(_cs, LOW);
  digitalWrite(_cs, HIGH);
  delay(tPowerUpCs);
  digitalWrite(_cs, LOW);
  delay(tPowerUpCs);
  
  delay(tWakeup);

  write(POWER_UP_RESET, 0x5A);
  read(PROD_ID);
  write(PERFORMANCE,0x80);
  write(0x1D,	0x0A);
  write(0x14,	0x40);
  write(0x18,	0x40);
  write(0x34,	0x28);
  write(0x64,	0x32);
  write(0x65,	0x32);
  write(0x66,	0x26);
  write(0x67,	0x26);
  write(0x21,	0x04);
  write(PERFORMANCE, 0x00);
  write(RESOLUTION,	0x86);
  read(AXIS_CONTROL);
  write(AXIS_CONTROL,	0xA0);
  write(BURST_READ_FIRST,	0x03);
  read(MOTION);
  read(DELTA_X);
  read(DELTA_Y);
}

void MouseSensor::write(uint8_t reg, uint8_t value) {
  digitalWrite(_cs, LOW);
  SPI.beginTransaction(_settings);
  SPI.transfer((uint8_t)(0x80 | reg));
  delayMicroseconds(tWus);
  SPI.transfer(value);
  delayMicroseconds(tWus);
  SPI.endTransaction();
  digitalWrite(_cs, HIGH);
}

uint8_t MouseSensor::read(uint8_t reg) {
  digitalWrite(_cs, LOW);
  SPI.beginTransaction(_settings);
  SPI.transfer(reg);
  delayMicroseconds(tWus);
  uint8_t ret_value = SPI.transfer(IDLE_READ);
  delayMicroseconds(tWus);
  SPI.endTransaction();
  digitalWrite(_cs, HIGH);
  return ret_value;
}
