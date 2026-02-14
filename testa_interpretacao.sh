#graziele de cassia rodrigues 21.1.8120

#chmod +x testa_interpretacao.sh
# ./testa_interpretacao.sh

-

#!/bin/bash
# Graziele de Cassia Rodrigues - 21.1.8120

# chmod +x testa_interpretacao.sh
# ./testa_interpretacao.sh

COMPILER=./compiler

DIRS=(
  "testes/semantica/certo/simple"
  "testes/semantica/certo/function"
  "testes/semantica/certo/full"
)

LOGDIR="logs_interpretacao"

mkdir -p "$LOGDIR/out" "$LOGDIR/err"
FAILED="$LOGDIR/falhas_interpretacao.txt"
> "$FAILED"

shopt -s nullglob

total_pass=0
total_fail=0
total_timeout=0

TIMEOUT_SECS=3

# entrada padrão para testes que chamam read()
DEFAULT_INPUT="1 2 3 4 5 6 7 8 9 10
"

for DIR in "${DIRS[@]}"; do
  echo
  echo "=== Testando INTERPRETAÇÃO (-i) em $DIR ==="

  files=("$DIR"/*.lan)

  if [ ${#files[@]} -eq 0 ]; then
    echo "Nenhum arquivo .lan encontrado em $DIR"
    continue
  fi

  pass=0
  fail=0

  for file in "${files[@]}"; do
    base="$(basename "$file" .lan)"
    out="$LOGDIR/out/$base.out"
    err="$LOGDIR/err/$base.err"

    echo "----------------------------------"
    echo "Arquivo: $base.lan"

    # Se o teste usa read(), injeta stdin; senão fecha stdin.
    if grep -q "read()" "$file"; then
      printf "%s" "$DEFAULT_INPUT" | timeout "${TIMEOUT_SECS}s" "$COMPILER" -i "$file" >"$out" 2>"$err"
    else
      timeout "${TIMEOUT_SECS}s" "$COMPILER" -i "$file" </dev/null >"$out" 2>"$err"
    fi

    code=$?

    if [ $code -eq 124 ]; then
      echo "TIMEOUT (>${TIMEOUT_SECS}s) -> $err"
      echo "$file (TIMEOUT)" >> "$FAILED"
      fail=$((fail+1))
      total_fail=$((total_fail+1))
      total_timeout=$((total_timeout+1))

    elif [ $code -eq 0 ]; then
      echo "OK (exit=0) -> $out"
      pass=$((pass+1))
      total_pass=$((total_pass+1))
      [ -s "$err" ] || rm -f "$err"

    else
      echo "FALHOU (exit=$code) -> $err"
      echo "$file (exit=$code)" >> "$FAILED"
      fail=$((fail+1))
      total_fail=$((total_fail+1))
    fi
  done

  echo "Resumo em $DIR:"
  echo "  Passaram: $pass"
  echo "  Falharam: $fail"
done

echo
echo "=================================="
echo "TOTAL GERAL"
echo "Passaram: $total_pass"
echo "Falharam: $total_fail"
echo "  Timeouts: $total_timeout"

if [ -s "$FAILED" ]; then
  echo
  echo "Lista de falhas: $FAILED"
  cat "$FAILED"
else
  echo "Todos os testes passaram com sucesso!"
  rm -f "$FAILED"
fi
