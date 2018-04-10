#include <storage/metadata/StatInfo.h>

namespace minisql {
namespace storage {
namespace metadata {

StatInfo::StatInfo(int32_t numBlocks, int32_t numRecs)
    : numBlocks_(numBlocks), numRecs_(numRecs) {}

int32_t StatInfo::blocksAccessed() const { return numBlocks_; }

int32_t StatInfo::recordsOutput() const { return numRecs_; }

int32_t StatInfo::distinctValues(const std::string &fldName) const {
  return (int32_t)(1 + (numRecs_ / 3.0));
}

}  // namespace metadata
}  // namespace storage
}  // namespace minisql