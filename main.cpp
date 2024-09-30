#include <iostream>

#include "src/database.h"

void clearConsole()
{
#ifdef _WIN32
  system("cls");
#elif __linux__ || __APPLE__
  system("clear");
#else
  std::cout << "No se puede limpiar la consola en este sistema operativo." << std::endl;
#endif
}

void printBanner()
{
  std::cout << std::endl;
  std::cout << "  /$$$$$$   /$$$$$$  /$$       /$$   /$$              " << std::endl;
  std::cout << " /$$__  $$ /$$__  $$| $$      |__/  | $$              " << std::endl;
  std::cout << "| $$  \\__/| $$  \\ $$| $$       /$$ /$$$$$$    /$$$$$$ " << std::endl;
  std::cout << "|  $$$$$$ | $$  | $$| $$      | $$|_  $$_/   /$$__  $$" << std::endl;
  std::cout << " \\____  $$| $$  | $$| $$      | $$  | $$    | $$  \\ $$" << std::endl;
  std::cout << " /$$  \\ $$| $$/$$ $$| $$      | $$  | $$ /$$| $$  | $$" << std::endl;
  std::cout << "|  $$$$$$/|  $$$$$$/| $$$$$$$$| $$  |  $$$$/|  $$$$$$/" << std::endl;
  std::cout << " \\______/  \\____ $$$|________/|__/   \\___/   \\______/ " << std::endl;
  std::cout << "                \\__/                                  " << std::endl;
  std::cout << std::endl;
  std::cout << "SQLito v1.0.0" << std::endl;
  std::cout << "By: Fabricio Huaquisto" << std::endl;
  std::cout << std::endl;
  std::cout << "Escribe \"exit\" para salir." << std::endl;
  std::cout << std::endl;
}

int main()
{
  Database db = Database();

  clearConsole();
  printBanner();

  std::string query;

  while (true)
  {
    std::cout << "$:: ";
    std::getline(std::cin, query);

    if (query == "exit")
    {
      break;
    }

    db.setQuery(query);
    db.execute();

    std::cout << std::endl;
  }

  return 0;
}