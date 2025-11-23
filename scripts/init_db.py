#!/usr/bin/env python3
import sqlite3
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / 'data'
DB_PATH = DATA / 'transport.db'
SCHEMA = DATA / 'schema.sql'
SEED1 = DATA / 'seed' / 'stops_seed.sql'
SEED2 = DATA / 'seed' / 'routes_seed.sql'

def read_sql(path: Path) -> str:
    if not path.exists():
        print(f"ERROR: missing {path}")
        sys.exit(2)
    return path.read_text(encoding='utf-8')

def main():
    DATA.mkdir(parents=True, exist_ok=True)
    if DB_PATH.exists():
        try:
            DB_PATH.unlink()
        except Exception as e:
            print(f"ERROR: cannot remove existing DB: {e}")
            sys.exit(3)

    schema_sql = read_sql(SCHEMA)
    seed1_sql = read_sql(SEED1)
    seed2_sql = read_sql(SEED2)

    try:
        conn = sqlite3.connect(str(DB_PATH))
        cur = conn.cursor()
        cur.executescript(schema_sql)
        cur.executescript(seed1_sql)
        cur.executescript(seed2_sql)
        conn.commit()
        conn.close()
        print("DB_CREATED_PY: created", DB_PATH)
    except Exception as e:
        print("ERROR: failed to create DB:", e)
        if DB_PATH.exists():
            try:
                DB_PATH.unlink()
            except Exception:
                pass
        sys.exit(4)

if __name__ == '__main__':
    main()
