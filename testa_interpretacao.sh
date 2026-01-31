#graziele de cassia rodrigues 21.1.8120

#chmod +x testa_interpretacao.sh
#!/bin/bash

COMPILER=./compiler
DIR="testes/sintaxe/certo"  
LOGDIR="logs_interpretacao"

mkdir -p "$LOGDIR/out" "$LOGDIR/err"
FAILED="$LOGDIR/falhas_interpretacao.txt"
> "$FAILED"

echo "=== Testando INTERPRETAÇÃO (-i) em $DIR ==="

shopt -s nullglob
files=("$DIR"/*.lan)

if [ ${#files[@]} -eq 0 ]; then
    echo "Nenhum arquivo .lan encontrado em $DIR"
    exit 1
fi

pass=0
fail=0

for file in "${files[@]}"; do
    base="$(basename "$file" .lan)"
    out="$LOGDIR/out/$base.out"
    err="$LOGDIR/err/$base.err"

    echo "----------------------------------"
    echo "Arquivo: $base.lan"

    # roda e captura stdout/stderr
    "$COMPILER" -i "$file" >"$out" 2>"$err"
    code=$?

    if [ $code -eq 0 ]; then
        echo "OK (exit=$code) -> $out"
        pass=$((pass+1))
        # se não teve erro, limpa o arquivo de erro
        [ -s "$err" ] || rm -f "$err"
    else
        echo "FALHOU (exit=$code) -> $err"
        echo "$file (exit=$code)" >> "$FAILED"
        fail=$((fail+1))
    fi
done

echo "=================================="
echo "Passaram: $pass"
echo "Falharam: $fail"

if [ -s "$FAILED" ]; then
    echo
    echo "Lista de falhas: $FAILED"
    cat "$FAILED"
else
    echo "Todos os testes passaram com sucesso!"
    rm -f "$FAILED"
fi
