#pragma once

#include <common/Constant.h>
#include <query/Expression.h>
#include <query/Predicate.h>
#include <query/Term.h>
#include <storage/record/Schema.h>
#include <sstream>
#include <utility>
#include <vector>

namespace minisql {
namespace query {
namespace parse {

class QueryData {
 public:
  QueryData(std::vector<std::string> fields, std::vector<std::string> tables,
            std::unique_ptr<query::Predicate> pred)
      : fields_(std::move(fields)),
        tables_(std::move(tables)),
        pred_(std::move(pred)) {}

  std::string toString() const {
    std::stringstream ss;
    ss << "select ";
    for (auto it = fields_.begin(); it != fields_.end(); it++) {
      ss << *it;
      if (it + 1 != fields_.end()) {
        ss << ", ";
      }
    }
    ss << " from ";
    for (auto it = tables_.begin(); it != tables_.end(); it++) {
      ss << *it;
      if (it + 1 != tables_.end()) {
        ss << ", ";
      }
    }
    if (!pred_->empty()) {
      ss << " where " << pred_->toString();
    }
    return ss.str();
  }

  const std::vector<std::string> &fields() const { return fields_; }

  const std::vector<std::string> &tables() const { return tables_; }

  std::shared_ptr<query::Predicate> pred() const { return pred_; }

 private:
  std::vector<std::string> fields_;
  std::vector<std::string> tables_;
  std::shared_ptr<query::Predicate> pred_;
};

class UpdateData {
 public:
  enum Type { Insert, Modify, Delete, CreateTable, CreateIndex, CreateView };

  virtual Type type() const = 0;

  ~UpdateData() = default;
};

class InsertData : public UpdateData {
 public:
  InsertData(std::string tblName, std::vector<std::string> flds,
             const std::vector<std::shared_ptr<common::Constant>> &vals)
      : tblName_(std::move(tblName)), flds_(std::move(flds)) {
    for (auto &val : vals) {
      vals_.push_back(val);
    }
  }

  Type type() const override { return Insert; }

  std::string &tblName() { return tblName_; }

  std::vector<std::string> &flds() { return flds_; }

  std::vector<std::shared_ptr<common::Constant>> &vals() { return vals_; }

 private:
  std::string tblName_;
  std::vector<std::string> flds_;
  std::vector<std::shared_ptr<common::Constant>> vals_;
};

class ModifyData : public UpdateData {
 public:
  ModifyData(std::string tblName, std::string fldName,
             std::unique_ptr<query::Expression> newVal,
             std::unique_ptr<query::Predicate> pred)
      : tblName_(std::move(tblName)),
        fldName_(std::move(fldName)),
        pred_(std::move(pred)),
        newVal_(std::move(newVal)) {}

  Type type() const override { return Modify; }

  const std::string &tblName() const { return tblName_; }

  const std::string &fldName() const { return fldName_; }

  std::shared_ptr<query::Predicate> pred() const { return pred_; }

  const query::Expression &newVal() const { return *newVal_; }

 private:
  std::string tblName_;
  std::string fldName_;
  std::shared_ptr<query::Predicate> pred_;
  std::unique_ptr<query::Expression> newVal_;
};

class DeleteData : public UpdateData {
 public:
  DeleteData(std::string tblName, std::unique_ptr<query::Predicate> pred)
      : tblName_(std::move(tblName)), pred_(std::move(pred)) {}

  Type type() const override { return Delete; }

  const std::string &tblName() const { return tblName_; }

  std::shared_ptr<query::Predicate> pred() { return pred_; }

 private:
  std::string tblName_;
  std::shared_ptr<query::Predicate> pred_;
};

class CreateTableData : public UpdateData {
 public:
  CreateTableData(std::string tblName,
                  std::shared_ptr<storage::record::Schema> sch)
      : tblName_(std::move(tblName)), sch_(std::move(sch)) {}

  Type type() const override { return CreateTable; }

  const std::string &tblName() const { return tblName_; }

  std::shared_ptr<storage::record::Schema> sch() const { return sch_; }

 private:
  std::string tblName_;
  std::shared_ptr<storage::record::Schema> sch_;
};

class CreateIndexData : public UpdateData {
 public:
  CreateIndexData(std::string idxName, std::string tblName, std::string fldName)
      : idxName_(std::move(idxName)),
        tblName_(std::move(tblName)),
        fldName_(std::move(fldName)) {}

  Type type() const override { return CreateIndex; }

  auto idxName() const { return idxName_; }

  auto tblName() const { return tblName_; }

  auto fldName() const { return fldName_; }

 private:
  std::string idxName_;
  std::string tblName_;
  std::string fldName_;
};

class CreateViewData : public UpdateData {
 public:
  CreateViewData(std::string viewName, std::unique_ptr<QueryData> qryData)
      : viewName_(std::move(viewName)), qryData_(std::move(qryData)) {}

  Type type() const override { return CreateView; }

  auto viewName() const { return viewName_; }

  auto viewDef() const { return qryData_->toString(); }

 private:
  std::string viewName_;
  std::unique_ptr<QueryData> qryData_;
};

}  // namespace parse
}  // namespace query
}  // namespace minisql