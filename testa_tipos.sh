#!/bin/bash
# Graziele de Cassia Rodrigues - 21.1.8120
#
# chmod +x testa_tipos.sh
# ./testa_tipos.sh

COMPILER=./compiler

DIRS=(
  "testes/tipos/simple"
  "testes/tipos/function"
  "testes/tipos/full"
  "testes/tipos/certo"
  "testes/tipos/errado"
)

LOGDIR="logs_tipos"
mkdir -p "$LOGDIR/out" "$LOGDIR/err"

FAILED="$LOGDIR/falhas_tipos.txt"
> "$FAILED"

shopt -s nullglob

total_pass=0
total_fail=0
total_timeout=0

TIMEOUT_SECS=3

for DIR in "${DIRS[@]}"; do
  echo
  echo "=== Testando TIPOS (-ty) em $DIR ==="

  files=("$DIR"/*.lan)

  if [ ${#files[@]} -eq 0 ]; then
    echo "Nenhum arquivo .lan encontrado em $DIR"
    continue
  fi

  pass=0
  fail=0

  for file in "${files[@]}"; do
    # cria um "id" único por caminho, evitando sobrescrever logs
    rel="${file%.lan}"
    safe="${rel//\//_}"  # troca / por _
    out="$LOGDIR/out/${safe}.out"
    err="$LOGDIR/err/${safe}.err"

    echo "----------------------------------"
    echo "Arquivo: $file"

    # Executa e:
    # - mostra stdout (well typed / no typed) no terminal
    # - salva stdout em $out
    # - salva stderr em $err
    timeout "${TIMEOUT_SECS}s" "$COMPILER" -ty "$file" </dev/null \
      | tee "$out" 2>"$err"

    # OBS: com pipe, o exit code do compiler vem de PIPESTATUS[0]
    code=${PIPESTATUS[0]}

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
echo "TOTAL GERAL (TIPOS -ty)"
echo "Passaram: $total_pass"
echo "Falharam: $total_fail"
echo "  Timeouts: $total_timeout"

if [ -s "$FAILED" ]; then
  echo
  echo "Lista de falhas: $FAILED"
  cat "$FAILED"
else
  echo "Todos os testes de tipos passaram com sucesso!"
  rm -f "$FAILED"
fi