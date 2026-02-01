#ifndef SPI_H_MOCK
#define SPI_H_MOCK

#include <cstdint>
#include <ostream>
#include <queue>
#include <vector>

#define SPI_MSBFIRST 1
#define SPI_MODE3 3

class SPISettings {
public:
  SPISettings() = default;
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
    (void)clock;
    (void)bitOrder;
    (void)dataMode;
  }
};

struct SPIMessage {
  uint8_t reg;
  uint8_t value;

  bool operator==(const SPIMessage &other) const {
    return reg == other.reg && value == other.value;
  }

  friend std::ostream &operator<<(std::ostream &os, const SPIMessage &msg) {
    return os << "{0x" << std::hex << static_cast<int>(msg.reg) << ", 0x"
              << static_cast<int>(msg.value) << std::dec << "}";
  }
};

class SPIClass {
public:
  void begin(int8_t sck = -1, int8_t cipo = -1, int8_t copi = -1) {
    (void)sck;
    (void)cipo;
    (void)copi;
  }
  void beginTransaction(SPISettings settings) {
    (void)settings;
    _inTransaction = true;
  }
  void endTransaction() { _inTransaction = false; }
  uint8_t transfer(uint8_t data) {
    if (_pendingReg < 0) {
      _pendingReg = data;
      _pendingInTransaction = _inTransaction;
    } else {
      _messages.push_back({static_cast<uint8_t>(_pendingReg), data});
      if (!_pendingInTransaction || !_inTransaction) {
        _hasOutOfTransactionMessage = true;
      }
      _pendingReg = -1;
    }
    if (!_responses.empty()) {
      uint8_t ret = _responses.front();
      _responses.pop();
      return ret;
    }
    return 0;
  }

  void queueResponse(uint8_t value) { _responses.push(value); }
  void queueResponses(std::initializer_list<uint8_t> values) {
    for (auto v : values) {
      _responses.push(v);
    }
  }

  void clearMessages() {
    _messages.clear();
    _pendingReg = -1;
    _hasOutOfTransactionMessage = false;
    _responses = {};
  }
  const std::vector<SPIMessage> &getMessages() const { return _messages; }
  bool allMessagesInTransaction() const { return !_hasOutOfTransactionMessage; }

private:
  bool _inTransaction = false;
  bool _pendingInTransaction = false;
  bool _hasOutOfTransactionMessage = false;
  int16_t _pendingReg = -1;
  std::vector<SPIMessage> _messages;
  std::queue<uint8_t> _responses;
};

extern SPIClass SPI;

#endif // SPI_H_MOCK
