#pragma once

#include <exception>

namespace minisql {
namespace query {
namespace parse {

class BadSyntaxException : public std::exception {
 public:
  const char *what() const noexcept override { return "bad syntax"; }
};

}  // namespace parse
}  // namespace query
}  // namespace minisql