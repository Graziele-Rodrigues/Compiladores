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

### Execução versao 

```bash
./compiler -v
```
### Execução análise sintática

```bash
./compiler -syn arquivo.lang2
```

### Execução interpretador

```bash
./compiler -i arquivo.lang2
```

