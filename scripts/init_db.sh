#!/usr/bin/env bash

# Initialize transport SQLite DB. This script prefers to call the Python helper
# `scripts/init_db.py` when available (it provides better error handling).

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DB_FILE_DEFAULT="$ROOT/data/transport.db"
SCHEMA_FILE_DEFAULT="$ROOT/data/schema.sql"
SEED_DIR_DEFAULT="$ROOT/data/seed"

DB_FILE="$DB_FILE_DEFAULT"
SCHEMA_FILE="$SCHEMA_FILE_DEFAULT"
SEEDS="$SEED_DIR_DEFAULT/stops_seed.sql,$SEED_DIR_DEFAULT/routes_seed.sql"
FORCE=0
USE_PY=1

usage() {
    cat <<EOF
Usage: $(basename "$0") [--db PATH] [--schema PATH] [--seeds file1,file2] [--force] [--no-python]

Options:
  --db PATH        Path to sqlite DB file (default: data/transport.db)
  --schema PATH    Path to schema SQL file (default: data/schema.sql)
  --seeds LIST     Comma-separated seed SQL files (default: data/seed/*.sql)
  --force          Remove existing DB before creating
  --no-python      Do not use Python helper, run sqlite3 directly
  -h, --help       Show this help
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --db)
            DB_FILE="$2"; shift 2;;
        --schema)
            SCHEMA_FILE="$2"; shift 2;;
        --seeds)
            SEEDS="$2"; shift 2;;
        --force)
            FORCE=1; shift;;
        --no-python)
            USE_PY=0; shift;;
        -h|--help)
            usage; exit 0;;
        *)
            echo "Unknown option: $1" >&2; usage; exit 1;;
    esac
done

echo "Initializing DB: ${DB_FILE}"

mkdir -p "$(dirname "$DB_FILE")"

if [ -f "$DB_FILE" ]; then
    if [ "$FORCE" -eq 1 ]; then
        echo "Removing existing DB..."
        rm -f "$DB_FILE"
    else
        echo "DB already exists: $DB_FILE (use --force to recreate)"
        exit 0
    fi
fi

# If python helper exists and user didn't disable it, prefer it
if [ "$USE_PY" -eq 1 ] && command -v python3 >/dev/null 2>&1 && [ -f "$ROOT/scripts/init_db.py" ]; then
    echo "Using Python helper to create DB"
    ARGS=("$ROOT/scripts/init_db.py" --db "$DB_FILE" --schema "${SCHEMA_FILE:-$SCHEMA_FILE_DEFAULT}")
    IFS=',' read -r -a seed_arr <<< "$SEEDS"
    for s in "${seed_arr[@]}"; do
        ARGS+=(--seeds "$s")
    done
    if [ "$FORCE" -eq 1 ]; then ARGS+=(--force); fi
    python3 "${ARGS[@]}"
    exit $?
fi

# Fall back to sqlite3 CLI
if ! command -v sqlite3 >/dev/null 2>&1; then
    echo "ERROR: sqlite3 CLI not found and Python helper not available" >&2
    exit 2
fi

if [ ! -f "$SCHEMA_FILE" ]; then
    echo "ERROR: schema file not found: $SCHEMA_FILE" >&2
    exit 3
fi

echo "Applying schema: $SCHEMA_FILE"
sqlite3 "$DB_FILE" < "$SCHEMA_FILE"

IFS=',' read -r -a seed_arr <<< "$SEEDS"
for s in "${seed_arr[@]}"; do
    if [ -f "$s" ]; then
        echo "Applying seed: $s"
        sqlite3 "$DB_FILE" < "$s"
    else
        echo "Warning: seed file not found, skipping: $s" >&2
    fi
done

echo "DB initialized: $DB_FILE"
echo "Stats:"
sqlite3 "$DB_FILE" "SELECT 'Stops: ' || COUNT(*) FROM stops;"
sqlite3 "$DB_FILE" "SELECT 'Routes: ' || COUNT(*) FROM routes;"
sqlite3 "$DB_FILE" "SELECT 'Trips: ' || COUNT(*) FROM trips;"