#include <common/Constant.h>
#include <query/scan/ProjectScan.h>

namespace minisql {
namespace query {

ProjectScan::ProjectScan(std::unique_ptr<Scan> s,
                         const std::vector<std::string> &fieldList)
    : s_(std::move(s)), fieldList_(fieldList) {}

void ProjectScan::beforeFirst() { s_->beforeFirst(); }

bool ProjectScan::next() { return s_->next(); }

void ProjectScan::close() { s_->close(); }

std::shared_ptr<common::Constant> ProjectScan::getVal(
    const std::string &fldName) const {
  if (hasField(fldName)) {
    return s_->getVal(fldName);
  }
  throw std::runtime_error("ProjectScan. field \"" + fldName + "\" not found.");
}

int32_t ProjectScan::getInt32(const std::string &fldName) const {
  if (hasField(fldName)) {
    return s_->getInt32(fldName);
  }
  throw std::runtime_error("ProjectScan: field \"" + fldName + "\" not found.");
}

std::string ProjectScan::getString(const std::string &fldName) const {
  if (hasField(fldName)) {
    return s_->getString(fldName);
  }
  throw std::runtime_error("ProjectScan: field \"" + fldName + "\" not found.");
}

bool ProjectScan::hasField(const std::string &fldName) const {
  return std::find(fieldList_.begin(), fieldList_.end(), fldName) !=
         fieldList_.end();
}

}  // namespace query
}  // namespace minisql