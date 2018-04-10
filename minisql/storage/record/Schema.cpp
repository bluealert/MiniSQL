#include <storage/record/Schema.h>

namespace minisql {
namespace storage {
namespace record {

void Schema::addField(const std::string &fldName, Type type, int32_t len) {
  info_.insert(std::make_pair(fldName, FieldInfo{type, len}));
}

void Schema::addInt32Field(const std::string &fldName) {
  addField(fldName, Type::INT32, 0);
}

void Schema::addStringField(const std::string &fldName, int32_t len) {
  addField(fldName, Type::VARCHAR, len);
}

void Schema::add(const std::string &fldName, Schema &sch) {
  addField(fldName, sch.type(fldName), sch.length(fldName));
}

void Schema::addAll(const Schema &sch) {
  for (const auto &kv : sch.info_) {
    addField(kv.first, kv.second.type_, kv.second.length_);
  }
}

std::vector<std::string> Schema::fields() const {
  std::vector<std::string> keys;
  for (const auto &kv : info_) {
    keys.push_back(kv.first);
  }
  return keys;
}

bool Schema::hasField(const std::string &fldName) const {
  for (const auto &kv : info_) {
    if (kv.first == fldName) {
      return true;
    }
  }
  return false;
}

Schema::Type Schema::type(const std::string &fldName) {
  return info_[fldName].type_;
}

int32_t Schema::length(const std::string &fldName) {
  return info_[fldName].length_;
}

}  // namespace record
}  // namespace storage
}  // namespace minisql