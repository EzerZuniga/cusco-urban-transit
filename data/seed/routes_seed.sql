INSERT INTO routes (id, name, transport_type) VALUES
(1, 'Línea 1 - Centro', 'bus'),
(2, 'Línea 2 - Norte-Sur', 'bus'),
(3, 'Línea 3 - Este-Oeste', 'bus'),
(4, 'Línea Express Aeropuerto', 'bus');

-- Rutas de metro
INSERT INTO routes (id, name, transport_type) VALUES
(5, 'Línea A - Roja', 'metro'),
(6, 'Línea B - Azul', 'metro'),
(7, 'Línea C - Verde', 'metro');

-- Paradas para Línea 1 - Centro (bus)
INSERT INTO route_stops (route_id, stop_id, sequence) VALUES
(1, 1, 1),  -- Plaza Central
(1, 3, 2),  -- Mercado Municipal
(1, 4, 3),  -- Hospital General
(1, 8, 4),  -- Centro Comercial
(1, 9, 5);  -- Estación 

-- Paradas para Línea 2 - Norte-Sur (bus)
INSERT INTO route_stops (route_id, stop_id, sequence) VALUES
(2, 2, 1),  -- Estación Norte
(2, 5, 2),  -- Universidad
(2, 1, 3),  -- Plaza Central
(2, 9, 4),  -- Estación 
(2, 10, 5); -- Aeropuerto

-- Paradas para Línea A - Roja (metro)
INSERT INTO route_stops (route_id, stop_id, sequence) VALUES
(5, 11, 1), -- Metro Centro
(5, 12, 2), -- Metro Norte
(5, 13, 3), -- Metro Este
(5, 14, 4), -- Metro Oeste
(5, 15, 5); -- Metro Sur

-- Paradas para Línea Express Aeropuerto (bus)
INSERT INTO route_stops (route_id, stop_id, sequence) VALUES
(4, 1, 1),   -- Plaza Central
(4, 10, 2);  -- Aeropuerto (directo)

-- Viajes de ejemplo
INSERT INTO trips (id, route_id, start_time, end_time) VALUES
(1, 1, '06:00:00', '06:45:00'),
(2, 1, '07:00:00', '07:45:00'),
(3, 2, '06:30:00', '07:30:00'),
(4, 5, '05:45:00', '06:15:00');