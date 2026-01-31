#graziele de cassia rodrigues 21.1.8120

# chmod +x testa_sintaxe_certo.sh
#!/bin/bash

COMPILER=./compiler
DIR=testes/sintaxe/certo
FAILED="falhas_sintaxe_certo.txt"

# limpa arquivo de falhas
> "$FAILED"

echo "=== Testando arquivos em $DIR ==="

shopt -s nullglob
files=("$DIR"/*.lan)

if [ ${#files[@]} -eq 0 ]; then
    echo "Nenhum arquivo .lan encontrado"
    exit 1
fi

for file in "${files[@]}"; do
    echo "----------------------------------"
    echo "Arquivo: $(basename "$file")"

    if "$COMPILER" -syn "$file" >/dev/null 2>&1; then
        echo "accepted"
    else
        echo "rejected"
        echo "$file" >> "$FAILED"
    fi
done

echo "=================================="

if [ -s "$FAILED" ]; then
    echo "Arquivos que FALHARAM:"
    cat "$FAILED"
else
    echo "Testes passaram com sucesso!"
    rm "$FAILED"
fi
