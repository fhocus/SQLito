#include "query.h"
#include "triplet.h"

#include <sstream>
#include <algorithm>

Query *Query::parse(std::string query)
{
  std::stringstream queryStream(query);
  std::string queryToken;

  std::string method;
  std::string methodType;
  std::vector<std::pair<std::pair<std::string, std::string>, std::string>> arguments;
  std::vector<std::string> argumentsOf;
  std::vector<Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>>> filters;
  std::vector<std::string> filtersGroup;
  std::string saveName;

  // Obtener el método
  queryStream >> queryToken;
  method = queryToken;
  std::transform(method.begin(), method.end(), method.begin(), ::tolower);

  if (method == "create")
  {
    // Obtener el tipo de método
    queryStream >> queryToken;
    methodType = queryToken;
    std::transform(methodType.begin(), methodType.end(), methodType.begin(), ::tolower);

    if (methodType == "table")
    {
      // Obtener el nombre de dónde se va a trabajar
      queryStream >> queryToken;
      std::transform(queryToken.begin(), queryToken.end(), queryToken.begin(), ::tolower);
      argumentsOf.push_back(queryToken);

      queryStream >> queryToken;
      std::transform(queryToken.begin(), queryToken.end(), queryToken.begin(), ::tolower);

      if (queryToken == "set")
      {
        // Obtener los argumentos
        while (std::getline(queryStream, queryToken, ','))
        {
          std::stringstream argumentStream(queryToken);
          std::string argumentName;
          std::string argumentType;

          argumentStream >> argumentName;
          std::transform(argumentName.begin(), argumentName.end(), argumentName.begin(), ::tolower);
          argumentStream >> argumentType;
          std::transform(argumentType.begin(), argumentType.end(), argumentType.begin(), ::tolower);

          arguments.push_back(std::make_pair(std::make_pair(argumentsOf[0], argumentName), argumentType));
        }
      }
      else
      {
        return nullptr;
      }
    }

    else
    {
      return nullptr;
    }
  }
  else if (method == "insert" || method == "update")
  {
    // Obtener el nombre de dónde se va a trabajar
    queryStream >> queryToken;
    std::transform(queryToken.begin(), queryToken.end(), queryToken.begin(), ::tolower);
    argumentsOf.push_back(queryToken);

    queryStream >> queryToken;
    std::transform(queryToken.begin(), queryToken.end(), queryToken.begin(), ::tolower);

    // Verificar la palabra SET
    if (queryToken == "set")
    {
      std::string queryNext;
      // Obtener los argumentos
      while (std::getline(queryStream, queryToken, ','))
      {
        std::stringstream argumentStream(queryToken);
        std::string argumentName;
        std::string argumentValue;

        argumentStream >> argumentName;
        std::transform(argumentName.begin(), argumentName.end(), argumentName.begin(), ::tolower);

        std::getline(argumentStream, argumentValue, '\'');
        std::getline(argumentStream, argumentValue, '\'');

        arguments.push_back(std::make_pair(std::make_pair(argumentsOf[0], argumentName), argumentValue));

        std::getline(argumentStream, queryNext);
      }

      queryStream = std::stringstream(queryNext);
      queryStream >> queryToken;
      std::transform(queryToken.begin(), queryToken.end(), queryToken.begin(), ::tolower);

      if (queryToken == "where")
      {
        std::string fieldName;
        char op;
        std::string fieldValue;
        std::string logicOperator;

        filtersGroup.push_back("and");

        while (queryStream >> fieldName)
        {
          std::transform(fieldName.begin(), fieldName.end(), fieldName.begin(), ::tolower);
          queryStream >> op;

          std::getline(queryStream, fieldValue, '\'');
          std::getline(queryStream, fieldValue, '\'');

          filters.push_back(Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>>(std::make_pair(argumentsOf[0], fieldName), op, std::make_pair(argumentsOf[0], fieldValue)));

          queryStream >> logicOperator;
          std::transform(logicOperator.begin(), logicOperator.end(), logicOperator.begin(), ::tolower);
          if (logicOperator == "and" || logicOperator == "or")
          {
            filtersGroup.push_back(logicOperator);
            logicOperator = "";
          }
        }
      }
    }

    else
    {
      return nullptr;
    }
  }
  else if (method == "select")
  {
    std::string queryNext;

    // Obtener los argumentos
    while (std::getline(queryStream, queryToken, ','))
    {
      std::stringstream argumentStream(queryToken);
      std::string argumentOf = "";
      std::string argumentName;
      if (queryToken.find('.') != std::string::npos)
      {
        argumentStream >> queryToken;
        std::stringstream argumentStreamClean(queryToken);
        std::getline(argumentStreamClean, argumentOf, '.');
        std::getline(argumentStreamClean, argumentName, '.');
      }
      else
      {
        argumentStream >> argumentName;
        if (argumentName == "*")
        {
          argumentOf = "all";
        }
      }
      std::transform(argumentOf.begin(), argumentOf.end(), argumentOf.begin(), ::tolower);
      std::transform(argumentName.begin(), argumentName.end(), argumentName.begin(), ::tolower);
      arguments.push_back(std::make_pair(std::make_pair(argumentOf, argumentName), ""));

      std::getline(argumentStream, queryNext);
    }

    queryStream = std::stringstream(queryNext);
    queryStream >> queryToken;
    std::transform(queryToken.begin(), queryToken.end(), queryToken.begin(), ::tolower);

    if (queryToken == "from")
    {
      while (std::getline(queryStream, queryToken, ';'))
      {
        std::stringstream argumentOfStream(queryToken);
        std::string argumentOf;

        argumentOfStream >> argumentOf;
        std::transform(argumentOf.begin(), argumentOf.end(), argumentOf.begin(), ::tolower);

        argumentsOf.push_back(argumentOf);

        std::getline(argumentOfStream, queryNext);
      }
    }
    else
    {
      return nullptr;
    }

    // TODO: Modificar para que vincule los argumentos de la consulta con la tabla referente en caso exista.
    for (size_t i = 0; i < arguments.size(); i++)
    {
      if (arguments[i].first.first == "")
      {
        arguments[i].first.first = argumentsOf[0];
      }
    }

    queryStream = std::stringstream(queryNext);
    queryStream >> queryToken;
    std::transform(queryToken.begin(), queryToken.end(), queryToken.begin(), ::tolower);

    std::string logicOperator;
    if (queryToken == "where")
    {
      std::string argumentOfFirst = argumentsOf[0];
      std::string argumentFirst;
      std::string argumentOfSecond = argumentsOf[0];
      std::string argumentSecond;
      char op;

      filtersGroup.push_back("and");

      while (queryStream >> argumentFirst)
      {
        if (argumentFirst.find('.') != std::string::npos)
        {
          std::stringstream argumentFirstStream(argumentFirst);
          std::getline(argumentFirstStream, argumentOfFirst, '.');
          std::getline(argumentFirstStream, argumentFirst, '.');
        }

        std::transform(argumentFirst.begin(), argumentFirst.end(), argumentFirst.begin(), ::tolower);

        queryStream >> op;

        queryStream >> argumentSecond;
        std::stringstream argumentSecondStream(argumentSecond);

        if (argumentSecond.find('.') != std::string::npos)
        {
          std::getline(argumentSecondStream, argumentOfSecond, '.');
          std::getline(argumentSecondStream, argumentSecond, '.');
          std::transform(argumentSecond.begin(), argumentSecond.end(), argumentSecond.begin(), ::tolower);
        }
        else
        {
          std::getline(argumentSecondStream, argumentSecond, '\'');
          std::getline(argumentSecondStream, argumentSecond, '\'');
          argumentOfSecond = argumentOfFirst;
        }

        filters.push_back(Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>>(std::make_pair(argumentOfFirst, argumentFirst), op, std::make_pair(argumentOfSecond, argumentSecond)));

        queryStream >> logicOperator;
        std::transform(logicOperator.begin(), logicOperator.end(), logicOperator.begin(), ::tolower);
        if (logicOperator == "and" || logicOperator == "or")
        {
          filtersGroup.push_back(logicOperator);
          logicOperator = "";
        }
      }
    }

    if (logicOperator == "|")
    {
      queryStream >> queryToken;
      std::transform(queryToken.begin(), queryToken.end(), queryToken.begin(), ::tolower);
      saveName = queryToken;
    }
  }
  else
  {
    return nullptr;
  }

  return new Query(query, method, methodType, arguments, argumentsOf, filters, filtersGroup, saveName);
}

Query::Query(std::string query, std::string method, std::string methodType, std::vector<std::pair<std::pair<std::string, std::string>, std::string>> arguments, std::vector<std::string> argumentsOf, std::vector<Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>>> filters, std::vector<std::string> filtersGroup, std::string saveName)
{
  query_ = query;
  method_ = method;
  methodType_ = methodType;
  arguments_ = arguments;
  argumentsOf_ = argumentsOf;
  filters_ = filters;
  filtersGroup_ = filtersGroup;
  saveName_ = saveName;
}

void Query::print()
{
  std::cout << "Query: " << query_ << std::endl;
  std::cout << "Method: " << method_ << std::endl;
  std::cout << "Method Type: " << methodType_ << std::endl;

  std::cout << "Arguments: " << std::endl;
  for (const auto &arg : arguments_)
  {
    std::cout << "  " << arg.first.first << "." << arg.first.second << " : " << arg.second << std::endl;
  }

  std::cout << "Arguments Of: " << std::endl;
  for (const auto &argOf : argumentsOf_)
  {
    std::cout << "  " << argOf << std::endl;
  }

  std::cout << "Filters: " << std::endl;
  for (const auto &filter : filters_)
  {
    std::cout << "  " << filter.getFirst().first << "." << filter.getFirst().second << " " << filter.getSecond() << " " << filter.getThird().first << "." << filter.getThird().second << std::endl;
  }

  std::cout << "Filters Group: " << std::endl;
  for (const auto &group : filtersGroup_)
  {
    std::cout << "  " << group << std::endl;
  }

  std::cout << "Save Name: " << saveName_ << std::endl;
}

Query::~Query() {}

std::string Query::getQuery() const
{
  return query_;
}

std::string Query::getMethod() const
{
  return method_;
}

std::string Query::getMethodType() const
{
  return methodType_;
}

std::vector<std::pair<std::pair<std::string, std::string>, std::string>> Query::getArguments() const
{
  return arguments_;
}

std::vector<std::string> Query::getArgumentsOf() const
{
  return argumentsOf_;
}

std::vector<Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>>> Query::getFilters() const
{
  return filters_;
}

std::vector<std::string> Query::getFiltersGroup() const
{
  return filtersGroup_;
}

std::string Query::getSaveName() const
{
  return saveName_;
}