#pragma once

namespace minisql {
namespace storage {

namespace file {
class Page;
}

namespace buffer {

class PageFormatter {
 public:
  virtual ~PageFormatter() = default;

  virtual void format(file::Page &p) = 0;
};

}  // namespace buffer
}  // namespace storage
}  // namespace minisql