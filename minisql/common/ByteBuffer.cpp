#include <common/ByteBuffer.h>

namespace minisql {
namespace common {

static constexpr int32_t INT_SIZE{sizeof(int32_t)};

ByteBuffer::ByteBuffer(int32_t size) : size_(size) { buf_ = new char[size]; }

ByteBuffer::~ByteBuffer() { delete[] buf_; }

int32_t ByteBuffer::getInt32(int32_t offset) const {
  auto p = buf_ + offset;
  if (p + INT_SIZE >= buf_ + size_) {
    throw std::overflow_error("ByteBuffer::getInt32");
  }
  return ((p[0] & 0xFF) << 24) | ((p[1] & 0xFF) << 16) | ((p[2] & 0xFF) << 8) |
         (p[3] & 0xFF);
}

void ByteBuffer::setInt32(int32_t offset, int32_t val) {
  auto p = buf_ + offset;
  if (p + INT_SIZE >= buf_ + size_) {
    throw std::overflow_error("ByteBuffer::setInt32");
  }
  p[0] = static_cast<char>(val >> 24);
  p[1] = static_cast<char>(val >> 16);
  p[2] = static_cast<char>(val >> 8);
  p[3] = static_cast<char>(val);
}

std::string ByteBuffer::getString(int32_t offset) const {
  auto len = getInt32(offset);
  auto p = buf_ + offset + INT_SIZE;
  if (p + INT_SIZE + len >= buf_ + size_) {
    throw std::overflow_error("ByteBuffer::getString");
  }
  return std::string(p, (unsigned long)len);
}

void ByteBuffer::setString(int32_t offset, const std::string& s) {
  setInt32(offset, (int32_t)s.length());
  auto p = buf_ + offset;
  if (p + INT_SIZE + s.length() >= buf_ + size_) {
    throw std::overflow_error("ByteBuffer::setString");
  }
  ::memcpy(p + INT_SIZE, s.c_str(), s.length());
}

char* ByteBuffer::bytes() const { return buf_; }

}  // namespace common
}  // namespace minisql