#include "MouseSensor.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("MouseSensor initializes PMW3320DB-TYDU", "[PMW-Init]") {
  auto [dpi, expected_resolution] = GENERATE(table<uint16_t, uint8_t>({
      {750, 0x83},
      {1500, 0x86},
  }));
  SPI.clearMessages();

  auto sensor = MouseSensor(3, dpi);

  const auto &messages = SPI.getMessages();

  REQUIRE(SPI.allMessagesInTransaction());

  REQUIRE(messages[0] == SPIMessage{0xBA, 0x5A}); // write(POWER_UP_RESET, 0x5A)
  REQUIRE(messages[1] == SPIMessage{0x00, 0x00}); // read(PROD_ID)
  REQUIRE(messages[2] == SPIMessage{0xA2, 0x80}); // write(PERFORMANCE, 0x80)
  REQUIRE(messages[3] == SPIMessage{0x9D, 0x0A}); // write(Unknown)
  REQUIRE(messages[4] == SPIMessage{0x94, 0x40}); // write(Unknown)
  REQUIRE(messages[5] == SPIMessage{0x98, 0x40}); // write(Unknown)
  REQUIRE(messages[6] == SPIMessage{0xB4, 0x28}); // write(Unknown)
  REQUIRE(messages[7] == SPIMessage{0xE4, 0x32}); // write(Unknown)
  REQUIRE(messages[8] == SPIMessage{0xE5, 0x32}); // write(Unknown)
  REQUIRE(messages[9] == SPIMessage{0xE6, 0x26}); // write(Unknown)
  REQUIRE(messages[10] == SPIMessage{0xE7, 0x26}); // write(Unknown)
  REQUIRE(messages[11] == SPIMessage{0xA1, 0x04}); // write(Unknown)
  REQUIRE(messages[12] == SPIMessage{0xA2, 0x00}); // write(PERFORMANCE, 0x00)
  REQUIRE(messages[13] ==
          SPIMessage{0x8D, expected_resolution});  // write(RESOLUTION, value)
  REQUIRE(messages[14] == SPIMessage{0x1A, 0x00}); // read(AXIS_CONTROL)
  REQUIRE(messages[15] == SPIMessage{0x9A, 0xA0}); // write(AXIS_CONTROL, 0xA0)
  REQUIRE(messages[16] == SPIMessage{0xC2, 0x02}); // write(BURST_READ, MOTION)
  REQUIRE(messages[17] == SPIMessage{0x02, 0x00}); // read(MOTION)
  REQUIRE(messages[18] == SPIMessage{0x03, 0x00}); // read(DELTA_X)
  REQUIRE(messages[19] == SPIMessage{0x04, 0x00}); // read(DELTA_Y)
}

TEST_CASE("read and write toggle CS appropriately", "[SPI-CS]") {
  const int8_t cs_pin = 3;
  auto sensor = MouseSensor(cs_pin, 1000);

  // Clear events from init
  Arduino.clearEvents();
  SPI.clearMessages();

  SECTION("write pulls CS low before transfer, high after") {
    sensor.write(0x22, 0x80);

    const auto &events = Arduino.getGpioEvents();
    REQUIRE(events.size() == 2);
    REQUIRE(events[0] == GpioEvent{cs_pin, LOW});
    REQUIRE(events[1] == GpioEvent{cs_pin, HIGH});
  }

  SECTION("read pulls CS low before transfer, high after") {
    sensor.read(0x00);

    const auto &events = Arduino.getGpioEvents();
    REQUIRE(events.size() == 2);
    REQUIRE(events[0] == GpioEvent{cs_pin, LOW});
    REQUIRE(events[1] == GpioEvent{cs_pin, HIGH});
  }
}

TEST_CASE("dpiToRegisterValue converts DPI to register steps",
          "[dpiToRegisterValue]") {
  SECTION("clamps values below minimum to 1 step") {
    auto dpi = GENERATE(0, 1, 124, 249);
    CAPTURE(dpi);
    REQUIRE(MouseSensor::dpiToRegisterValue(dpi) == 1);
  }

  SECTION("clamps values above maximum to 14 steps") {
    auto dpi = GENERATE(3501, 4000, 5000, 65535);
    CAPTURE(dpi);
    REQUIRE(MouseSensor::dpiToRegisterValue(dpi) == 14);
  }

  SECTION("rounds to nearest 250 DPI step") {
    auto [dpi, expected] = GENERATE(table<uint16_t, uint8_t>({
        // Exact boundaries
        {250, 1},
        {1000, 4},
        {1500, 6},
        {3500, 14},
        // Rounding down (below midpoint)
        {374, 1}, // 250 + 124 -> rounds to 1
        {624, 2}, // 500 + 124 -> rounds to 2
        // Rounding up (at or above midpoint)
        {375, 2}, // 250 + 125 -> rounds to 2
        {625, 3}, // 500 + 125 -> rounds to 3
    }));
    CAPTURE(dpi);
    REQUIRE(MouseSensor::dpiToRegisterValue(dpi) == expected);
  }
}
