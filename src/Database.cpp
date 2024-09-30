#include "Database.h"

#include <algorithm>

Database::Database()
{
  if (!std::filesystem::exists(databaseFolder_))
  {
    std::filesystem::create_directory(databaseFolder_);
    std::ofstream schemeFile(databaseFolder_ + "/schema.txt");
    schemeFile.close();
  }
}

Database::~Database() {}

void Database::setQuery(const std::string &query)
{
  query_ = Query::parse(query);
}

void Database::execute()
{
  if (query_ == nullptr)
  {
    std::cout << "No hay consulta disponible." << std::endl;
    return;
  }

  std::string method = query_->getMethod();

  if (method == "create")
  {
    executeCreate();
  }
  else if (method == "insert")
  {
    executeInsert();
  }
  else if (method == "update")
  {
    executeUpdate();
  }
  else if (method == "select")
  {
    executeSelect();
  }
  else
  {
    std::cout << "ERROR: Método no reconocido." << std::endl;
  }
}

void Database::executeCreate()
{
  if (query_->getMethodType() != "table")
  {
    return;
  }

  std::vector<std::vector<std::string>> tableSchemas = loadTableSchemas_();

  std::string tableName = query_->getArgumentsOf()[0];
  for (const std::vector<std::string> &tableSchema : tableSchemas)
  {
    if (tableSchema[0] == tableName)
    {
      std::cout << "La tabla \"" << tableName << "\" ya existe." << std::endl;
      return;
    }
  }

  std::vector<std::string> tableSchema;
  tableSchema.push_back(tableName);

  for (std::pair<std::pair<std::string, std::string>, std::string> arg : query_->getArguments())
  {
    tableSchema.push_back(arg.first.second);
    tableSchema.push_back(arg.second);
  }

  tableSchemas.push_back(tableSchema);

  saveTableSchemas_(tableName, tableSchemas);
}

void Database::executeInsert()
{
  std::string tableName = query_->getArgumentsOf()[0];
  std::vector<std::pair<std::string, std::string>> tableSchema = loadTableSchema_(tableName);
  if (tableSchema.size() == 0)
  {
    std::cout << "La tabla \"" << tableName << "\" no existe." << std::endl;
    return;
  }

  std::vector<std::string> insertContent;

  for (const std::pair<std::string, std::string> &field : tableSchema)
  {
    bool found = false;
    for (std::pair<std::pair<std::string, std::string>, std::string> &arg : query_->getArguments())
    {
      if (arg.first.second == field.first)
      {
        if (field.second == "int")
        {
          try
          {
            std::stoi(arg.second);
          }
          catch (const std::invalid_argument &e)
          {
            std::cout << "ERROR: El valor \"" << arg.second << "\" no es de tipo \"" << field.second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
            return;
          }
        }
        else if (field.second == "bool")
        {
          try
          {
            std::transform(arg.second.begin(), arg.second.end(), arg.second.begin(), ::tolower);

            if (arg.second == "true")
            {
              arg.second = "1";
            }
            else if (arg.second == "false")
            {
              arg.second = "0";
            }
            else
            {
              int value = std::stoi(arg.second);

              if (value != 0 && value != 1)
              {
                std::cout << "ERROR: El valor \"" << arg.second << "\" no es de tipo \"" << field.second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
                return;
              }
            }
          }
          catch (const std::invalid_argument &e)
          {
            std::cout << "ERROR: El valor \"" << arg.second << "\" no es de tipo \"" << field.second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
            return;
          }
        }
        insertContent.push_back(arg.second);

        found = true;
        break;
      }
    }

    if (!found)
    {
      std::cout << "ERROR: Falta el campo \"" << field.first << "\" en la tabla \"" << tableName << "\"." << std::endl;
      return;
    }
  }

  std::vector<std::vector<std::string>> tableContent = loadTableContent_(tableName);
  tableContent.push_back(insertContent);
  saveTableContent_(tableName, tableContent);
}

void Database::executeUpdate()
{
  std::string tableName = query_->getArgumentsOf()[0];
  std::vector<std::pair<std::string, std::string>> tableSchema = loadTableSchema_(tableName);
  if (tableSchema.size() == 0)
  {
    std::cout << "La tabla \"" << tableName << "\" no existe." << std::endl;
    return;
  }

  std::vector<std::vector<std::string>> tableContent = loadTableContent_(tableName);

  const std::vector<Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>>> filters = query_->getFilters();
  const std::vector<std::string> filtersGroup = query_->getFiltersGroup();

  for (size_t i = 0; i < tableContent.size(); i++)
  {
    bool isValid = true;
    for (size_t j = 0; j < filters.size(); j++)
    {
      Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>> filter = filters[j];
      std::string filterGroup = filtersGroup[j];
      std::pair<std::string, std::string> filterField = filter.getFirst();
      char filterOperator = filter.getSecond();
      std::string filterValue = filter.getThird().second;

      size_t schema_i;
      bool fountSchema = false;
      for (size_t k = 0; k < tableSchema.size(); k++)
      {
        if (tableSchema[k].first == filter.getFirst().second)
        {
          if (tableSchema[k].second == "int")
          {
            try
            {
              std::stoi(filterValue);
            }
            catch (const std::invalid_argument &e)
            {
              std::cout << "ERROR: El valor \"" << filterValue << "\" no es de tipo \"" << tableSchema[k].second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
              return;
            }
          }
          else if (tableSchema[k].second == "bool")
          {
            try
            {
              std::transform(filterValue.begin(), filterValue.end(), filterValue.begin(), ::tolower);

              if (filterValue == "true")
              {
                filter.setThird(std::make_pair(filter.getThird().first, "1"));
              }
              else if (filterValue == "false")
              {
                filter.setThird(std::make_pair(filter.getThird().first, "0"));
              }
              else
              {
                int value = std::stoi(filterValue);

                if (value != 0 && value != 1)
                {
                  std::cout << "ERROR: El valor \"" << filterValue << "\" no es de tipo \"" << tableSchema[k].second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
                  return;
                }
              }
            }
            catch (const std::invalid_argument &e)
            {
              std::cout << "ERROR: El valor \"" << filterValue << "\" no es de tipo \"" << tableSchema[k].second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
              return;
            }
          }
          schema_i = k;
          fountSchema = true;
          break;
        }
      }

      if (!fountSchema)
      {
        std::cout << "ERROR: El campo \"" << filters[j].getFirst().second << "\" no existe en la tabla \"" << tableName << "\"." << std::endl;
        return;
      }

      int filterValueI;
      int contentValueI;

      if (filterField.second == "int")
      {
        filterValueI = std::stoi(filterValue);
        contentValueI = std::stoi(tableContent[i][schema_i]);
      }

      if (filterGroup == "and")
      {
        if (filterOperator == '=')
        {
          if (filterField.second == "int")
          {
            isValid &= filterValueI == contentValueI;
          }
          else
          {
            isValid &= filterValue == tableContent[i][schema_i];
          }
        }
        else if (filterOperator == '>')
        {
          if (filterField.second == "int")
          {
            isValid &= filterValueI > contentValueI;
          }
          else
          {
            std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para el tipo \"" << filterField.second << "\"." << std::endl;
            return;
          }
        }
        else if (filterOperator == '<')
        {
          if (filterField.second == "int")
          {
            isValid &= filterValueI < contentValueI;
          }
          else
          {
            std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para el tipo \"" << filterField.second << "\"." << std::endl;
            return;
          }
        }
        else if (filterOperator == '!')
        {
          if (filterField.second == "int")
          {
            isValid &= filterValueI != contentValueI;
          }
          else
          {
            isValid &= filterValue != tableContent[i][schema_i];
          }
        }
      }
      else if (filterGroup == "or")
      {
        if (filterOperator == '=')
        {
          if (filterField.second == "int")
          {
            isValid |= filterValueI == contentValueI;
          }
          else
          {
            isValid |= filterValue == tableContent[i][schema_i];
          }
        }
        else if (filterOperator == '>')
        {
          if (filterField.second == "int")
          {
            isValid |= filterValueI > contentValueI;
          }
          else
          {
            std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para el tipo \"" << filterField.second << "\"." << std::endl;
            return;
          }
        }
        else if (filterOperator == '<')
        {
          if (filterField.second == "int")
          {
            isValid |= filterValueI < contentValueI;
          }
          else
          {
            std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para el tipo \"" << filterField.second << "\"." << std::endl;
            return;
          }
        }
        else if (filterOperator == '!')
        {
          if (filterField.second == "int")
          {
            isValid |= filterValueI != contentValueI;
          }
          else
          {
            isValid |= filterValue != tableContent[i][schema_i];
          }
        }
      }
    }

    if (!isValid)
    {
      continue;
    }

    for (std::pair<std::pair<std::string, std::string>, std::string> &arg : query_->getArguments())
    {
      bool found = false;
      for (size_t k = 0; k < tableSchema.size(); k++)
      {
        if (tableSchema[k].first == arg.first.second)
        {
          if (tableSchema[k].second == "int")
          {
            try
            {
              std::stoi(arg.second);
            }
            catch (const std::invalid_argument &e)
            {
              std::cout << "ERROR: El valor \"" << arg.second << "\" no es de tipo \"" << tableSchema[k].second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
              return;
            }
          }
          else if (tableSchema[k].second == "bool")
          {
            try
            {
              std::transform(arg.second.begin(), arg.second.end(), arg.second.begin(), ::tolower);

              if (arg.second == "true")
              {
                arg.second = "1";
              }
              else if (arg.second == "false")
              {
                arg.second = "0";
              }
              else
              {
                int value = std::stoi(arg.second);

                if (value != 0 && value != 1)
                {
                  std::cout << "ERROR: El valor \"" << arg.second << "\" no es de tipo \"" << tableSchema[k].second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
                  return;
                }
              }
            }
            catch (const std::invalid_argument &e)
            {
              std::cout << "ERROR: El valor \"" << arg.second << "\" no es de tipo \"" << tableSchema[k].second << "\" como esperaba la tabla \"" << tableName << "\"." << std::endl;
              return;
            }
          }

          tableContent[i][k] = arg.second;

          found = true;
          break;
        }
      }

      if (!found)
      {
        std::cout << "ERROR: El campo \"" << arg.first.second << "\" no existe en la tabla \"" << tableName << "\"." << std::endl;
        return;
      }
    }
  }

  saveTableContent_(tableName, tableContent);
}

void Database::executeSelect()
{
  std::vector<std::vector<std::vector<std::string>>> allTableContent;
  std::vector<std::vector<std::pair<std::string, std::string>>> allTableSchema;
  std::vector<std::vector<std::pair<std::string, std::string>>> allTableSchemaFields;

  for (const std::string &tableName : query_->getArgumentsOf())
  {
    allTableContent.push_back(loadTableContent_(tableName));
    allTableSchema.push_back(loadTableSchema_(tableName));
    allTableSchemaFields.push_back(loadTableSchemaFields_(tableName));
  }

  std::vector<std::vector<std::string>> tableContent;
  std::vector<std::vector<std::string>> newTableContent;
  std::vector<std::string> currentCombination;
  combinateTables_(allTableContent, tableContent, currentCombination, 0);

  std::vector<std::pair<std::string, std::string>> tableSchema = combinateSchemas_(allTableSchema);
  std::vector<std::pair<std::string, std::string>> tableSchemaFields = combinateSchemas_(allTableSchemaFields);

  std::vector<Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>>> filters = query_->getFilters();
  std::vector<std::string> filtersGroup = query_->getFiltersGroup();

  for (const std::vector<std::string> &row : tableContent)
  {
    bool isValid = true;
    for (size_t i = 0; i < filters.size(); i++)
    {
      Triplet<std::pair<std::string, std::string>, char, std::pair<std::string, std::string>> filter = filters[i];
      std::string filterGroup = filtersGroup[i];
      std::pair<std::string, std::string> filterField = filter.getFirst();
      char filterOperator = filter.getSecond();
      std::pair<std::string, std::string> filterCondition = filter.getThird();

      size_t schemaField;
      size_t schemaCondition;
      bool foundSchema = false;

      for (size_t j = 0; j < tableSchemaFields.size(); j++)
      {
        if (tableSchemaFields[j].first == filterField.first && tableSchemaFields[j].second == filterField.second)
        {
          schemaField = j;
          foundSchema = true;
          break;
        }
      }

      if (!foundSchema)
      {
        std::cout << "ERROR: El campo \"" << filterField.second << "\" no existe en la tabla \"" << filterField.first << "\"." << std::endl;
        return;
      }

      foundSchema = false;

      int filterValueI;
      int contentValueI;

      if (filterField.first == filterCondition.first)
      {
        if (tableSchema[schemaField].second == "int")
        {
          try
          {
            filterValueI = std::stoi(filterCondition.second);
            contentValueI = std::stoi(row[schemaField]);
          }
          catch (const std::invalid_argument &e)
          {
            std::cout << "ERROR: El valor \"" << filterCondition.second << "\" no es de tipo \"" << tableSchema[schemaField].second << "\" como esperaba la tabla \"" << filterField.first << "\"." << std::endl;
            return;
          }
        }
        else if (tableSchema[schemaField].second == "bool")
        {
          std::string filterValue = filterCondition.second;
          try
          {
            std::transform(filterValue.begin(), filterValue.end(), filterValue.begin(), ::tolower);

            if (filterValue == "true")
            {
              filter.setThird(std::make_pair(filterCondition.first, "1"));
            }
            else if (filterValue == "false")
            {
              filter.setThird(std::make_pair(filterCondition.first, "0"));
            }
            else
            {
              int value = std::stoi(filterValue);

              if (value != 0 && value != 1)
              {
                std::cout << "ERROR: El valor \"" << filterValue << "\" no es de tipo \"" << tableSchema[schemaField].second << "\" como esperaba la tabla \"" << filterCondition.first << "\"." << std::endl;
                return;
              }
            }

            filterCondition = filter.getThird();
          }
          catch (const std::invalid_argument &e)
          {
            std::cout << "ERROR: El valor \"" << filterValue << "\" no es de tipo \"" << tableSchema[schemaField].second << "\" como esperaba la tabla \"" << filterCondition.first << "\"." << std::endl;
            return;
          }
        }

        if (filterGroup == "and")
        {
          if (filterOperator == '=')
          {
            if (filterField.second == "int")
            {
              isValid &= filterValueI == contentValueI;
            }
            else
            {
              isValid &= filterCondition.second == row[schemaField];
            }
          }
          else if (filterOperator == '>')
          {
            if (filterField.second == "int")
            {
              isValid &= filterValueI > contentValueI;
            }
            else
            {
              std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para el tipo \"" << tableSchema[schemaField].second << "\"." << std::endl;
              return;
            }
          }
          else if (filterOperator == '<')
          {
            if (filterField.second == "int")
            {
              isValid &= filterValueI < contentValueI;
            }
            else
            {
              std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para el tipo \"" << tableSchema[schemaField].second << "\"." << std::endl;
              return;
            }
          }
          else if (filterOperator == '!')
          {
            if (filterField.second == "int")
            {
              isValid &= filterValueI != contentValueI;
            }
            else
            {
              isValid &= filterCondition.second != row[schemaField];
            }
          }
        }
        else if (filterGroup == "or")
        {
          if (filterOperator == '=')
          {
            if (filterField.second == "int")
            {
              isValid |= filterValueI == contentValueI;
            }
            else
            {
              isValid |= filterCondition.second == row[schemaField];
            }
          }
          else if (filterOperator == '>')
          {
            if (filterField.second == "int")
            {
              isValid |= filterValueI > contentValueI;
            }
            else
            {
              std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para el tipo \"" << tableSchema[schemaField].second << "\"." << std::endl;
              return;
            }
          }
          else if (filterOperator == '<')
          {
            if (filterField.second == "int")
            {
              isValid |= filterValueI < contentValueI;
            }
            else
            {
              std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para el tipo \"" << tableSchema[schemaField].second << "\"." << std::endl;
              return;
            }
          }
          else if (filterOperator == '!')
          {
            if (filterField.second == "int")
            {
              isValid |= filterValueI != contentValueI;
            }
            else
            {
              isValid |= filterCondition.second != row[schemaField];
            }
          }
        }
      }
      else
      {
        for (size_t j = 0; j < tableSchemaFields.size(); j++)
        {
          if (tableSchemaFields[j].first == filterCondition.first && tableSchemaFields[j].second == filterCondition.second)
          {
            schemaCondition = j;
            foundSchema = true;
            break;
          }
        }

        if (!foundSchema)
        {
          std::cout << "ERROR: El campo \"" << filterField.second << "\" no existe en la tabla \"" << filterField.first << "\"." << std::endl;
          return;
        }

        if (tableSchema[schemaField].second != tableSchema[schemaCondition].second)
        {
          std::cout << "ERROR: El tipo de dato no es el mismo para el campo \"" << filterField.first << " (" << tableSchema[schemaField].second << ")\" y el campo \"" << filterCondition.first << " (" << tableSchema[schemaCondition].second << ")\"." << std::endl;
          return;
        }

        if (filterOperator == '=')
        {
          if (filterGroup == "and")
          {
            isValid &= row[schemaField] == row[schemaCondition];
          }
          else if (filterGroup == "or")
          {
            isValid |= row[schemaField] == row[schemaCondition];
          }
        }
        else
        {
          std::cout << "ERROR: El operador \"" << filterOperator << "\" no es soportado para los JOINs." << std::endl;
          return;
        }
      }
    }

    if (isValid)
    {
      newTableContent.push_back(row);
    }
  }

  std::vector<std::pair<std::pair<std::string, std::string>, std::string>> arguments = query_->getArguments();
  std::vector<size_t> argumentsOrder;

  std::string argumentTable;
  std::string argumentColumn;

  for (size_t i = 0; i < tableSchemaFields.size(); i++)
  {
    for (const std::pair<std::pair<std::string, std::string>, std::string> &arg : arguments)
    {
      const std::pair<std::string, std::string> &argument = arg.first;
      argumentTable = argument.first;
      argumentColumn = argument.second;

      if (argumentTable == "all")
      {
        argumentsOrder.push_back(i);
        break;
      }

      if (argumentTable == tableSchemaFields[i].first && argumentColumn == tableSchemaFields[i].second)
      {
        argumentsOrder.push_back(i);
        break;
      }
    }

    if (argumentTable == "all")
    {
      break;
    }
  }

  if (argumentTable == "all")
  {
    argumentsOrder.clear();
    for (size_t i = 0; i < tableSchemaFields.size(); i++)
    {
      argumentsOrder.push_back(i);
    }
  }

  std::vector<std::vector<std::string>> finalTableContent;
  std::vector<std::pair<std::string, std::string>> finalTableSchemaField;
  std::vector<std::pair<std::string, std::string>> finalTableSchema;

  for (size_t i = 0; i < tableSchemaFields.size(); i++)
  {
    for (const size_t &argOrder : argumentsOrder)
    {
      if (argOrder == i)
      {
        finalTableSchemaField.push_back(tableSchemaFields[i]);
        finalTableSchema.push_back(tableSchema[i]);
      }
    }
  }

  for (const std::vector<std::string> &row : newTableContent)
  {
    std::vector<std::string> finalRow;
    for (size_t i = 0; i < row.size(); i++)
    {
      for (const size_t &argOrder : argumentsOrder)
      {
        if (argOrder == i)
        {
          finalRow.push_back(row[i]);
        }
      }
    }
    finalTableContent.push_back(finalRow);
  }

  std::vector<std::string> tableSchemaNames;
  for (const auto &pair : finalTableSchemaField)
  {
    tableSchemaNames.push_back(pair.first + "_" + pair.second);
  }

  if (query_->getSaveName().empty())
  {
    printTable_(tableSchemaNames, finalTableContent);
  }
  else
  {
    std::vector<std::vector<std::string>> tableSchemasTotal = loadTableSchemas_();
    for (const std::vector<std::string> &schema : tableSchemasTotal)
    {
      if (schema[0] == query_->getSaveName())
      {
        std::cout << "La tabla \"" << query_->getSaveName() << "\" ya existe." << std::endl;
        return;
      }
    }
    std::vector<std::string> tableSchemaCurrent;
    tableSchemaCurrent.push_back(query_->getSaveName());

    for (size_t i = 0; i < tableSchemaNames.size(); i++)
    {
      tableSchemaCurrent.push_back(tableSchemaNames[i]);
      tableSchemaCurrent.push_back(finalTableSchema[i].second);
    }

    tableSchemasTotal.push_back(tableSchemaCurrent);
    saveTableSchemas_(query_->getSaveName(), tableSchemasTotal);
    saveTableContent_(query_->getSaveName(), finalTableContent);
  }
}

std::vector<std::vector<std::string>> Database::loadTableSchemas_()
{
  std::vector<std::vector<std::string>> tableSchemas;

  std::ifstream schemeFile(databaseFolder_ + "/schema.txt");

  std::string schemeLine;
  while (std::getline(schemeFile, schemeLine))
  {
    std::vector<std::string> tableSchema;
    std::stringstream schemeStream(schemeLine);
    std::string field;

    while (std::getline(schemeStream, field, '#'))
    {
      tableSchema.push_back(field);
    }

    tableSchemas.push_back(tableSchema);
  }

  schemeFile.close();

  return tableSchemas;
}

std::vector<std::pair<std::string, std::string>> Database::loadTableSchema_(const std::string &tableName)
{
  std::vector<std::vector<std::string>> tableSchemas = loadTableSchemas_();
  std::vector<std::pair<std::string, std::string>> tableSchema;

  for (const std::vector<std::string> &tableSchemaIter : tableSchemas)
  {
    if (tableSchemaIter[0] == tableName)
    {
      for (size_t i = 1; i < tableSchemaIter.size(); i += 2)
      {
        tableSchema.push_back(std::make_pair(tableSchemaIter[i], tableSchemaIter[i + 1]));
      }
      break;
    }
  }
  return tableSchema;
}

std::vector<std::pair<std::string, std::string>> Database::loadTableSchemaFields_(const std::string &tableName)
{
  std::vector<std::vector<std::string>> tableSchemas = loadTableSchemas_();
  std::vector<std::pair<std::string, std::string>> tableSchema;

  for (const std::vector<std::string> &tableSchemaIter : tableSchemas)
  {
    if (tableSchemaIter[0] == tableName)
    {
      for (size_t i = 1; i < tableSchemaIter.size(); i += 2)
      {
        tableSchema.push_back(std::make_pair(tableName, tableSchemaIter[i]));
      }
      break;
    }
  }
  return tableSchema;
}

std::vector<std::vector<std::string>> Database::loadTableContent_(const std::string &tableName)
{
  std::vector<std::vector<std::string>> tableContent;

  std::ifstream tableFile(databaseFolder_ + "/" + tableName + ".txt");

  std::string tableLine;
  while (std::getline(tableFile, tableLine))
  {
    std::vector<std::string> tableContentLine;
    std::stringstream tableStream(tableLine);
    std::string field;

    while (std::getline(tableStream, field, '#'))
    {
      tableContentLine.push_back(field);
    }

    tableContent.push_back(tableContentLine);
  }

  tableFile.close();

  return tableContent;
}

void Database::saveTableSchemas_(const std::string &tableName, const std::vector<std::vector<std::string>> &tableSchemas)
{
  std::ofstream schemeFile(databaseFolder_ + "/schema.txt");

  for (const std::vector<std::string> &tableSchema : tableSchemas)
  {
    for (size_t i = 0; i < tableSchema.size(); i++)
    {
      if (i == 0)
      {
        schemeFile << tableSchema[i];
      }
      else
      {
        schemeFile << "#" << tableSchema[i];
      }
    }
    schemeFile << std::endl;
  }

  schemeFile.close();

  std::ofstream tableFile(databaseFolder_ + "/" + tableName + ".txt");
  tableFile.close();

  std::cout << "La tabla \"" << tableName << "\" ha sido creada." << std::endl;
}

void Database::saveTableContent_(const std::string &tableName, const std::vector<std::vector<std::string>> &tableContent)
{
  std::ofstream tableFile(databaseFolder_ + "/" + tableName + ".txt");

  for (const std::vector<std::string> &tableContentLine : tableContent)
  {
    for (size_t i = 0; i < tableContentLine.size(); i++)
    {
      if (i == 0)
      {
        tableFile << tableContentLine[i];
      }
      else
      {
        tableFile << "#" << tableContentLine[i];
      }
    }
    tableFile << std::endl;
  }

  tableFile.close();

  std::cout << "El contenido de la tabla \"" << tableName << "\" ha sido guardado." << std::endl;
}

void Database::printTable_(const std::vector<std::string> tableSchema, const std::vector<std::vector<std::string>> &tableContent)
{
  for (const std::string &field : tableSchema)
  {
    std::cout << field << "\t";
  }
  std::cout << std::endl;

  for (const std::vector<std::string> &tableContentLine : tableContent)
  {
    for (const std::string &field : tableContentLine)
    {
      std::cout << field << "\t";
    }
    std::cout << std::endl;
  }
}

void Database::combinateTables_(const std::vector<std::vector<std::vector<std::string>>> &allTableContent, std::vector<std::vector<std::string>> &tableContent, std::vector<std::string> &currentCombination, size_t depth)
{
  if (depth == allTableContent.size())
  {
    tableContent.push_back(currentCombination);
    return;
  }

  for (const auto &row : allTableContent[depth])
  {
    std::vector<std::string> tempCombination = currentCombination;
    tempCombination.insert(tempCombination.end(), row.begin(), row.end());
    combinateTables_(allTableContent, tableContent, tempCombination, depth + 1);
  }
}

std::vector<std::pair<std::string, std::string>> Database::combinateSchemas_(const std::vector<std::vector<std::pair<std::string, std::string>>> &allTableSchema)
{
  std::vector<std::pair<std::string, std::string>> tableSchema;
  for (const std::vector<std::pair<std::string, std::string>> &schema : allTableSchema)
  {
    for (const std::pair<std::string, std::string> &pair : schema)
    {
      tableSchema.push_back(pair);
    }
  }

  return tableSchema;
}
