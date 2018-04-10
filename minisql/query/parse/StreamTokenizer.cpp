#include <query/parse/StreamTokenizer.h>
#include <boost/algorithm/string/case_conv.hpp>

namespace minisql {
namespace query {
namespace parse {

StreamTokenizer::StreamTokenizer(std::string s)
    : type_(Type::NONE), nval_(0), pos_(0), s_(std::move(s)) {}

bool StreamTokenizer::nextToken() {
  type_ = Type::NONE;

  while (validPos() && std::isspace(s_[pos_])) {
    pos_++;
  }
  if (!validPos()) {
    return false;
  }

  char lastChar = s_[pos_++];
  if (lastChar == ',' || lastChar == '(' || lastChar == ')' ||
      lastChar == '=') {
    sval_.clear();
    sval_ += lastChar;
    type_ = Type::DELIM;
    return true;
  }

  if (std::isalpha(lastChar)) {
    type_ = Type::WORD;

    sval_.clear();
    sval_ += lastChar;
    while (validPos() && (std::isalnum(s_[pos_]) || s_[pos_] == '_')) {
      sval_ += s_[pos_++];
    }
    boost::to_lower(sval_);

    return true;
  }

  if (lastChar == '\'') {
    type_ = Type::STRING;

    sval_.clear();
    while (validPos() && s_[pos_] != '\'') {
      sval_ += s_[pos_++];
    }
    if (!validPos()) {
      return false;
    }
    pos_++;  // skip '

    return true;
  }

  if (std::isdigit(lastChar)) {
    type_ = Type::NUMBER;

    sval_.clear();
    sval_ += lastChar;
    while (validPos() && std::isdigit(s_[pos_])) {
      sval_ += s_[pos_++];
    }

    try {
      nval_ = std::stoi(sval_);
    } catch (const std::exception& e) {
      return false;
    }

    return true;
  }

  return false;
}

StreamTokenizer::Type StreamTokenizer::type() const { return type_; }

int32_t StreamTokenizer::numberVal() const { return nval_; }

std::string StreamTokenizer::strVal() const { return sval_; }

bool StreamTokenizer::validPos() const { return pos_ < s_.length(); }

}  // namespace parse
}  // namespace query
}  // namespace minisql