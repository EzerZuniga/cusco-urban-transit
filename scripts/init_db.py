#!/usr/bin/env python3
"""Initialize SQLite database with schema and seed data.

Usage: ./scripts/init_db.py [--db PATH] [--schema PATH] [--seeds seed1 seed2 ...]
"""

import argparse
import sqlite3
import sys
from pathlib import Path


def parse_args():
    p = argparse.ArgumentParser(description="Create/initialize transport SQLite DB")
    root = Path(__file__).resolve().parents[1]
    default_data = root / "data"
    p.add_argument("--db", type=Path, default=default_data / "transport.db",
                   help="Path to the SQLite database to create")
    p.add_argument("--schema", type=Path, default=default_data / "schema.sql",
                   help="Path to SQL schema file")
    p.add_argument("--seeds", type=Path, nargs="*",
                   default=[default_data / "seed" / "stops_seed.sql", default_data / "seed" / "routes_seed.sql"],
                   help="Optional seed SQL files (applied in order)")
    p.add_argument("--force", action="store_true", help="Remove existing DB if present")
    return p.parse_args()


def read_sql(path: Path) -> str:
    if not path.exists():
        raise FileNotFoundError(str(path))
    return path.read_text(encoding="utf-8")


def main():
    args = parse_args()
    db_path: Path = args.db
    schema_path: Path = args.schema
    seeds = args.seeds

    db_path.parent.mkdir(parents=True, exist_ok=True)

    if db_path.exists():
        if args.force:
            try:
                db_path.unlink()
            except Exception as e:
                print(f"ERROR: cannot remove existing DB: {e}", file=sys.stderr)
                return 3
        else:
            print(f"DB already exists: {db_path}. Use --force to recreate.")
            return 0

    try:
        schema_sql = read_sql(schema_path)
    except FileNotFoundError:
        print(f"ERROR: schema file not found: {schema_path}", file=sys.stderr)
        return 2

    seed_sqls = []
    for s in seeds:
        try:
            seed_sqls.append(read_sql(s))
        except FileNotFoundError:
            print(f"WARNING: seed file not found, skipping: {s}", file=sys.stderr)

    try:
        with sqlite3.connect(str(db_path)) as conn:
            cur = conn.cursor()
            cur.executescript(schema_sql)
            for script in seed_sqls:
                cur.executescript(script)
            conn.commit()
        print(f"DB created: {db_path}")
        return 0
    except sqlite3.Error as e:
        print(f"ERROR: failed to create DB: {e}", file=sys.stderr)
        if db_path.exists():
            try:
                db_path.unlink()
            except Exception:
                pass
        return 4


if __name__ == "__main__":
    raise SystemExit(main())
