## Compilador da Linguagem lang2

**Disciplina:** Compiladores
**Ano/Semestre:** 2025/2
**Aluno:** Graziele de Cassia Rodrigues
**Matrícula:** 21.1.8120

## 📌 Descrição do Projeto

Este projeto implementa um compilador para a linguagem **lang2**, conforme especificação fornecida na disciplina de Compiladores.
A implementação contempla:

* Análise léxica (Flex)
* Análise sintática (Bison em C++)
* Definição completa da gramática da linguagem
* Interface de linha de comando conforme especificação


## 🛠️ Ferramentas Utilizadas

* **Flex** — geração do analisador léxico
* **Bison (C++)** — geração do analisador sintático
* **G++ (C++17)** — compilação do projeto
* **Make** — automação do processo de build

## 📂 Estrutura do Projeto

```
.
├── src/
│   ├── lexer.l        # Especificação léxica (Flex)
│   ├── parser.y       # Gramática sintática (Bison)
│   └── main.cpp       # Programa principal
├── build/
│   ├── lexer.cpp      # Código gerado pelo Flex
│   ├── parser.cpp     # Código gerado pelo Bison
│   └── parser.hpp     # Interface do parser
├── Makefile
└── README.md
```


## ⚙️ Compilação

Para compilar o projeto, basta executar na raiz do diretório:

```bash
make
```

O comando irá gerar o executável:

```bash
./compiler
```

---

## ▶️ Execução

### Execução padrão (análise sintática)

```bash
./compiler arquivo.lang2
```

O compilador realiza a análise sintática do arquivo informado e imprime o resultado da análise.

