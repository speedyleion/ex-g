#include "MouseSensor.hpp"
#include <Arduino.h>
#include <SPI.h>

// Based on https://www.espruino.com/datasheets/ADNS5050.pdf
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

// Register addresses from
// https://www.epsglobal.com/Media-Library/EPSGlobal/Products/files/pixart/PMW3320DB-TYDU.pdf
const int PROD_ID = 0x00;
const int POWER_UP_RESET = 0x3A;
const int PERFORMANCE = 0x22;
const int RESOLUTION = 0x0D;
const int AXIS_CONTROL = 0x1A;
const int BURST_READ_FIRST = 0x42;
const int MOTION = 0x02;
const int DELTA_X = 0x03;
const int DELTA_Y = 0x04;

// As specified in
// https://www.epsglobal.com/Media-Library/EPSGlobal/Products/files/pixart/PMW3320DB-TYDU.pdf
const int MAX_DPI = 3500;
const int DPI_RESOLUTION = 250;
const int MAX_CLOCK_SPEED = 1'000'000;

/**
 * @brief Construct a MouseSensor and configure SPI and sensor hardware.
 *
 * Initializes SPI with the provided SCK/CIPO/COPI pins, applies SPI settings
 * (1 MHz, MSB first, mode 3), stores the chip-select pin, and runs the PMW
 * sensor initialization sequence.
 *
 * @param cs Chip-select pin connected to the sensor.
 * @param dpi Sensor DPI value (logical configuration; may be used elsewhere).
 * @param sck Serial clock pin (SCLK).
 * @param cipo Controller-In-Peripheral-Out pin (CIPO).
 * @param copi Controller-Out-Peripheral-In pin (COPI).
 */
MouseSensor::MouseSensor(int8_t cs, uint16_t dpi, int8_t sck, int8_t cipo,
                         int8_t copi) {
  SPI.begin(sck, cipo, copi);
  _settings = SPISettings(MAX_CLOCK_SPEED, SPI_MSBFIRST, SPI_MODE3);
  _cs = cs;
  _resolution = dpiToRegisterValue(dpi);

  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH); // Deselect initially
  initPmw();
}

uint8_t MouseSensor::dpiToRegisterValue(uint16_t dpi) {
  if (dpi < DPI_RESOLUTION) {
    dpi = DPI_RESOLUTION;
  }
  if (dpi > MAX_DPI) {
    dpi = MAX_DPI;
  }
  uint16_t steps = (dpi + (DPI_RESOLUTION / 2)) / DPI_RESOLUTION;
  return (uint8_t)steps;
}

/**
 * @brief Initializes the PMW/ADNS optical sensor and configures its operating
 * registers.
 *
 * Performs the required chip-select wake/power-up sequence and executes the
 * sensor initialization register sequence to reset the device, configure
 * performance and resolution, set axis control, and enable burst/motion
 * reporting.
 */
void MouseSensor::initPmw() {
  // Drive High and then low from
  // https://media.digikey.com/pdf/data%20sheets/avago%20pdfs/adns-3050.pdf
  digitalWrite(_cs, LOW);
  digitalWrite(_cs, HIGH);
  delay(tPowerUpCs);
  digitalWrite(_cs, LOW);
  delay(tPowerUpCs);

  delay(tWakeup);

  write(POWER_UP_RESET, 0x5A);
  // The OEM software read this value, copying the behavior to be safe
  read(PROD_ID);
  write(PERFORMANCE, 0x80);
  // These registers are unknown. They were observed to be written to by the OEM
  // EX-G software,
  // https://speedyleion.github.io/mice/electronics/2026/01/11/ex-g-pmw3320db-tydu-spi-traffic.html
  write(0x1D, 0x0A);
  write(0x14, 0x40);
  write(0x18, 0x40);
  write(0x34, 0x28);
  write(0x64, 0x32);
  write(0x65, 0x32);
  write(0x66, 0x26);
  write(0x67, 0x26);
  write(0x21, 0x04);
  write(PERFORMANCE, 0x00);
  // The resolution for the PMW3320DB-TYDU is documented as a max of 3500 DPI
  // with a 250 DPI resolution. Observing the OEM EX-G software a value of 0x83
  // was sent for 750 DPI, and a value of 0x86 was sent for a value of 1500 DPI
  // It seems that the MSB needs to be set, and that the LSB's represent the DPI
  // value. 3500/250 = 14 or 0x0D so this is likely 0x81-0x8D
  write(RESOLUTION, 0x80 | _resolution);
  // The OEM software read the value before writing, copying the behavior to be
  // safe
  read(AXIS_CONTROL);
  // The 0XA0 value was observed from the OEM EX-G software. It's likely
  // specific to the physical orientation of the sensor in the case.
  write(AXIS_CONTROL, 0xA0);
  write(BURST_READ_FIRST, 0x02);

  // The OEM software read these. I'm thinking it's likely to ensure they're
  // cleared.
  read(MOTION);
  read(DELTA_X);
  read(DELTA_Y);
}

/**
 * @brief Writes a byte to a sensor register over SPI.
 *
 * @param reg Sensor register address to write to.
 * @param value Data byte to write into the register.
 */
void MouseSensor::write(uint8_t reg, uint8_t value) {
  SPI.beginTransaction(_settings);
  digitalWrite(_cs, LOW);
  SPI.transfer((uint8_t)(0x80 | reg));
  delayMicroseconds(tWus);
  SPI.transfer(value);
  delayMicroseconds(tWus);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
}

/**
 * @brief Read a single byte from a PMW/ADNS sensor register over SPI.
 *
 * Selects the sensor, issues a read for the given register address, and returns
 * the byte read from that register.
 *
 * @param reg Register address to read.
 * @return uint8_t The byte value read from the specified register.
 */
uint8_t MouseSensor::read(uint8_t reg) {
  SPI.beginTransaction(_settings);
  digitalWrite(_cs, LOW);
  SPI.transfer(reg);
  delayMicroseconds(tWus);
  uint8_t ret_value = SPI.transfer(IDLE_READ);
  delayMicroseconds(tWus);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  return ret_value;
}
