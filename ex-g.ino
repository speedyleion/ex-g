#include "Button.hpp"
#include "MotionSensor.hpp"
#include "ScrollWheel.hpp"
#include <USB.h>
#include <USBHIDMouse.h>
#include <optional>

USBHIDMouse Mouse;

// The USB mouse takes the entire serial pipe. This prevents uploading new
// software. To prevent needing to access the boot button of the board. This
// flag is used to prevent the mouse logic, leaving the serial bus open.
// This is achieved by holding down left and right click while plugging in the
// device.
bool serialUploadMode = false;
std::optional<MotionSensor> sensor;
std::optional<ScrollWheel> scrollWheel;
std::optional<Button> buttons[3];
const uint8_t mouseButtons[] = {MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE};
static_assert(sizeof(buttons) / sizeof(buttons[0]) ==
                  sizeof(mouseButtons) / sizeof(mouseButtons[0]),
              "buttons and mouseButtons arrays must have the same count");

/**
 * @brief Check if D2 and D3 are held low for 1 second to enable serial upload
 * mode.
 * @return true if both buttons were held low for the full duration.
 */
bool checkSerialUploadMode() {
  pinMode(D2, INPUT_PULLUP);
  pinMode(D3, INPUT_PULLUP);
  unsigned long start = millis();
  while (millis() - start < 1000) {
    if (digitalRead(D2) != LOW || digitalRead(D3) != LOW) {
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
  buttons[0].emplace(D2);
  buttons[1].emplace(D3);
  buttons[2].emplace(D4);
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

  for (size_t i = 0; i < std::size(buttons); i++) {
    auto state = buttons[i]->stateChange();
    if (state) {
      if (*state == ButtonState::PRESSED) {
        Mouse.press(mouseButtons[i]);
      } else {
        Mouse.release(mouseButtons[i]);
      }
    }
  }
}
