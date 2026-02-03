#ifndef SCROLL_WHEEL_HPP
#define SCROLL_WHEEL_HPP
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <optional>

class ScrollWheel {
public:
  /**
   * @brief Construct a ScrollWheel that uses 2 signal pins
   *
   * Sets the 2 signal pins as inputs with pull-up resistors
   *
   * @param a The first signal pin of the scroll wheel
   * @param b The second signal pin of the scroll wheel
   */
  ScrollWheel(uint8_t a, uint8_t b) : _encoder() {
    ESP32Encoder::useInternalWeakPullResistors = puType::up;
    _encoder.attachHalfQuad((int)a, (int)b);
  }

  ScrollWheel(const ScrollWheel &) = delete;
  ScrollWheel &operator=(const ScrollWheel &) = delete;

  /**
   * @brief Get the scroll delta since the last read.
   *
   * Returns the accumulated encoder count and resets it to zero.
   *
   * @return The scroll delta, or std::nullopt if no movement occurred.
   */
  std::optional<int8_t> delta() {
    _encoder.pauseCount();
    auto count = _encoder.getCount();
    if (count != 0) {
      _encoder.clearCount();
      _encoder.resumeCount();
      return (int8_t)count;
    }
    _encoder.resumeCount();
    return std::nullopt;
  }

private:
  ESP32Encoder _encoder;
};

#endif // SCROLL_WHEEL_HPP
