# Cusco Urban Transit

![C++](https://img.shields.io/badge/lang-C%2B%2B-blue) ![CMake](https://img.shields.io/badge/build-CMake-lightgrey) ![SQLite](https://img.shields.io/badge/db-SQLite-orange)

Sistema modular en C++ para modelar y gestionar transporte urbano (paradas, rutas y viajes). El repositorio contiene la lógica de la aplicación, infraestructura para acceso a SQLite, scripts para inicializar la base de datos y pruebas unitarias.

---

## Tabla de contenidos

- [Descripción](#descripción)
- [Características](#características)
- [Requisitos](#requisitos)
- [Compilación y pruebas](#compilación-y-pruebas)
- [Base de datos](#base-de-datos)
- [Estructura del repositorio](#estructura-del-repositorio)
- [Scripts útiles](#scripts-útiles)
- [Contribución](#contribución)
- [Licencia y contacto](#licencia-y-contacto)

## Descripción

Proyecto educativo/prototipo que implementa:

- Modelado de paradas (`stops`), rutas (`routes`) y viajes (`trips`).
- Relación ordenada entre rutas y paradas (`route_stops`) y entre viajes y paradas (`trip_stops`).
- Capa de acceso a datos en `include/infra` y `src/infra` (wrapper SQLite, pool de conexiones).
- Servicios en `src/app/services` (rutas, paradas, viajes) que exponen operaciones CRUD y lógicas de negocio.

## Características

- Implementación en C++17 con CMake
- Uso de SQLite para persistencia local
- Pool de conexiones y wrapper alrededor de sqlite3
- Scripts para inicializar y poblar la BD (`scripts/init_db.py`, `scripts/check_db.py`)
- Pruebas unitarias en `tests/` (GoogleTest compatible)

## Requisitos

- CMake >= 3.15
- Compilador con soporte C++17 (GCC, Clang o MSVC)
- Python 3 (para scripts de inicialización, opcional)
- SQLite3 (biblioteca de desarrollo si compilas contra ella)

## Compilación y pruebas

1. Configurar el proyecto (out-of-source):

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

2. Compilar:

```bash
cmake --build build --config Debug -- -j
```

3. Ejecutar pruebas (si están configuradas en CMake):

```bash
ctest --test-dir build --output-on-failure
```

También puedes usar las tareas de VS Code en `.vscode/tasks.json` (ya configuradas) para `Configure`, `Build`, `Run tests`.

## Base de datos

El esquema está en `data/schema.sql` y los seeds en `data/seed/`.

Para crear o recrear la base de datos local `data/transport.db` puedes usar el script multiplataforma en Python:

```bash
python scripts/init_db.py
```

Verifica las tablas con:

```bash
python scripts/check_db.py
```

Nota: `data/transport.db` está en `.gitignore` por ser una copia local.

## Estructura del repositorio

```
CMakeLists.txt
data/
	├─ schema.sql
	├─ seed/
	│   ├─ routes_seed.sql
	│   └─ stops_seed.sql
	└─ transport.db (local, ignorado)
include/
	├─ infra/ (db, sqlite wrapper, logger)
	├─ core/ (algorithms, graph)
	└─ transport/ (services headers)
src/
	├─ app/
	│   ├─ services/ (route_service.cpp, stop_service.cpp, trip_service.cpp)
	│   ├─ algorithms.cpp
	│   └─ transport.cpp
	└─ infra/ (db.cpp, sqlite/, logging/)
tests/
	├─ test_algorithms.cpp
	├─ test_routes.cpp
	└─ test_stops.cpp
scripts/
	├─ init_db.py
	└─ check_db.py
```

## Scripts útiles

- `scripts/init_db.py` — crea `data/transport.db` desde `schema.sql` y los seeds.
- `scripts/check_db.py` — lista tablas en `data/transport.db`.
- `scripts/run_tests.sh` — wrapper para ejecutar tests (si aplica en tu entorno).

## Contribución

Si deseas contribuir:

1. Abre un issue describiendo el cambio.
2. Crea una rama con nombre descriptivo.
3. Añade tests para cualquier lógica nueva o bugfix.
4. Envía un pull request explicando el motivo y pruebas realizadas.

## Licencia y contacto

El repositorio contiene una licencia MIT simple por defecto en `LICENSE`.

Contacto: abre un issue o escribe a tu email de contacto en el repositorio.

---
