#!/bin/bash

# Script de construcción para Urban Transport System
# Uso: ./scripts/build.sh [debug|release]

set -e

BUILD_TYPE="${1:-release}"
BUILD_DIR="build"

echo "🔨 Construyendo Urban Transport System ($BUILD_TYPE)"

# Crear directorio de construcción si no existe
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
    echo "Directorio de construcción creado: $BUILD_DIR"
fi

cd "$BUILD_DIR"

# Configurar CMake
if [ "$BUILD_TYPE" = "debug" ]; then
    cmake -DCMAKE_BUILD_TYPE=Debug ..
else
    cmake -DCMAKE_BUILD_TYPE=Release ..
fi

# Compilar
echo "Compilando proyecto..."
make -j$(nproc)

echo "Construcción completada en modo $BUILD_TYPE"
echo "Ejecutable: $BUILD_DIR/urban-transport-system"