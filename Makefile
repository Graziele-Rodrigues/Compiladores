# Nome do compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Ferramentas
FLEX = flex
BISON = bison

# Diretórios
SRC_DIR   = src
BUILD_DIR = build

# Arquivos gerados pelo Flex/Bison
LEX_CPP     = $(BUILD_DIR)/lexer.cpp
PARSER_CPP  = $(BUILD_DIR)/parser.cpp
PARSER_HPP  = $(BUILD_DIR)/parser.hpp

# Executável final
TARGET = compiler


CPP_SRCS = \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/ast/interpreter_call.cpp \
    $(SRC_DIR)/ast/interpreter_cmd.cpp \
    $(SRC_DIR)/ast/interpreter_expr.cpp \
    $(SRC_DIR)/ast/interpreter_lvalue.cpp \
    $(SRC_DIR)/ast/interpreter_new.cpp \
    $(SRC_DIR)/ast/interpreter_print.cpp \
    $(SRC_DIR)/ast/interpreter_program.cpp

GEN_SRCS = \
    $(LEX_CPP) \
    $(PARSER_CPP)

SRCS = $(CPP_SRCS) $(GEN_SRCS)


OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SRCS)) \
       $(patsubst $(BUILD_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(GEN_SRCS))

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR) $(BUILD_DIR)/ast

# ---------- Flex / Bison ----------

$(PARSER_CPP) $(PARSER_HPP): $(SRC_DIR)/parser.y | $(BUILD_DIR)
	$(BISON) -d -o $(PARSER_CPP) $<

$(LEX_CPP): $(SRC_DIR)/lexer.l $(PARSER_HPP) | $(BUILD_DIR)
	$(FLEX) -o $@ $<

# ---------- Compilação ----------

$(BUILD_DIR)/%.o: $(BUILD_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(BUILD_DIR) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(PARSER_HPP)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -I$(BUILD_DIR) -c $< -o $@


# ---------- Link ----------

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^


clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
