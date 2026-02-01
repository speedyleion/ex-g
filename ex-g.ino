#include "MotionSensor.hpp"
#include <USB.h>
#include <USBHIDMouse.h>
#include <optional>

USBHIDMouse Mouse;
std::optional<MotionSensor> sensor;
/**
 * @brief Called once at program startup to perform initialization.
 *
 * Place any hardware or application initialization code here; this function
 * is invoked once before the main execution loop begins.
 */
void setup() {
  Mouse.begin();
  USB.begin();
  sensor.emplace(D7, 1500);
}

/**
 * @brief Executes repeatedly after setup to perform the sketch's main logic.
 *
 * This function is invoked in a continuous loop by the Arduino runtime; place
 * recurring or periodic code here. Currently the implementation is empty.
 */
void loop() {
  auto motion = sensor->motion();
  if (motion) {
    Mouse.move(motion->delta_x, motion->delta_y);
  }
}
