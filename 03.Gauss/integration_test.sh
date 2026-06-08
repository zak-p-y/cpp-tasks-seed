#!/bin/bash

status=true
tmpdir=$(mktemp -d /tmp/gauss-itest.XXXXXX)

cleanup() {
  rm -rf "$tmpdir"
}
trap cleanup EXIT

python3 - "$tmpdir" <<'PY'
import csv
import os
import random
import sys

tmpdir = sys.argv[1]


def write_matrix_csv(path, header, rows):
    with open(path, "w", newline="") as f:
        writer = csv.writer(f)
        if header is not None:
            writer.writerow(header)
        writer.writerows(rows)


def write_vector_csv(path, vector, prec=6):
    with open(path, "w", newline="") as f:
        for idx, value in enumerate(vector):
            f.write(f"{value:.{prec}f}")
            if idx + 1 < len(vector):
                f.write("\n")


def build_case(prefix, matrix, solution):
    input_path = os.path.join(tmpdir, f"{prefix}_input.csv")
    expected_path = os.path.join(tmpdir, f"{prefix}_expected.csv")
    header = [f"X{i + 1}" for i in range(len(solution))] + ["B"]
    rows = [row + [rhs] for row, rhs in matrix]
    write_matrix_csv(input_path, header, rows)
    write_vector_csv(expected_path, solution)


# Small exact case.
small_matrix = [
    ([2.0, 1.0], 5.0),
    ([1.0, -1.0], 1.0),
]
small_solution = [2.0, 1.0]
build_case("small", small_matrix, small_solution)

# Larger reproducible case.
rng = random.Random(20240608)
n = 12
solution = [float(rng.randint(-4, 4)) for _ in range(n)]
matrix = []
for row in range(n):
    coeffs = [0.0] * n
    total = 0.0
    for col in range(n):
        if col == row:
            continue
        value = float(rng.randint(-5, 5))
        coeffs[col] = value
        total += abs(value)
    coeffs[row] = total + 10.0 + float(rng.randint(0, 5))
    rhs = sum(coeffs[col] * solution[col] for col in range(n))
    matrix.append((coeffs, rhs))

build_case("large", matrix, solution)
PY

run_case() {
  local input_file=$1
  local expected_file=$2
  local actual_file=$3

  if ! ./gauss "$input_file" >"$actual_file"; then
    echo "gauss failed for $input_file" >&2
    status=false
    return
  fi

  if cmp -s "$expected_file" "$actual_file"; then
    echo "$(basename "$input_file") ok"
  else
    echo "$(basename "$input_file") failed" >&2
    status=false
  fi
}

run_case "$tmpdir/small_input.csv" "$tmpdir/small_expected.csv" "$tmpdir/small_actual.csv"
run_case "$tmpdir/large_input.csv" "$tmpdir/large_expected.csv" "$tmpdir/large_actual.csv"

$status
