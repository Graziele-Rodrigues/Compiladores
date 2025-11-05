# Trabalho de Compiladores - Analisador Léxico
# nome: Graziele de Cassia Rodrigues
# matricula: 21.1.8120
# data: 11/2025


# Diretório de build
BUILD_DIR := Build

# Compiladores
CC := g++
LEX := flex++

# Fontes
SRC := main.cpp parser.cpp
LEX_SRC := lexer.l

# Arquivos gerados
LEX_GEN := $(BUILD_DIR)/lex.yy.cc
OBJS := $(addprefix $(BUILD_DIR)/, main.o parser.o lex.yy.o)
EXEC := $(BUILD_DIR)/main

# Alvo padrão
all: $(EXEC)

# Cria o diretório de build se não existir
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compila o executável
$(EXEC): $(OBJS)
	$(CC) $^ -o $@

# Compilação dos arquivos .cpp
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CC) -c -std=c++17 $< -o $@

# Compilação do arquivo gerado pelo flex
$(BUILD_DIR)/lex.yy.o: $(LEX_GEN) tokens.h | $(BUILD_DIR)
	$(CC) -c -std=c++17 -I. $(LEX_GEN) -o $@

# Geração do código do Flex (dentro de Build)
$(LEX_GEN): $(LEX_SRC) tokens.h | $(BUILD_DIR)
	$(LEX) -o $@ $<

# Limpeza
clean:
	rm -rf $(BUILD_DIR)

# Execução
run: all
	$(EXEC) < teste1.txt

.PHONY: all clean run
