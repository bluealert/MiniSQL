#pragma once

#include <common/Constant.h>
#include <query/Expression.h>
#include <query/Predicate.h>
#include <query/Term.h>
#include <query/parse/Lexer.h>
#include <query/parse/ParseData.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {
namespace parse {

class Parser {
 public:
  explicit Parser(std::string s);

  std::unique_ptr<QueryData> query();
  std::unique_ptr<UpdateData> updateCmd();

 private:
  std::string field();
  std::shared_ptr<common::Constant> constant();
  std::unique_ptr<query::Expression> expression();
  std::unique_ptr<query::Term> term();
  std::unique_ptr<query::Predicate> predicate();

  std::unique_ptr<UpdateData> create();
  std::unique_ptr<DeleteData> del();
  std::unique_ptr<InsertData> insert();
  std::unique_ptr<ModifyData> update();

  std::unique_ptr<CreateTableData> createTable();
  std::unique_ptr<CreateViewData> createView();
  std::unique_ptr<CreateIndexData> createIndex();

  std::vector<std::string> selectList();
  std::vector<std::string> tableList();
  std::vector<std::string> fieldList();
  std::vector<std::shared_ptr<common::Constant>> constList();

  std::unique_ptr<storage::record::Schema> fieldDefs();
  std::unique_ptr<storage::record::Schema> fieldDef();
  std::unique_ptr<storage::record::Schema> fieldType(
      const std::string &fldName);

 private:
  Lexer lex_;
};

}  // namespace parse
}  // namespace query
}  // namespace minisql