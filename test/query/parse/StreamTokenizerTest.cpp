#include <gtest/gtest.h>
#include <query/parse/StreamTokenizer.h>

namespace minisql {
namespace query {
namespace parse {

using P = std::pair<StreamTokenizer::Type, std::string>;

static const std::map<std::string, std::vector<P>> data = {
    {"select name, grad_year from   student",
     {
         {StreamTokenizer::Type::WORD, "select"},
         {StreamTokenizer::Type::WORD, "name"},
         {StreamTokenizer::Type::DELIM, ","},
         {StreamTokenizer::Type::WORD, "grad_year"},
         {StreamTokenizer::Type::WORD, "from"},
         {StreamTokenizer::Type::WORD, "student"},
     }},
    {"create table student (name varchar(12), grad_year int)",
     {
         {StreamTokenizer::Type::WORD, "create"},
         {StreamTokenizer::Type::WORD, "table"},
         {StreamTokenizer::Type::WORD, "student"},
         {StreamTokenizer::Type::DELIM, "("},
         {StreamTokenizer::Type::WORD, "name"},
         {StreamTokenizer::Type::WORD, "varchar"},
         {StreamTokenizer::Type::DELIM, "("},
         {StreamTokenizer::Type::NUMBER, "12"},
         {StreamTokenizer::Type::DELIM, ")"},
         {StreamTokenizer::Type::DELIM, ","},
         {StreamTokenizer::Type::WORD, "grad_year"},
         {StreamTokenizer::Type::WORD, "int"},
         {StreamTokenizer::Type::DELIM, ")"},
     }},
    {"insert into student (name, grad_year) values ('xxx', 2000)",
     {
         {StreamTokenizer::Type::WORD, "insert"},
         {StreamTokenizer::Type::WORD, "into"},
         {StreamTokenizer::Type::WORD, "student"},
         {StreamTokenizer::Type::DELIM, "("},
         {StreamTokenizer::Type::WORD, "name"},
         {StreamTokenizer::Type::DELIM, ","},
         {StreamTokenizer::Type::WORD, "grad_year"},
         {StreamTokenizer::Type::DELIM, ")"},
         {StreamTokenizer::Type::WORD, "values"},
         {StreamTokenizer::Type::DELIM, "("},
         {StreamTokenizer::Type::STRING, "xxx"},
         {StreamTokenizer::Type::DELIM, ","},
         {StreamTokenizer::Type::NUMBER, "2000"},
         {StreamTokenizer::Type::DELIM, ")"},
     }},
    {"select name, grad_year from student where grad_year = 2000 and name = "
     "'xxx'",
     {
         {StreamTokenizer::Type::WORD, "select"},
         {StreamTokenizer::Type::WORD, "name"},
         {StreamTokenizer::Type::DELIM, ","},
         {StreamTokenizer::Type::WORD, "grad_year"},
         {StreamTokenizer::Type::WORD, "from"},
         {StreamTokenizer::Type::WORD, "student"},
         {StreamTokenizer::Type::WORD, "where"},
         {StreamTokenizer::Type::WORD, "grad_year"},
         {StreamTokenizer::Type::DELIM, "="},
         {StreamTokenizer::Type::NUMBER, "2000"},
         {StreamTokenizer::Type::WORD, "and"},
         {StreamTokenizer::Type::WORD, "name"},
         {StreamTokenizer::Type::DELIM, "="},
         {StreamTokenizer::Type::STRING, "xxx"},
     }}};

TEST(StreamTokenizerTest, nextToken) {
  for (auto& kv : data) {
    StreamTokenizer st(kv.first);
    auto& v = kv.second;
    for (const auto& i : v) {
      ASSERT_TRUE(st.nextToken());
      ASSERT_EQ(st.type(), i.first);
      ASSERT_EQ(st.strVal(), i.second);
    }
    ASSERT_FALSE(st.nextToken());
  }
}

}  // namespace parse
}  // namespace query
}  // namespace minisql