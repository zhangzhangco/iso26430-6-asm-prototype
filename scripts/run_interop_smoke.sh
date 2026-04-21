#!/bin/zsh
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
TMP_DIR="$ROOT_DIR/tmp/interop-smoke"
RESPONDER_BIN="$BUILD_DIR/asm_responder"
INITIATOR_BIN="$BUILD_DIR/asm_initiator"

CERT_DIR="$ROOT_DIR/fixtures/certs/dev"
EVENTS_FILE="$ROOT_DIR/fixtures/events/default-events.jsonl"
KEYS_FILE="$TMP_DIR/keys.jsonl"
RESPONDER_LOG="$TMP_DIR/responder.log"
REPORT_FILE="$TMP_DIR/report.txt"

mkdir -p "$TMP_DIR"
rm -f "$KEYS_FILE" "$RESPONDER_LOG" "$REPORT_FILE"

if [[ ! -x "$RESPONDER_BIN" || ! -x "$INITIATOR_BIN" ]]; then
  echo "missing binaries under $BUILD_DIR; run: cmake -S . -B build && cmake --build build" >&2
  exit 1
fi

cleanup() {
  if [[ -n "${RESPONDER_PID:-}" ]]; then
    kill -INT "$RESPONDER_PID" >/dev/null 2>&1 || true
    wait "$RESPONDER_PID" >/dev/null 2>&1 || true
  fi
}
trap cleanup EXIT INT TERM

"$RESPONDER_BIN" \
  "$CERT_DIR/server.cert.pem" \
  "$CERT_DIR/server.key.pem" \
  "$CERT_DIR/ca.cert.pem" \
  "$EVENTS_FILE" \
  "$KEYS_FILE" >"$RESPONDER_LOG" 2>&1 &
RESPONDER_PID=$!

CLIENT_ARGS=(
  "127.0.0.1"
)
CERT_ARGS=(
  "$CERT_DIR/client.cert.pem"
  "$CERT_DIR/client.key.pem"
  "$CERT_DIR/ca.cert.pem"
)

ready=0
for _ in {1..20}; do
  if "$INITIATOR_BIN" "${CLIENT_ARGS[@]}" get-time "${CERT_ARGS[@]}" >/dev/null 2>&1; then
    ready=1
    break
  fi
  sleep 0.2
done

if [[ "$ready" -ne 1 ]]; then
  echo "responder did not become ready" >&2
  exit 1
fi

run_step() {
  local label="$1"
  shift
  echo "## $label" >>"$REPORT_FILE"
  echo '$' "$INITIATOR_BIN" "${CLIENT_ARGS[@]}" "$@" >>"$REPORT_FILE"
  "$INITIATOR_BIN" "${CLIENT_ARGS[@]}" "$@" >>"$REPORT_FILE"
  echo >>"$REPORT_FILE"
}

run_step "GetTime" get-time "${CERT_ARGS[@]}"
run_step "QuerySPB" query-spb "${CERT_ARGS[@]}"
run_step "GetEventList" get-event-list 1776758400 1776759000 "${CERT_ARGS[@]}"
run_step "GetEventID" get-event-id 1002 "${CERT_ARGS[@]}"
run_step "LEKeyLoad" le-key-load 7101 00112233445566778899aabbccddeeff 3600 123456789 "${CERT_ARGS[@]}"
run_step "LEKeyQueryAll" le-key-query-all "${CERT_ARGS[@]}"
run_step "LEKeyPurgeAll" le-key-purge-all "${CERT_ARGS[@]}"

{
  echo "## Responder Log"
  cat "$RESPONDER_LOG"
} >>"$REPORT_FILE"

cat "$REPORT_FILE"
