#pragma once

#include <string>

namespace minisql {
namespace query {
namespace parse {

class StreamTokenizer {
 public:
  enum class Type { NONE, DELIM, WORD, NUMBER, STRING };

  explicit StreamTokenizer(std::string s);

  bool nextToken();
  Type type() const;
  int32_t numberVal() const;
  std::string strVal() const;

 private:
  bool validPos() const;

 private:
  Type type_;
  int32_t nval_;
  std::string sval_;

  size_t pos_;
  std::string s_;
};

}  // namespace parse
}  // namespace query
}  // namespace minisql