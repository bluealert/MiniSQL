#include <query/multibuffer/BufferNeeds.h>
#include <storage/buffer/BufferMgr.h>
#include <cmath>

namespace minisql {
namespace query {
namespace multibuffer {

int32_t BufferNeeds::bestFactor(int32_t size) {
  auto avail = storage::buffer::BufferMgr::get().available();
  if (avail <= 1) {
    return 1;
  }
  int32_t k = size;
  double i = 1.0;
  while (k > avail) {
    i++;
    k = (int32_t)std::ceil(size / i);
  }
  return k;
}

}  // namespace multibuffer
}  // namespace query
}  // namespace minisql