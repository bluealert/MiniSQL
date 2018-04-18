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

    minisql::storage::tx::Transaction tx1;
    std::string sql = "create table student (name varchar(12), grad_year int)";
    planner->executeUpdate(sql, tx1);
    tx1.commit();

    const std::vector<std::pair<std::string, uint32_t>> data = {
        {"xxx", 2000}, {"yyy", 2000}, {"zzz", 2000},
        {"aaa", 2001}, {"bbb", 2001}, {"ccc", 2001}};

    for (const auto& i : data) {
      minisql::storage::tx::Transaction tx2;
      const char* fmt =
          "insert into student (name, grad_year) values ('%s', %u)";
      char buf[1024];
      sprintf(buf, fmt, i.first.c_str(), i.second);
      std::cout << buf << std::endl;
      planner->executeUpdate(buf, tx2);
      tx2.commit();
    }

    minisql::storage::tx::Transaction tx3;
    sql = "select name, grad_year from student";
    auto plan = planner->createQueryPlan(sql, tx3);
    assert(plan);
    auto scan = plan->open();
    assert(scan);
    while (scan->next()) {
      std::cout << scan->getString("name") << ", "
                << scan->getInt32("grad_year") << std::endl;
    }
    scan->close();
    tx3.commit();

    minisql::storage::tx::Transaction tx4;
    sql = "update student set grad_year = 2000 where name = 'aaa'";
    planner->executeUpdate(sql, tx4);
    tx4.commit();

    minisql::storage::tx::Transaction tx5;
    sql = "delete from student where name = 'zzz'";
    planner->executeUpdate(sql, tx5);
    tx5.commit();

    minisql::storage::tx::Transaction tx6;
    sql = "select name, grad_year from student where grad_year = 2000";
    plan = planner->createQueryPlan(sql, tx6);
    assert(plan);
    scan = plan->open();
    assert(scan);
    while (scan->next()) {
      std::cout << scan->getString("name") << ", "
                << scan->getInt32("grad_year") << std::endl;
    }
    scan->close();
    tx6.commit();

    minisql::storage::tx::Transaction tx7;
    sql =
        "select name, grad_year from student "
        "where grad_year = 2000 and name = 'xxx'";
    plan = planner->createQueryPlan(sql, tx7);
    assert(plan);
    scan = plan->open();
    assert(scan);
    while (scan->next()) {
      std::cout << scan->getString("name") << ", "
                << scan->getInt32("grad_year") << std::endl;
    }
    scan->close();
    tx7.commit();
  } catch (const std::bad_alloc& e) {
    std::cerr << "Out of memory!" << std::endl;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  } catch (...) {
    std::cerr << "Internal error" << std::endl;
  }

  return 0;
}