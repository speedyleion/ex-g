#ifndef MOTION_SENSOR_HPP
#define MOTION_SENSOR_HPP
#include <Arduino.h>
#include <SPI.h>
#include <cstdint>
#include <optional>
#include <ostream>

struct Motion {
  int8_t delta_x;
  int8_t delta_y;

  bool operator==(const Motion &other) const {
    return delta_x == other.delta_x && delta_y == other.delta_y;
  }

  friend std::ostream &operator<<(std::ostream &os, const Motion &m) {
    return os << "{dx=" << (int)m.delta_x << ", dy=" << (int)m.delta_y << "}";
  }
};

// MotionSensor
class MotionSensor {
public:
  /**
   * @brief Construct a MotionSensor and configure SPI and sensor hardware.
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
  MotionSensor(int8_t cs, uint16_t dpi, int8_t sck = -1, int8_t cipo = -1,
              int8_t copi = -1);
  /**
   * @brief Get the motion since the last time motion was retrieved
   *
   * Returns the motion values from the sensor if available.
   */
  std::optional<Motion> motion();

private:
  SPISettings _settings;
  int8_t _cs;
  // DPI resolution in register units
  uint8_t _resolution;
  void initPmw();

  // public for ease of testing
public:
  uint8_t read(uint8_t reg);
  void write(uint8_t reg, uint8_t value);
  static uint8_t dpiToRegisterValue(uint16_t dpi);
};
#endif // MOTION_SENSOR_HPP
