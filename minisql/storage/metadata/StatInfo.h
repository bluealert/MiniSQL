#pragma once

#include <cstdint>
#include <string>

namespace minisql {
namespace storage {
namespace metadata {

class StatInfo {
 public:
  StatInfo(int32_t numBlocks, int32_t numRecs);

  int32_t blocksAccessed() const;
  int32_t recordsOutput() const;
  int32_t distinctValues(const std::string &fldName) const;

 private:
  int32_t numBlocks_, numRecs_;
};

}  // namespace metadata
}  // namespace storage
}  // namespace minisql