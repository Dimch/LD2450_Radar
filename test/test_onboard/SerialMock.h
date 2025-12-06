#ifndef __SerialMock_H__
#define __SerialMock_H__

#include <Arduino.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;

class SerialFake {
public:
  uint32_t dataIndex;
  vector<uint8_t> data;

  SerialFake() {
    dataIndex = 0;
  }
  void init(const vector<uint8_t>& dataToRead) {
    data = dataToRead;
    dataIndex = 0;
  }
  int available() {
    return data.size() - dataIndex;
  }
  int read() {
    return available() ? data[dataIndex++] : -1;
  }
  size_t readBytes(uint8_t *buffer, size_t length) {
    size_t count = 0;
    while (count < length) {
      int c = read();
      if (c < 0) {
        break;
      }
      *buffer++ = (uint8_t)c;
      count++;
    }
    return count;
  }
  
};

// Test the read_until method with this
class SerialMock : public Stream {
public:
  MOCK_METHOD(int, available, (), (override));
  MOCK_METHOD(int, read, (), (override));
  MOCK_METHOD(int, peek, (), (override));
  MOCK_METHOD(size_t, write, (uint8_t), (override));
  MOCK_METHOD(size_t, readBytes, (uint8_t*, size_t), (override));
};

#endif
