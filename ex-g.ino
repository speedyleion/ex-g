#include "Button.hpp"
#include "MotionSensor.hpp"
#include "ScrollWheel.hpp"
#include <USB.h>
#include <USBHIDMouse.h>
#include <optional>

USBHIDMouse Mouse;

struct MouseButton {
  uint8_t pin;
  uint8_t mouseButton;
  std::optional<Button> button;
};

enum MouseButtonIndex : uint8_t {
  LEFT = 0,
  RIGHT = 1,
  MIDDLE = 2,
};

// The USB mouse takes the entire serial pipe, which prevents uploading new
// software. To avoid needing to access the boot button on the board, this
// flag is used to skip the mouse logic, leaving the serial bus open.
// This is achieved by holding down left and right click while plugging in the
// device.
bool serialUploadMode = false;
std::optional<MotionSensor> sensor;
std::optional<ScrollWheel> scrollWheel;
MouseButton mouseButtons[] = {
    {D2, MOUSE_LEFT, {}},
    {D3, MOUSE_RIGHT, {}},
    {D4, MOUSE_MIDDLE, {}},
};

/**
 * @brief Check if LEFT and RIGHT are held low for 1 second to enable serial
 * upload mode.
 * @return true if both buttons were held low for the full duration.
 */
bool checkSerialUploadMode() {
  pinMode(mouseButtons[LEFT].pin, INPUT_PULLUP);
  pinMode(mouseButtons[RIGHT].pin, INPUT_PULLUP);
  unsigned long start = millis();
  while (millis() - start < 1000) {
    if (digitalRead(mouseButtons[LEFT].pin) != LOW ||
        digitalRead(mouseButtons[RIGHT].pin) != LOW) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Called once at program startup to perform initialization.
 *
 * Place any hardware or application initialization code here; this function
 * is invoked once before the main execution loop begins.
 */
void setup() {
  serialUploadMode = checkSerialUploadMode();
  if (serialUploadMode) {
    return;
  }

  Mouse.begin();
  USB.begin();
  // D8, D9, D10 are SPI pins
  sensor.emplace(D7, 1500);
  scrollWheel.emplace(D0, D1);
  for (auto &mb : mouseButtons) {
    mb.button.emplace(mb.pin);
  }
}

/**
 * @brief Executes repeatedly after setup to perform the sketch's main logic.
 *
 * This function is invoked in a continuous loop by the Arduino runtime; place
 * recurring or periodic code here. Currently the implementation is empty.
 */
void loop() {
  if (serialUploadMode) {
    return;
  }
  auto motion = sensor->motion();
  auto scroll = scrollWheel->delta();
  if (motion || scroll) {
    auto m = motion.value_or(Motion{0, 0});
    Mouse.move(m.delta_x, m.delta_y, scroll.value_or(0));
  }

  for (auto &mb : mouseButtons) {
    auto state = mb.button->stateChange();
    if (state) {
      if (*state == ButtonState::PRESSED) {
        Mouse.press(mb.mouseButton);
      } else {
        Mouse.release(mb.mouseButton);
      }
    }
  }
}
