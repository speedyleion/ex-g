#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include "MouseSensor.hpp"

TEST_CASE("dpiToRegisterValue converts DPI to register steps", "[dpiToRegisterValue]") {
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
            {374, 1},   // 250 + 124 -> rounds to 1
            {624, 2},   // 500 + 124 -> rounds to 2
            // Rounding up (at or above midpoint)  
            {375, 2},   // 250 + 125 -> rounds to 2
            {625, 3},   // 500 + 125 -> rounds to 3
        }));
        CAPTURE(dpi);
        REQUIRE(MouseSensor::dpiToRegisterValue(dpi) == expected);
    }
}
