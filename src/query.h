#ifndef QUERY_H
#define QUERY_H

#include "triplet.h"

#include <string>
#include <vector>
#include <iostream>

class Query
{
public:
  static Query *parse(std::string);
  void print();

  Query(std::string, std::string, std::string, std::vector<std::pair<std::pair<std::string, std::string>, std::string>>, std::vector<std::string>, std::vector<Triplet<std::pair<std::string, std::string>, char, std::string>>, std::vector<std::string>, std::string);

private:
  std::string query_;
  std::string method_;
  std::string methodType_;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string>> arguments_;
  std::vector<std::string> argumentsOf_;
  std::vector<Triplet<std::pair<std::string, std::string>, char, std::string>> filters_;
  std::vector<std::string> filtersGroup_;
  std::string saveName_;
};

#endif