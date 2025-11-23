#!/usr/bin/env python3
import sqlite3
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
DB = ROOT / 'data' / 'transport.db'
if not DB.exists():
    print('MISSING_DB')
    raise SystemExit(2)
conn = sqlite3.connect(str(DB))
cur = conn.cursor()
cur.execute("SELECT name, type FROM sqlite_master WHERE type IN ('table','view') ORDER BY name;")
rows = cur.fetchall()
if not rows:
    print('NO_TABLES')
else:
    for name, typ in rows:
        print(f"{typ}: {name}")
conn.close()
