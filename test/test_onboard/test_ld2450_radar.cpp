#include <Arduino.h>
#include "helpers.h"
#include "./SerialMock.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using ::testing::_;
using ::testing::ElementsAre;

/// Let the testing begin...

TEST(Helpers, Concatenate_Vectors) {
  auto v1 = vector<uint8_t>{0x01, 0x05, 0x0A};
  auto v2 = vector<uint8_t>{0x02, 0x06, 0x0B};
  auto result = concatenate_vectors<uint8_t>({v1, v2});
  EXPECT_EQ(result.size(), 6) << "Should have 6 elements";
  EXPECT_THAT(result, ElementsAre(0x01, 0x05, 0x0A, 0x02, 0x06, 0x0B));
}

TEST(Helpers, Read_Until) {
  auto streamData = vector<uint8_t>{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  auto terminatorBytes = vector<uint8_t>{0x06, 0x07};
  auto serialMock = SerialMock();
  auto serialFake = SerialFake();
  serialFake.init(streamData);
  ON_CALL(serialMock, available).WillByDefault([&serialFake]() {
    return serialFake.available();
  });
  ON_CALL(serialMock, read).WillByDefault([&serialFake]() {
    return serialFake.read();
  });
  ON_CALL(serialMock, readBytes).WillByDefault([&serialFake](uint8_t *buffer, size_t length) {
    return serialFake.readBytes(buffer, length);
  });;
  EXPECT_EQ(serialMock.available(), 8); 
  // works down to here
  auto readData = read_until(&serialMock, terminatorBytes);
  EXPECT_EQ(readData.size(), 7) << "Should have 7 bytes";
  EXPECT_THAT(readData, ElementsAre(0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07)) << "Should return correct elements";
  EXPECT_EQ(serialMock.available(), 1);
  auto lastByte = serialMock.read();
  EXPECT_EQ(lastByte, 0x08) << "Should return correct value";
  EXPECT_EQ(serialMock.available(), 0) << "Stream should be unavailable";
  auto moreData = read_until(&serialMock, terminatorBytes);
  EXPECT_EQ(moreData.size(), 0) << "Should return empty data immediately";
}

TEST(Helpers, From_Little_Endian) {
  auto v = vector<uint8_t>{0x05, 0x01};
  auto result = from_little_endian<int16_t>(v);
  EXPECT_EQ(result, 261) << "Should be correct value";
  v = vector<uint8_t>{0xF0, 0xD8};
  auto result2 = from_little_endian<int16_t>(v);
  EXPECT_EQ(result2, -10000) << "Should be correct value";
}

TEST(Helpers, To_Little_Endian) {
  auto result = to_little_endian<int16_t>(261);
  EXPECT_EQ(result.size(), 2) << "Should have 2 elements";
  EXPECT_THAT(result, ElementsAre(0x05, 0x01)) << "Should have correct elements";
  auto result2 = to_little_endian<int16_t>(-10000);
  EXPECT_EQ(result2.size(), 2) << "Should have 2 elements";
  EXPECT_THAT(result2, ElementsAre(0xF0, 0xD8)) << "Should have correct elements";
}

TEST(Helpers, To_From_Little_Endian) {
  auto little = to_little_endian<int16_t>(261);
  auto result = from_little_endian<int16_t>(little);
  EXPECT_EQ(result, 261);
}

TEST(Helpers, Contains) {
  auto v = vector<uint8_t>{0x05, 0x01, 0x06, 0x02, 0x0F, 0x0A};
  auto search = vector<uint8_t>{0x02, 0x0F};
  auto result = contains<uint8_t>(v, search);
  EXPECT_EQ(result, true) << "Should return true";
  search = vector<uint8_t>{0x0A, 0x0F};
  result = contains<uint8_t>(v, search);
  EXPECT_EQ(result, false) << "Should return false";
}

TEST(Helpers, To_Signed) {
  auto result = to_signed(180);
  EXPECT_EQ(result, -180) << "Should be correct value";
  result = to_signed(34481);
  EXPECT_EQ(result, 1713) << "Should be correct value";
}
// TEST_F(...)

void setup() {
  // should be the same value as for the `test_speed` option in "platformio.ini"
  // default value is test_speed=115200
  Serial.begin(115200);

  ::testing::InitGoogleTest();
  // if you plan to use GMock, replace the line above with
  // ::testing::InitGoogleMock();
}

void loop() {
  // Run tests
  if (RUN_ALL_TESTS())
  ;

  // sleep for 1 sec
  delay(1000);
}
