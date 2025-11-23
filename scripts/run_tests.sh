#!/bin/bash

# Script para ejecutar todas las pruebas

set -e

BUILD_DIR="build"
TEST_EXEC="$BUILD_DIR/test_transport"

echo "Ejecutando pruebas unitarias..."

# Construir en modo debug si no existe
if [ ! -f "$TEST_EXEC" ]; then
    echo "Construyendo proyecto en modo debug..."
    ./scripts/build.sh debug
fi

# Ejecutar pruebas
echo "Ejecutando pruebas..."
"$TEST_EXEC"

echo "Todas las pruebas pasaron"