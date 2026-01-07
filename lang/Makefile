# Nome do compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Ferramentas
FLEX = flex
BISON = bison

# Diretórios
SRC_DIR = src
BUILD_DIR = build

# Arquivos gerados
LEX_CPP = $(BUILD_DIR)/lexer.cpp
PARSER_CPP = $(BUILD_DIR)/parser.cpp
PARSER_HPP = $(BUILD_DIR)/parser.hpp

# Executável final
TARGET = compiler

# Fontes C++
SRCS = \
    $(LEX_CPP) \
    $(PARSER_CPP) \
    $(SRC_DIR)/main.cpp

OBJS = $(SRCS:.cpp=.o)

# Regra padrão
all: $(BUILD_DIR) $(TARGET)

# Cria diretório build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Gera parser.cpp e parser.hpp
$(PARSER_CPP) $(PARSER_HPP): $(SRC_DIR)/parser.y
	$(BISON) -d -o $(PARSER_CPP) $<

# Gera lexer.cpp (DEPENDE do parser.hpp)
$(LEX_CPP): $(SRC_DIR)/lexer.l $(PARSER_HPP)
	$(FLEX) -o $@ $<

# Linkagem
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilação
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(BUILD_DIR) -c $< -o $@

# Limpeza
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
