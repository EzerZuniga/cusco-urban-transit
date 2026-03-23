# Cusco Urban Transit

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Build](https://img.shields.io/badge/build-CMake-0f4c81)
![Database](https://img.shields.io/badge/database-SQLite%20%7C%20PostgreSQL-2f855a)
![License](https://img.shields.io/badge/license-MIT-green)

Sistema de transporte urbano en C++ con arquitectura por capas, orientado a mantener el código limpio, modular y escalable.

## Tabla de contenidos

1. [Resumen](#resumen)
2. [Arquitectura](#arquitectura)
3. [Estructura del repositorio](#estructura-del-repositorio)
4. [Requisitos](#requisitos)
5. [Inicio rápido (SQLite)](#inicio-rápido-sqlite)
6. [Ejecución](#ejecución)
7. [Pruebas](#pruebas)
8. [Modo PostgreSQL](#modo-postgresql)
9. [Opciones CMake](#opciones-cmake)
10. [Contribución](#contribución)
11. [Licencia](#licencia)

## Resumen

El proyecto implementa:

- Gestión de paradas (`stops`), rutas (`routes`) y viajes (`trips`).
- Relación ordenada entre rutas y paradas (`route_stops`) y entre viajes y paradas (`trip_stops`).
- Algoritmos de rutas sobre un grafo de transporte.
- Abstracción de base de datos para cambiar backend entre SQLite y PostgreSQL.

## Arquitectura

La organización sigue una separación clara por capas:

- `apps/`: entrypoints (CLI y futuros frontends/API).
- `src/app`: casos de uso y orquestación.
- `src/core`: algoritmos y estructuras base.
- `src/infra`: persistencia, logging e integraciones.
- `include/`: interfaces públicas consumidas por app y tests.

La lógica reusable se construye como librería `urban_transport_core`; ejecutables y pruebas solo la consumen.

## Estructura del repositorio

```text
.
├─ apps/
│  ├─ CMakeLists.txt
│  └─ cli/
│     └─ main.cpp
├─ cmake/
│  ├─ Dependencies.cmake
│  ├─ ProjectOptions.cmake
│  └─ modules/
├─ data/
│  ├─ schema.sql
│  └─ seed/
│     ├─ routes_seed.sql
│     └─ stops_seed.sql
├─ docs/
│  └─ ARCHITECTURE.md
├─ include/
│  ├─ core/
│  ├─ infra/
│  └─ transport/
├─ src/
│  ├─ CMakeLists.txt
│  ├─ app/
│  ├─ core/
│  └─ infra/
├─ tests/
│  ├─ CMakeLists.txt
│  ├─ test_algorithms.cpp
│  ├─ test_routes.cpp
│  └─ test_stops.cpp
└─ CMakeLists.txt
```

## Requisitos

- CMake >= 3.15
- Compilador con soporte C++17 (MSVC, GCC o Clang)
- SQLite3 (opcional, solo si no usas el amalgamation embebido)
- PostgreSQL client libs/headers (solo si compilas con `USE_POSTGRES=ON`)

## Inicio rápido (SQLite)

### 1) Configurar y compilar

```bash
cmake -S . -B build
cmake --build build --config Debug
```

### 2) Crear base de datos local

Aplica esquema y seeds sobre `data/transport.db` (requiere `sqlite3` CLI):

```bash
sqlite3 data/transport.db < data/schema.sql
sqlite3 data/transport.db < data/seed/stops_seed.sql
sqlite3 data/transport.db < data/seed/routes_seed.sql
```

### 3) Ejecutar la app CLI

Windows (Visual Studio generator):

```powershell
.\build\apps\Debug\urban-transport-system.exe
```

Linux/macOS (single-config generators):

```bash
./build/apps/urban-transport-system
```

## Ejecución

La aplicación usa por defecto `data/transport.db`.

Si defines variables de entorno, toma prioridad:

- `PG_CONN`
- `DATABASE_URL`

Esto permite cambiar la conexión sin modificar código.

## Pruebas

Configura y ejecuta pruebas:

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Debug
ctest --test-dir build --output-on-failure -C Debug
```

Notas importantes:

- Si GoogleTest no está instalado y `UT_FETCH_MISSING_TEST_DEPS=OFF`, el target de tests se omite sin romper la configuración.
- Si quieres permitir descarga automática de GoogleTest durante `configure`, usa:

```bash
cmake -S . -B build -DBUILD_TESTING=ON -DUT_FETCH_MISSING_TEST_DEPS=ON
```

- Las pruebas crean bases temporales dentro del directorio de build (`.../tests/runtime/`).

## Modo PostgreSQL

Compilar con backend PostgreSQL:

```bash
cmake -S . -B build -DUSE_POSTGRES=ON
cmake --build build --config Debug
```

Antes de ejecutar, define una cadena de conexión válida en `PG_CONN` o `DATABASE_URL`.

## Opciones CMake

Opciones más útiles del proyecto:

- `-DUSE_POSTGRES=ON|OFF`: selecciona backend de BD.
- `-DBUILD_TESTING=ON|OFF`: habilita/deshabilita tests.
- `-DUT_FETCH_MISSING_TEST_DEPS=ON|OFF`: permite descargar GoogleTest si falta.
- `-DUT_ENABLE_STRICT_WARNINGS=ON|OFF`: activa/desactiva warnings estrictos.

## Contribución

Flujo recomendado:

1. Crear rama de trabajo descriptiva.
2. Hacer cambios pequeños y atómicos.
3. Ejecutar build y pruebas antes del commit.
4. Abrir PR con contexto técnico y validación realizada.

## Licencia

Este proyecto está licenciado bajo MIT. Revisa [LICENSE](LICENSE).
