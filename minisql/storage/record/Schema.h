#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace minisql {
namespace storage {
namespace record {

class Schema {
 public:
  enum class Type { INT32 = 0, VARCHAR = 1 };

  struct FieldInfo {
    Type type_;
    int32_t length_;
  };

  Schema() = default;

  void addField(const std::string &fldName, Type type, int32_t length);
  void addInt32Field(const std::string &fldName);
  void addStringField(const std::string &fldName, int32_t length);
  void add(const std::string &fldName, Schema &sch);
  void addAll(const Schema &sch);
  std::vector<std::string> fields() const;
  bool hasField(const std::string &fldName) const;
  Type type(const std::string &fldName);
  int32_t length(const std::string &fldName);

 private:
  std::unordered_map<std::string, FieldInfo> info_;
};

}  // namespace record
}  // namespace storage
}  // namespace minisql