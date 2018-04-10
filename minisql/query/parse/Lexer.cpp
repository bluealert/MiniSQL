#include <query/parse/BadSyntaxException.h>
#include <query/parse/Lexer.h>

namespace minisql {
namespace query {
namespace parse {

namespace {
const std::set<std::string> keywords_ = {
    "select", "from",    "where",  "and", "insert", "into",
    "values", "delete",  "update", "set", "create", "table",
    "int",    "varchar", "view",   "as",  "index",  "on"};
}

Lexer::Lexer(std::string s) : tok_(std::move(s)) { tok_.nextToken(); }

bool Lexer::matchDelim(char d) {
  return tok_.type() == StreamTokenizer::Type::DELIM &&
         tok_.strVal().size() == 1 && tok_.strVal().c_str()[0] == d;
}

bool Lexer::matchIntConstant() {
  return tok_.type() == StreamTokenizer::Type::NUMBER;
}

bool Lexer::matchStringConstant() {
  return tok_.type() == StreamTokenizer::Type::STRING;
}

bool Lexer::matchKeyword(const std::string &w) {
  return tok_.type() == StreamTokenizer::Type::WORD && tok_.strVal() == w;
}

bool Lexer::matchId() {
  if (tok_.type() != StreamTokenizer::Type::WORD) {
    return false;
  }
  return keywords_.find(tok_.strVal()) == keywords_.end();
}

void Lexer::eatDelim(char d) {
  if (!matchDelim(d)) {
    throw BadSyntaxException();
  }
  tok_.nextToken();
}

int32_t Lexer::eatIntConstant() {
  if (!matchIntConstant()) {
    throw BadSyntaxException();
  }
  auto i = tok_.numberVal();
  tok_.nextToken();
  return i;
}

std::string Lexer::eatStringConstant() {
  if (!matchStringConstant()) {
    throw BadSyntaxException();
  }
  auto s = tok_.strVal();
  tok_.nextToken();
  return s;
}

void Lexer::eatKeyword(const std::string &w) {
  if (!matchKeyword(w)) {
    throw BadSyntaxException();
  }
  tok_.nextToken();
}

std::string Lexer::eatId() {
  if (!matchId()) {
    throw BadSyntaxException();
  }
  auto s = tok_.strVal();
  tok_.nextToken();
  return s;
}

}  // namespace parse
}  // namespace query
}  // namespace minisql