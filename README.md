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


## 📦 Requisitos e Instalação

Para compilar e executar o compilador da linguagem **lang2**, é necessário que os seguintes pacotes estejam instalados no sistema:

* **Flex**
* **Bison**
* **Compilador C++ (G++ com suporte ao padrão C++17)**
* **Make**

### Instalação em sistemas Linux baseados em Debian/Ubuntu

Execute os comandos abaixo no terminal:

```bash
sudo apt update
sudo apt install flex bison build-essential
```

O pacote **build-essential** inclui o `g++`, o `make` e bibliotecas padrão necessárias para a compilação do projeto.

### Verificação da instalação

Após a instalação, é possível verificar se as ferramentas estão disponíveis executando:

```bash
flex --version
bison --version
g++ --version
make --version
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


## ▶️ Execução

### Execução – versão

```bash
./compiler -v
```

### Execução – análise sintática

```bash
./compiler -syn arquivo.lang2
```

### Execução – interpretador

```bash
./compiler -i arquivo.lang2
```
