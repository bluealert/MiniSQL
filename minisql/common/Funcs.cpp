#include <common/Funcs.h>

namespace minisql {
namespace common {

bool startsWith(const std::string& s1, const std::string& s2) {
  return s1.compare(0, s2.length(), s2) == 0;
}

}  // namespace common
}  // namespace minisql