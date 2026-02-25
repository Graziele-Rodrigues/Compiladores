#graziele de cassia rodrigues 21.1.8120
# chmod +x testa_sintaxe_errado.sh
# ./testa_sintaxe_errado.sh

COMPILER=./compiler
DIR=testes/sintaxe/errado
FAILED="falhas_sintaxe_errado.txt"

# limpa arquivo de falhas
> "$FAILED"

echo "=== Testando arquivos em $DIR (esperado: REJEITAR) ==="

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
        echo "ERRO: accepted"
        echo "$file" >> "$FAILED"
    else
        echo "OK: rejected"
    fi
done

echo "=================================="

if [ -s "$FAILED" ]; then
    echo "Arquivos incorretos que foram ACEITOS indevidamente:"
    cat "$FAILED"
else
    echo "Testes passaram com sucesso!"
    rm "$FAILED"
fi
