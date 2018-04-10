#include <storage/file/FileMgr.h>
#include <storage/file/Page.h>

namespace minisql {
namespace storage {
namespace file {

constexpr int32_t Page::INT32_SIZE;
constexpr int32_t Page::BLOCK_SIZE;

void Page::read(const Block &blk) {
  FileMgr::get().read(blk, buf_.bytes(), BLOCK_SIZE);
}

void Page::write(const Block &blk) {
  FileMgr::get().write(blk, buf_.bytes(), BLOCK_SIZE);
}

std::shared_ptr<Block> Page::append(const std::string &fileName) {
  return FileMgr::get().append(fileName, buf_.bytes(), BLOCK_SIZE);
}

int32_t Page::getInt32(int32_t offset) const {
  std::lock_guard<std::mutex> lk(mutex_);
  return buf_.getInt32(offset);
}

void Page::setInt32(int32_t offset, int32_t val) {
  std::lock_guard<std::mutex> lk(mutex_);
  buf_.setInt32(offset, val);
}

std::string Page::getString(int32_t offset) const {
  std::lock_guard<std::mutex> lk(mutex_);
  return buf_.getString(offset);
}

void Page::setString(int32_t offset, const std::string &s) {
  std::lock_guard<std::mutex> lk(mutex_);
  buf_.setString(offset, s);
}

}  // namespace file
}  // namespace storage
}  // namespace minisql