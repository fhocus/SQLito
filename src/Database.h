#ifndef DATABASE_H
#define DATABASE_H

#include "query.h"

#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <fstream>

class Database
{
private:
  Query *query_;
  std::string databaseFolder_ = "db";

  void executeCreate();
  void executeInsert();
  void executeUpdate();
  void executeSelect();

  std::vector<std::vector<std::string>> loadTableSchemas_();
  std::vector<std::pair<std::string, std::string>> loadTableSchema_(const std::string &tableName);
  std::vector<std::vector<std::string>> loadTableContent_(const std::string &tableName);
  std::vector<std::pair<std::string, std::string>> loadTableSchemaFields_(const std::string &tableName);

  void saveTableSchemas_(const std::string &tableName, const std::vector<std::vector<std::string>> &tableSchema);
  void saveTableContent_(const std::string &tableName, const std::vector<std::vector<std::string>> &tableContent);

  void printTable_(const std::vector<std::string> tableSchema, const std::vector<std::vector<std::string>> &tableContent);

  void combinateTables_(const std::vector<std::vector<std::vector<std::string>>> &, std::vector<std::vector<std::string>> &, std::vector<std::string> &, size_t);
  std::vector<std::pair<std::string, std::string>> combinateSchemas_(const std::vector<std::vector<std::pair<std::string, std::string>>> &);

public:
  Database();
  ~Database();

  void setQuery(const std::string &query);
  void execute();
};

#endif