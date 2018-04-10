#include <common/Funcs.h>
#include <glog/logging.h>
#include <storage/file/Block.h>
#include <storage/file/FileMgr.h>
#include <storage/file/Page.h>
#include <boost/filesystem.hpp>

namespace minisql {
namespace storage {
namespace file {

using boost::system::error_code;
namespace fs = boost::filesystem;

FileMgr &FileMgr::get() {
  static FileMgr inst;
  return inst;
}

bool exist(const std::string &path) {
  error_code err;
  return fs::exists(path, err);
}

void createFile(const std::string &path) {
  std::fstream fs;
  fs.open(path, std::ios::out);
  if (fs.fail()) {
    throw std::runtime_error("FileMgr. can't create file. path: " + path +
                             ", reason: " + strerror(errno));
  }
  fs.close();
  LOG(INFO) << "FileMgr. create file. path: " + path;
}

void createDir(const std::string &dbDir) {
  error_code ec;
  if (!fs::create_directory(dbDir, ec)) {
    throw std::runtime_error("FileMgr: can't create directory. path: " + dbDir +
                             ", reason: " + ec.message());
  }
}

void removeTempFiles(const std::string &dbDir) {
  static const std::string temp{"temp"};
  fs::directory_iterator itEnd;
  for (fs::directory_iterator it(dbDir); it != itEnd; ++it) {
    if (fs::is_regular_file(it->status()) &&
        common::startsWith(it->path().string(), temp)) {
      fs::remove(it->path());
    }
  }
}

bool FileMgr::init(std::string dbDir) {
  auto isNew = !exist(dbDir);
  if (isNew) {
    openFiles_.clear();
    createDir(dbDir);
    removeTempFiles(dbDir);
  }
  dbDir_ = std::move(dbDir);
  return isNew;
}

void FileMgr::read(const Block &blk, char *contents, int len) {
  std::lock_guard<std::mutex> lk(mutex_);

  auto fs = getFile(blk.fileName());
  fs->seekg(blk.number() * Page::BLOCK_SIZE, std::ios_base::beg);
  fs->read(contents, len);
}

void FileMgr::write(const Block &blk, const char *contents, int len) {
  std::lock_guard<std::mutex> lk(mutex_);

  writeImpl(blk, contents, len);
}

std::shared_ptr<Block> FileMgr::append(const std::string &fileName,
                                       const char *contents, int len) {
  std::lock_guard<std::mutex> lk(mutex_);

  auto count = blockCountImpl(fileName);
  auto blk = std::make_shared<Block>(fileName, count);
  writeImpl(*blk, contents, len);
  return blk;
}

int32_t FileMgr::blockCount(const std::string &fileName) const {
  std::lock_guard<std::mutex> lk(mutex_);

  return blockCountImpl(fileName);
}

std::shared_ptr<std::fstream> FileMgr::getFile(const std::string &fileName) {
  auto it = openFiles_.find(fileName);
  if (it != openFiles_.end()) {
    return it->second;
  }

  auto path = dbDir_ + '/' + fileName;
  if (!exist(path)) {
    createFile(path);
  }

  auto fs = std::make_shared<std::fstream>();
  fs->open(path, std::ios::in | std::ios::out | std::ios::binary);
  if (fs->fail()) {
    throw std::runtime_error("FileMgr. can't open file. path: " + path +
                             ", reason: " + strerror(errno));
  }
  LOG(INFO) << "FileMgr. open file. path: " + path;
  openFiles_.emplace(std::make_pair(fileName, fs));
  return fs;
}

void FileMgr::writeImpl(const Block &blk, const char *contents, int32_t len) {
  auto f = getFile(blk.fileName());
  f->seekp(blk.number() * Page::BLOCK_SIZE, std::ios_base::beg);
  f->write(contents, len);
  f->flush();
}

int32_t FileMgr::blockCountImpl(const std::string &fileName) const {
  error_code ec;
  auto path = dbDir_ + '/' + fileName;
  if (!exist(path)) {
    return 0;
  }
  auto fileSize = fs::file_size(path, ec);
  if (ec) {
    throw std::runtime_error("FileMgr. can't get file's size. path: " + path +
                             ", reason: " + ec.message());
  }
  return (int32_t)(fileSize / Page::BLOCK_SIZE);
}

}  // namespace file
}  // namespace storage
}  // namespace minisql