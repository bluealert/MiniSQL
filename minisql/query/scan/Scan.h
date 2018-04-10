#pragma once

#include <memory>
#include <string>

namespace minisql {

namespace common {
class Constant;
}

namespace query {

class Scan {
 public:
  virtual ~Scan() = default;

  virtual void beforeFirst() = 0;
  virtual bool next() = 0;

  virtual bool hasField(const std::string& fldName) const = 0;
  virtual int32_t getInt32(const std::string& fldName) const = 0;
  virtual std::string getString(const std::string& fldName) const = 0;

  virtual std::shared_ptr<common::Constant> getVal(
      const std::string& fldName) const = 0;

  virtual void close() = 0;
};

}  // namespace query
}  // namespace minisql