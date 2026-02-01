#ifndef SPI_TRANSACTION_HPP
#define SPI_TRANSACTION_HPP

#include <Arduino.h>
#include <SPI.h>

/**
 * @brief RAII wrapper for SPI transactions with chip-select control.
 *
 * Manages the lifecycle of an SPI transaction by beginning the transaction
 * and asserting chip-select LOW on construction, then releasing chip-select
 * HIGH and ending the transaction on destruction. This ensures proper cleanup
 * even when exceptions occur or early returns are taken.
 */
class SpiTransaction {
public:
  /**
   * @brief Begin an SPI transaction and assert chip-select.
   *
   * @param cs Chip-select pin to drive LOW for the duration of the transaction.
   * @param settings SPI configuration (clock speed, bit order, mode).
   */
  SpiTransaction(int8_t cs, SPISettings &settings) : _cs(cs) {
    SPI.beginTransaction(settings);
    digitalWrite(_cs, LOW);
  }

  /**
   * @brief End the SPI transaction and release chip-select.
   *
   * Drives the chip-select pin HIGH and calls SPI.endTransaction().
   */
  ~SpiTransaction() {
    digitalWrite(_cs, HIGH);
    SPI.endTransaction();
  }

  SpiTransaction(const SpiTransaction &) = delete;
  SpiTransaction &operator=(const SpiTransaction &) = delete;

private:
  int8_t _cs;
};

#endif
