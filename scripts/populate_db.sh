#!/bin/bash

# Script para poblar la base de datos con datos de ejemplo adicionales

set -e

DB_FILE="data/transport.db"

echo "Poblando base de datos con datos adicionales..."

if [ ! -f "$DB_FILE" ]; then
    echo "Error: Base de datos no encontrada. Ejecuta init_db.sh primero."
    exit 1
fi

# Datos adicionales de ejemplo
sqlite3 "$DB_FILE" << 'EOF'
-- Paradas adicionales
INSERT OR IGNORE INTO stops (id, name, latitude, longitude) VALUES
(16, 'Estación Intermodal', 40.7350, -74.0050),
(17, 'Polideportivo', 40.7280, -73.9900),
(18, 'Biblioteca Central', 40.7180, -74.0020);

-- Rutas adicionales
INSERT OR IGNORE INTO routes (id, name, transport_type) VALUES
(8, 'Línea Nocturna', 'bus'),
(9, 'Ruta Turística', 'bus');

-- Asignar paradas a nuevas rutas
INSERT OR IGNORE INTO route_stops (route_id, stop_id, sequence) VALUES
(8, 1, 1),
(8, 4, 2),
(8, 7, 3),
(8, 10, 4),
(9, 1, 1),
(9, 3, 2),
(9, 6, 3),
(9, 8, 4);

-- Viajes adicionales
INSERT OR IGNORE INTO trips (id, route_id, start_time, end_time) VALUES
(5, 8, '22:00:00', '22:45:00'),
(6, 9, '09:00:00', '10:30:00');

EOF

echo "Datos adicionales insertados"
echo "Nuevas estadísticas:"
sqlite3 "$DB_FILE" "SELECT 'Paradas: ' || COUNT(*) FROM stops;"
sqlite3 "$DB_FILE" "SELECT 'Rutas: ' || COUNT(*) FROM routes;"
sqlite3 "$DB_FILE" "SELECT 'Viajes: ' || COUNT(*) FROM trips;"