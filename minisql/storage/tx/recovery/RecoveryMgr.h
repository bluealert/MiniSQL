#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace minisql {
namespace storage {

namespace file {
class Block;
}
namespace buffer {
class Buffer;
}

namespace tx {
namespace recovery {

class RecoveryMgr {
 public:
  RecoveryMgr() = default;

  void start(int32_t txNum);

  int32_t setInt32(const buffer::Buffer &buff, int32_t offset, int32_t newVal);
  int32_t setString(const buffer::Buffer &buff, int32_t offset,
                    const std::string &newVal);

  void commit();
  void rollback();
  void recover();

 private:
  void doRollback();
  void doRecover();

 private:
  int32_t txNum_;
};

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql