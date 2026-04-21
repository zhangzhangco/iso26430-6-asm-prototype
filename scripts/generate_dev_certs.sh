#!/bin/zsh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
OUT_DIR="$ROOT_DIR/fixtures/certs/dev"

rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

openssl req -x509 -newkey rsa:2048 -sha256 -nodes \
  -keyout "$OUT_DIR/ca.key.pem" \
  -out "$OUT_DIR/ca.cert.pem" \
  -days 3650 \
  -subj "/CN=ASM Dev CA"

openssl req -newkey rsa:2048 -nodes \
  -keyout "$OUT_DIR/server.key.pem" \
  -out "$OUT_DIR/server.csr.pem" \
  -subj "/CN=ASM Server"
openssl x509 -req -in "$OUT_DIR/server.csr.pem" \
  -CA "$OUT_DIR/ca.cert.pem" -CAkey "$OUT_DIR/ca.key.pem" -CAcreateserial \
  -out "$OUT_DIR/server.cert.pem" -days 3650 -sha256

openssl req -newkey rsa:2048 -nodes \
  -keyout "$OUT_DIR/client.key.pem" \
  -out "$OUT_DIR/client.csr.pem" \
  -subj "/CN=ASM Client"
openssl x509 -req -in "$OUT_DIR/client.csr.pem" \
  -CA "$OUT_DIR/ca.cert.pem" -CAkey "$OUT_DIR/ca.key.pem" -CAcreateserial \
  -out "$OUT_DIR/client.cert.pem" -days 3650 -sha256

openssl req -x509 -newkey rsa:2048 -sha256 -nodes \
  -keyout "$OUT_DIR/rogue-ca.key.pem" \
  -out "$OUT_DIR/rogue-ca.cert.pem" \
  -days 3650 \
  -subj "/CN=ASM Rogue CA"

openssl req -newkey rsa:2048 -nodes \
  -keyout "$OUT_DIR/rogue-client.key.pem" \
  -out "$OUT_DIR/rogue-client.csr.pem" \
  -subj "/CN=ASM Rogue Client"
openssl x509 -req -in "$OUT_DIR/rogue-client.csr.pem" \
  -CA "$OUT_DIR/rogue-ca.cert.pem" -CAkey "$OUT_DIR/rogue-ca.key.pem" -CAcreateserial \
  -out "$OUT_DIR/rogue-client.cert.pem" -days 3650 -sha256

rm -f "$OUT_DIR"/*.csr.pem "$OUT_DIR"/*.srl
echo "generated fixtures in $OUT_DIR"
