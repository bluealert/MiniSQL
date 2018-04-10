#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace minisql {
namespace storage {
namespace file {

class Block;

class FileMgr {
 public:
  FileMgr(const FileMgr &) = delete;
  FileMgr &operator=(const FileMgr &) = delete;
  FileMgr(FileMgr &&) = delete;
  FileMgr &operator=(FileMgr &&) = delete;

  static FileMgr &get();

  bool init(std::string dbDir);

  void read(const Block &blk, char *contents, int32_t len);
  void write(const Block &blk, const char *contents, int32_t len);
  std::shared_ptr<Block> append(const std::string &fileName,
                                const char *contents, int32_t len);
  int32_t blockCount(const std::string &fileName) const;

 private:
  FileMgr() = default;

  std::shared_ptr<std::fstream> getFile(const std::string &fileName);
  void writeImpl(const Block &blk, const char *contents, int32_t len);
  int32_t blockCountImpl(const std::string &fileName) const;

 private:
  std::string dbDir_;
  mutable std::mutex mutex_;
  std::unordered_map<std::string, std::shared_ptr<std::fstream>> openFiles_;
};

}  // namespace file
}  // namespace storage
}  // namespace minisql