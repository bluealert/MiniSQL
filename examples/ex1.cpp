#include <init/MiniSQLInit.h>
#include <query/scan/Scan.h>
#include <storage/tx/Transaction.h>
#include <boost/filesystem.hpp>
#include <iostream>

using boost::system::error_code;
namespace fs = boost::filesystem;

int main() {
  try {
    error_code err;
    const std::string dbPath = "/tmp/minisql";
    if (fs::exists(dbPath, err)) {
      fs::remove_all(dbPath, err);
    }

    minisql::init::MiniSQLInit::init(dbPath);

    auto planner = minisql::init::MiniSQLInit::planner();

    // student table
    minisql::storage::tx::Transaction tx1;
    std::string sql =
        "create table student (sid int, sname varchar(12), grad_year int, "
        "major_id int)";
    planner->executeUpdate(sql, tx1);
    tx1.commit();

    struct StudentData {
      int32_t sid;
      std::string sname;
      int32_t grad_year;
      int32_t major_id;
    };
    const std::vector<StudentData> data = {
        {1, "joe", 2004, 10}, {2, "amy", 2004, 20}, {3, "max", 2005, 10},
        {4, "sue", 2005, 20}, {5, "bob", 2003, 30}, {6, "kim", 2001, 20},
        {7, "art", 2004, 30}, {8, "pat", 2001, 20}, {9, "lee", 2004, 10}};

    for (const auto& i : data) {
      minisql::storage::tx::Transaction tx2;
      const char* fmt =
          "insert into student (sid, sname, grad_year, major_id) values (%d, "
          "'%s', %d, %d)";
      char buf[1024];
      sprintf(buf, fmt, i.sid, i.sname.c_str(), i.grad_year, i.major_id);
      std::cout << buf << std::endl;
      planner->executeUpdate(buf, tx2);
      tx2.commit();
    }

    // dept table
    minisql::storage::tx::Transaction tx3;
    sql = "create table dept (did int, dname varchar(12))";
    planner->executeUpdate(sql, tx3);
    tx3.commit();

    struct DeptData {
      int32_t did;
      std::string dname;
    };
    const std::vector<DeptData> data2 = {
        {10, "compsci"}, {20, "math"}, {30, "drama"}};

    for (const auto& i : data2) {
      minisql::storage::tx::Transaction tx4;
      const char* fmt = "insert into dept (did, dname) values (%d, '%s')";
      char buf[1024];
      sprintf(buf, fmt, i.did, i.dname.c_str());
      std::cout << buf << std::endl;
      planner->executeUpdate(buf, tx4);
      tx4.commit();
    }

    minisql::storage::tx::Transaction tx5;
    sql =
        "select sid, sname, grad_year, did, dname from student, dept where "
        "major_id = did";
    auto plan = planner->createQueryPlan(sql, tx5);
    assert(plan);
    auto scan = plan->open();
    assert(scan);
    while (scan->next()) {
      std::cout << scan->getInt32("sid") << ", " << scan->getString("sname")
                << ", " << scan->getInt32("grad_year") << ", "
                << scan->getInt32("did") << ", " << scan->getString("dname")
                << std::endl;
    }
    scan->close();
    tx5.commit();
  } catch (const std::bad_alloc& e) {
    std::cerr << "Out of memory!" << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Internal error" << std::endl;
  }
}