#pragma once

#include <query/planner/Planner.h>

namespace minisql {
namespace init {

class MiniSQLInit {
 public:
  static void init(const std::string& dbPath);
  static bool initFileLogAndBuffer(const std::string& dbPath);
  static std::unique_ptr<query::planner::Planner> planner();
};

}  // namespace init
}  // namespace minisql