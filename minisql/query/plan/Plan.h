#pragma once

#include <memory>

namespace minisql {

namespace storage {
namespace record {
class Schema;
}
}  // namespace storage

namespace query {

class Scan;

class Plan {
 public:
  virtual ~Plan() = default;

  virtual std::unique_ptr<Scan> open() const = 0;
  virtual int32_t blocksAccessed() const = 0;
  virtual int32_t recordsOutput() const = 0;
  virtual int32_t distinctValues(const std::string& fldName) const = 0;
  virtual std::shared_ptr<storage::record::Schema> schema() const = 0;
};

}  // namespace query
}  // namespace minisql