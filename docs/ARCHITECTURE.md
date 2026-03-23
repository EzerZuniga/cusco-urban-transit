# Arquitectura

El proyecto sigue una arquitectura por capas:

- `apps/`: entrypoints (CLI u otras interfaces).
- `src/app`: orquestación de casos de uso.
- `src/core`: lógica algorítmica y estructuras base.
- `src/infra`: acceso a datos, logging e integraciones externas.
- `include/`: contratos públicos de cada capa.

## Escalabilidad

- Nuevo canal de entrada (API REST, worker, etc.): crear en `apps/` y enlazar `urban_transport_core`.
- Nuevo backend de datos: agregar wrapper en `src/infra/` e integrarlo en `cmake/Dependencies.cmake`.
- Nuevos módulos de dominio: crear carpeta en `src/<dominio>/` con headers en `include/<dominio>/`.

## Regla clave

La librería `urban_transport_core` concentra la lógica reusable.  
Los ejecutables y pruebas solo deben consumirla, no duplicar fuentes.
