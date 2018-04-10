#pragma once

#include <memory>
#include <string>

namespace minisql {
namespace common {

class ByteBuffer {
 public:
  explicit ByteBuffer(int32_t size);
  virtual ~ByteBuffer();

  char* bytes() const;

  int32_t getInt32(int32_t offset) const;
  void setInt32(int32_t offset, int32_t val);

  std::string getString(int32_t offset) const;
  void setString(int32_t offset, const std::string& s);

 private:
  int32_t size_;
  char* buf_;
};

}  // namespace common
}  // namespace minisql