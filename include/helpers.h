#ifndef __Helpers_H__
#define __Helpers_H__

#include <Arduino.h>
#include <memory>
#include <sstream>
#include <iomanip>
#include <span>

using namespace std;

template<typename T>
vector<T> concatenate_vectors(const vector<vector<T>>& vectors) {
  size_t totalSize = 0;
  for (const auto& v : vectors) {
    totalSize += v.size();
  }
  vector<T> result;
  result.reserve(totalSize);
  for (const auto& v : vectors) {
    result.insert(result.end(), v.begin(), v.end());
  }
  return result;
}

vector<uint8_t> read_until(Stream *stream, span<const uint8_t> bytes) {
  if (bytes.size() == 0) return vector<uint8_t>{};

  uint8_t val;
  auto match = bytes.begin();
  vector<uint8_t> entry;
  entry.reserve(100);
  while (stream->readBytes(&val, 1)) {
    entry.push_back(val);
    if (val == *match) {
      match++;
      if (match == bytes.end()) {
        return entry;
      }
    } else {
      match = bytes.begin();
    }
  }
  return vector<uint8_t>{};
}

template<typename T>
vector<uint8_t> to_little_endian(T value) {
  T netValue = htons(value);  // convert to little
  vector<uint8_t> result;
  result.reserve(sizeof netValue);
  result.insert(result.begin(), sizeof netValue, 0);
  memcpy(&result[0], &value, result.size());
  return result;
}

// template<typename T>
// T from_little_endian(const uint8_t *value) {
//   T result;
//   memcpy(&result, value, sizeof result);
//   return result;  // htons(result);
// }

template<typename T>
T from_little_endian(span<const uint8_t> bytes) {
  T result;
  if (sizeof result != bytes.size()) return 0;  // need to throw up here!
  memcpy(&result, &(*bytes.begin()), sizeof result);
  return result;  // htons(result);
}

template<typename T>
bool contains(const vector<T>& source, const vector<T>& target) {
  auto foundIt = search(source.begin(), source.end(), target.begin(), target.end());
  return foundIt != source.end();
}

/**
 * @brief Convert 16 bit value to signed integer. The highest bit 1 corresponds
 * to the positive speed, 0 corresponds to the negative value.
 * 
 * @param value unsigned 16 bit integer
 * @return signed 16 bit integer with correct value
 */
int16_t to_signed(uint16_t value) {
  return value < (1<<15) ? 0 - value : value - (1<<15);
}

string bytes_to_hex(span<const uint8_t> bytes) {
  stringstream ss;
  ss << hex << setfill('0');
  for_each(bytes.begin(), bytes.end(), [&](auto x) { ss << setw(2) << static_cast<int>(x) << " "; });
  return ss.str();
}

#endif
