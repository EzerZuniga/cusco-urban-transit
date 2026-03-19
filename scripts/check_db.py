#!/usr/bin/env python3
"""Check SQLite database for presence of tables/views.

Usage:
  ./scripts/check_db.py [--db PATH]

Exits:
  0 - OK (tables/views found or DB exists)
  2 - missing DB file
  3 - DB error (unable to open/query)
  4 - no tables/views found
"""

import argparse
import sqlite3
import sys
from pathlib import Path


def parse_args():
    p = argparse.ArgumentParser(description="Check SQLite DB for tables/views")
    p.add_argument("--db", type=Path, default=None,
                   help="Path to SQLite database (default: data/transport.db relative to repo root)")
    return p.parse_args()


def main():
    args = parse_args()
    root = Path(__file__).resolve().parents[1]
    db_path = args.db if args.db is not None else root / "data" / "transport.db"

    if not db_path.exists():
        print("MISSING_DB")
        return 2

    try:
        with sqlite3.connect(str(db_path)) as conn:
            cur = conn.cursor()
            cur.execute("SELECT name, type FROM sqlite_master WHERE type IN ('table','view') ORDER BY name;")
            rows = cur.fetchall()
    except sqlite3.Error as e:
        print(f"DB_ERROR: {e}", file=sys.stderr)
        return 3

    if not rows:
        print("NO_TABLES")
        return 4

    for name, typ in rows:
        print(f"{typ}: {name}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
