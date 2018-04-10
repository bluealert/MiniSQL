#pragma once

#include <query/parse/StreamTokenizer.h>
#include <memory>
#include <set>
#include <string>

namespace minisql {
namespace query {
namespace parse {

class Lexer {
 public:
  explicit Lexer(std::string s);

  bool matchDelim(char d);
  bool matchStringConstant();
  bool matchKeyword(const std::string &w);
  bool matchId();

  void eatDelim(char d);
  int32_t eatIntConstant();
  std::string eatStringConstant();
  void eatKeyword(const std::string &w);
  std::string eatId();

 private:
  bool matchIntConstant();

 private:
  StreamTokenizer tok_;
};

}  // namespace parse
}  // namespace query
}  // namespace minisql