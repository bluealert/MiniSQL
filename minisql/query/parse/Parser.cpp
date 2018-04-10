#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <query/ConstantExpression.h>
#include <query/FieldNameExpression.h>
#include <query/parse/Parser.h>

namespace minisql {
namespace query {
namespace parse {

using common::Constant;
using common::Int32Constant;
using common::StringConstant;

Parser::Parser(std::string s) : lex_(std::move(s)) {}

std::string Parser::field() { return lex_.eatId(); }

std::shared_ptr<Constant> Parser::constant() {
  if (lex_.matchStringConstant()) {
    return std::make_shared<StringConstant>(lex_.eatStringConstant());
  } else {
    return std::make_shared<Int32Constant>(lex_.eatIntConstant());
  }
}

std::unique_ptr<Expression> Parser::expression() {
  if (lex_.matchId()) {
    return std::make_unique<FieldNameExpression>(field());
  } else {
    return std::make_unique<ConstantExpression>(constant());
  }
}

std::unique_ptr<Term> Parser::term() {
  auto lhs = expression();
  lex_.eatDelim('=');
  auto rhs = expression();
  return std::make_unique<Term>(std::move(lhs), std::move(rhs));
}

std::unique_ptr<Predicate> Parser::predicate() {
  auto pred = std::make_unique<Predicate>(term());
  if (lex_.matchKeyword("and")) {
    lex_.eatKeyword("and");
    pred->conjoinWith(*predicate());
  }
  return pred;
}

std::unique_ptr<QueryData> Parser::query() {
  lex_.eatKeyword("select");
  auto fields = selectList();
  lex_.eatKeyword("from");
  auto tables = tableList();
  std::unique_ptr<Predicate> pred;
  if (lex_.matchKeyword("where")) {
    lex_.eatKeyword("where");
    pred = predicate();
  }
  return std::make_unique<QueryData>(fields, tables, std::move(pred));
}

std::vector<std::string> Parser::selectList() {
  std::vector<std::string> sl;
  sl.push_back(field());
  if (lex_.matchDelim(',')) {
    lex_.eatDelim(',');
    for (auto &s : selectList()) {
      sl.push_back(s);
    }
  }
  return sl;
}

std::vector<std::string> Parser::tableList() {
  std::vector<std::string> tl;
  tl.push_back(lex_.eatId());
  if (lex_.matchDelim(',')) {
    lex_.eatDelim(',');
    for (auto &s : tableList()) {
      tl.push_back(s);
    }
  }
  return tl;
}

std::unique_ptr<UpdateData> Parser::updateCmd() {
  if (lex_.matchKeyword("insert")) {
    return insert();
  } else if (lex_.matchKeyword("delete")) {
    return del();
  } else if (lex_.matchKeyword("update")) {
    return update();
  } else {
    return create();
  }
}

std::unique_ptr<UpdateData> Parser::create() {
  lex_.eatKeyword("create");
  if (lex_.matchKeyword("table")) {
    return createTable();
  } else if (lex_.matchKeyword("view")) {
    return createView();
  } else {
    return createIndex();
  }
}

std::unique_ptr<DeleteData> Parser::del() {
  lex_.eatKeyword("delete");
  lex_.eatKeyword("from");
  auto tblName = lex_.eatId();
  std::unique_ptr<query::Predicate> pred;
  if (lex_.matchKeyword("where")) {
    lex_.eatKeyword("where");
    pred = predicate();
  }
  return std::make_unique<DeleteData>(tblName, std::move(pred));
}

std::unique_ptr<InsertData> Parser::insert() {
  lex_.eatKeyword("insert");
  lex_.eatKeyword("into");
  auto tblName = lex_.eatId();
  lex_.eatDelim('(');
  auto flds = fieldList();
  lex_.eatDelim(')');
  lex_.eatKeyword("values");
  lex_.eatDelim('(');
  auto vals = constList();
  lex_.eatDelim(')');
  return std::make_unique<InsertData>(tblName, flds, vals);
}

std::unique_ptr<ModifyData> Parser::update() {
  lex_.eatKeyword("update");
  auto tblName = lex_.eatId();
  lex_.eatKeyword("set");
  auto fldName = field();
  lex_.eatDelim('=');
  auto newVal = expression();
  std::unique_ptr<query::Predicate> pred;
  if (lex_.matchKeyword("where")) {
    lex_.eatKeyword("where");
    pred = predicate();
  }
  return std::make_unique<ModifyData>(tblName, fldName, std::move(newVal),
                                      std::move(pred));
}

std::unique_ptr<CreateTableData> Parser::createTable() {
  lex_.eatKeyword("table");
  auto tblName = lex_.eatId();
  lex_.eatDelim('(');
  auto sch = fieldDefs();
  lex_.eatDelim(')');
  return std::make_unique<CreateTableData>(tblName, std::move(sch));
}

std::unique_ptr<CreateViewData> Parser::createView() {
  lex_.eatKeyword("view");
  auto viewName = lex_.eatId();
  lex_.eatKeyword("as");
  return std::make_unique<CreateViewData>(viewName, query());
}

std::unique_ptr<CreateIndexData> Parser::createIndex() {
  lex_.eatKeyword("index");
  auto idxName = lex_.eatId();
  lex_.eatKeyword("on");
  auto tblName = lex_.eatId();
  lex_.eatDelim('(');
  auto fldName = field();
  lex_.eatDelim(')');
  return std::make_unique<CreateIndexData>(idxName, tblName, fldName);
}

std::vector<std::string> Parser::fieldList() {
  std::vector<std::string> fl;
  fl.push_back(field());
  if (lex_.matchDelim(',')) {
    lex_.eatDelim(',');
    for (auto &s : fieldList()) {
      fl.push_back(s);
    }
  }
  return fl;
}

std::vector<std::shared_ptr<Constant>> Parser::constList() {
  std::vector<std::shared_ptr<Constant>> cl;
  cl.push_back(constant());
  if (lex_.matchDelim(',')) {
    lex_.eatDelim(',');
    for (auto &c : constList()) {
      cl.push_back(std::move(c));
    }
  }
  return cl;
}

std::unique_ptr<storage::record::Schema> Parser::fieldDefs() {
  auto sch = fieldDef();
  if (lex_.matchDelim(',')) {
    lex_.eatDelim(',');
    auto sch2 = fieldDefs();
    sch->addAll(*sch2);
  }
  return sch;
}

std::unique_ptr<storage::record::Schema> Parser::fieldDef() {
  return fieldType(field());
}

std::unique_ptr<storage::record::Schema> Parser::fieldType(
    const std::string &fldName) {
  auto sch = std::make_unique<storage::record::Schema>();
  if (lex_.matchKeyword("int")) {
    lex_.eatKeyword("int");
    sch->addInt32Field(fldName);
  } else {
    lex_.eatKeyword("varchar");
    lex_.eatDelim('(');
    auto strLen = lex_.eatIntConstant();
    lex_.eatDelim(')');
    sch->addStringField(fldName, strLen);
  }
  return sch;
}

}  // namespace parse
}  // namespace query
}  // namespace minisql