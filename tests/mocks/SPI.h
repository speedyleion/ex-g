#ifndef SPI_H_MOCK
#define SPI_H_MOCK

#include <cstdint>

#define SPI_MSBFIRST 1
#define SPI_MODE3 3

class SPISettings {
public:
    SPISettings() = default;
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
        (void)clock; (void)bitOrder; (void)dataMode;
    }
};

class SPIClass {
public:
    void begin(int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1) {
        (void)sck; (void)miso; (void)mosi;
    }
    void beginTransaction(SPISettings settings) { (void)settings; }
    void endTransaction() {}
    uint8_t transfer(uint8_t data) { (void)data; return 0; }
};

extern SPIClass SPI;

#endif // SPI_H_MOCK
