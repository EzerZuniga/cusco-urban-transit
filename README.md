# Cusco Urban Transit

![C++](https://img.shields.io/badge/lang-C%2B%2B-blue) ![CMake](https://img.shields.io/badge/build-CMake-lightgrey) ![DB](https://img.shields.io/badge/db-SQLite%20%7C%20PostgreSQL-orange)

Sistema modular en C++ para modelar transporte urbano (paradas, rutas y viajes), con arquitectura por capas y build escalable con CMake.

## Objetivos del proyecto

- Separar lógica de dominio, infraestructura y ejecutables.
- Mantener una estructura limpia para crecer sin desorden.
- Permitir cambio de backend de base de datos (`SQLite` o `PostgreSQL`) sin reescribir servicios.

## Requisitos

- CMake >= 3.15
- Compilador con soporte C++17 (MSVC, GCC o Clang)
- SQLite3 (opcional, si no se usa el amalgamation embebido)
- PostgreSQL client headers/libs (solo si compilas con `-DUSE_POSTGRES=ON`)

## Compilación

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

## Pruebas

```bash
ctest --test-dir build --output-on-failure
```

Notas:

- Las pruebas crean sus `.db` temporales dentro del directorio de build (`.../tests/runtime/`).
- Por defecto, si GoogleTest no está instalado, los tests se omiten sin romper `configure`.
- Para permitir descarga automática de GoogleTest: `-DUT_FETCH_MISSING_TEST_DEPS=ON`.

## Base de datos

- Esquema: `data/schema.sql`
- Seeds: `data/seed/routes_seed.sql`, `data/seed/stops_seed.sql`
- DB local recomendada para app: `data/transport.db`

## Estructura del repositorio

```text
.
├─ apps/
│  └─ cli/
│     └─ main.cpp                 # Entry point de la app
├─ cmake/
│  ├─ Dependencies.cmake          # Resolución de SQLite/Postgres + GTest
│  ├─ ProjectOptions.cmake        # Estándar C++, warnings, flags
│  └─ modules/
│     └─ FindSQLite3.cmake        # Fallback module
├─ data/
│  ├─ schema.sql
│  └─ seed/
├─ docs/
│  └─ ARCHITECTURE.md
├─ include/
│  ├─ core/
│  ├─ infra/
│  └─ transport/
├─ src/
│  ├─ app/
│  ├─ core/
│  └─ infra/
├─ tests/
│  ├─ CMakeLists.txt
│  ├─ test_algorithms.cpp
│  ├─ test_routes.cpp
│  └─ test_stops.cpp
├─ third_party/
└─ CMakeLists.txt
```

## Convenciones de mantenimiento

- Build siempre fuera de fuente (`build/`).
- Evitar archivos temporales en raíz del repo.
- Cada target CMake define sus includes, warnings y dependencias (sin flags globales).
- Escalar agregando nuevos ejecutables en `apps/` y nuevas librerías por dominio en `src/`.

## Licencia

MIT. Ver `LICENSE`.
