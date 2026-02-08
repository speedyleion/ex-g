#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <optional>

#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>

/**
 * @brief Represents the state of a button press event.
 */
enum class ButtonState {
  PRESSED, ///< Button was pressed down
  RELEASED ///< Button was released
};

/**
 * @brief Read button state changes with software debouncing
 *
 * Configured for active-low buttons with internal pull-up.
 */
class Button {
public:
  /**
   * @brief Construct a Button with the specified pin.
   *
   * Configures the pin as input with internal pull-up resistor and
   * sets a 2ms debounce interval.
   *
   * @param pin The GPIO pin connected to the button
   */
  Button(uint8_t pin) : _button() {
    _button.attach(pin, INPUT_PULLUP);
    // From testing the left and right click of the ex-g were a little over
    // 1 ms bouncing. The middle click was closer to 600ms for bouncing
    _button.interval(2);
    _button.setPressedState(LOW);
  }

  /**
   * @brief Poll the button and return the new state if it changed.
   *
   * Must be called regularly (e.g., in loop()) to detect state changes.
   *
   * @return ButtonState::PRESSED or ButtonState::RELEASED if the state
   *         changed since the last call, std::nullopt otherwise.
   */
  std::optional<ButtonState> stateChange() {
    _button.update();
    if (_button.pressed()) {
      return ButtonState::PRESSED;
    }
    if (_button.released()) {
      return ButtonState::RELEASED;
    }
    return std::nullopt;
  }

private:
  Bounce2::Button _button;
};

#endif // BUTTON_HPP
