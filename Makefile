# Nombre del compilador
CXX = g++

# Flags de compilación
CXXFLAGS = -Wall -g -I./src

# Directorios
SRC_DIR = src
OBJ_DIR = obj

# Encuentra todos los archivos .cpp en el directorio src
SRC_FILES = main.cpp $(wildcard $(SRC_DIR)/*.cpp)

# Genera archivos objeto correspondientes
OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Nombre del ejecutable
EXEC = SQLito

# Regla para compilar el ejecutable
$(EXEC): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_FILES)

# Regla para compilar archivos .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpia archivos intermedios y ejecutable
clean:
	rm -f $(OBJ_FILES) $(EXEC)

# PHONY asegura que las reglas no sean confundidas con archivos
.PHONY: clean
