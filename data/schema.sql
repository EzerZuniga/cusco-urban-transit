-- Tabla de paradas de transporte
CREATE TABLE IF NOT EXISTS stops (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Tabla de rutas de transporte
CREATE TABLE IF NOT EXISTS routes (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    transport_type TEXT NOT NULL CHECK (transport_type IN ('bus', 'metro', 'train', 'tram')),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Tabla de relación entre rutas y paradas (con orden)
CREATE TABLE IF NOT EXISTS route_stops (
    route_id INTEGER,
    stop_id INTEGER,
    sequence INTEGER NOT NULL,
    PRIMARY KEY (route_id, stop_id),
    FOREIGN KEY (route_id) REFERENCES routes(id) ON DELETE CASCADE,
    FOREIGN KEY (stop_id) REFERENCES stops(id) ON DELETE CASCADE
);

-- Tabla de viajes (horarios específicos)
CREATE TABLE IF NOT EXISTS trips (
    id INTEGER PRIMARY KEY,
    route_id INTEGER NOT NULL,
    start_time TEXT NOT NULL, -- HH:MM:SS
    end_time TEXT NOT NULL,   -- HH:MM:SS
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (route_id) REFERENCES routes(id) ON DELETE CASCADE
);

-- Tabla de secuencia de paradas por viaje
CREATE TABLE IF NOT EXISTS trip_stops (
    trip_id INTEGER,
    stop_id INTEGER,
    arrival_time TEXT NOT NULL, -- HH:MM:SS
    sequence INTEGER NOT NULL,
    PRIMARY KEY (trip_id, stop_id),
    FOREIGN KEY (trip_id) REFERENCES trips(id) ON DELETE CASCADE,
    FOREIGN KEY (stop_id) REFERENCES stops(id) ON DELETE CASCADE
);

-- Índices para mejorar el rendimiento
CREATE INDEX IF NOT EXISTS idx_stops_location ON stops(latitude, longitude);
CREATE INDEX IF NOT EXISTS idx_route_stops_route ON route_stops(route_id);
CREATE INDEX IF NOT EXISTS idx_route_stops_stop ON route_stops(stop_id);
CREATE INDEX IF NOT EXISTS idx_trips_route ON trips(route_id);
CREATE INDEX IF NOT EXISTS idx_trip_stops_trip ON trip_stops(trip_id);
CREATE INDEX IF NOT EXISTS idx_trip_stops_stop ON trip_stops(stop_id);