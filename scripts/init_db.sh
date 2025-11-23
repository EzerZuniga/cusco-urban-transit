#!/bin/bash

# Script para inicializar la base de datos desde cero

set -e

DB_FILE="data/transport.db"
SCHEMA_FILE="data/schema.sql"
SEED_DIR="data/seed"

echo "Inicializando base de datos..."

# Eliminar base de datos existente si existe
if [ -f "$DB_FILE" ]; then
    echo "Eliminando base de datos existente..."
    rm "$DB_FILE"
fi

# Crear directorio de datos si no existe
mkdir -p data

echo "Creando esquema de base de datos..."
if [ -f "$SCHEMA_FILE" ]; then
    sqlite3 "$DB_FILE" < "$SCHEMA_FILE"
    echo "Esquema creado"
else
    echo "Error: No se encontró $SCHEMA_FILE"
    exit 1
fi

# Poblar con datos iniciales
echo "Insertando datos iniciales..."
for seed_file in "$SEED_DIR"/*.sql; do
    if [ -f "$seed_file" ]; then
        echo "Insertando: $(basename $seed_file)"
        sqlite3 "$DB_FILE" < "$seed_file"
    fi
done

echo "Base de datos inicializada: $DB_FILE"
echo "Estadísticas:"
sqlite3 "$DB_FILE" "SELECT 'Paradas: ' || COUNT(*) FROM stops;"
sqlite3 "$DB_FILE" "SELECT 'Rutas: ' || COUNT(*) FROM routes;"
sqlite3 "$DB_FILE" "SELECT 'Viajes: ' || COUNT(*) FROM trips;"