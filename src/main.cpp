#include <iostream>
#include <limits>
#include <string>
#include "transport/transport.h"
#include "infra/logger.h"
using namespace urban_transport;

static int read_int(const std::string& prompt)
{
    int value;
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada inválida. Intente de nuevo: ";
    }
    return value;
}

void print_menu()
{
    std::cout << "\n=== Sistema de Transporte Urbano ===\n";
    std::cout << "1. Listar todas las paradas\n";
    std::cout << "2. Listar todas las rutas\n";
    std::cout << "3. Buscar camino más corto\n";
    std::cout << "4. Rutas por parada\n";
    std::cout << "5. Salir\n";
    std::cout << "Seleccione una opción: ";
}

void list_stops(TransportSystem& system)
{
    auto stops = system.get_all_stops();
    std::cout << "\n--- Paradas de Transporte ---\n";
    for (const auto& stop : stops) {
        std::cout << "ID: " << stop.id
                << " | Nombre: " << stop.name
                << " | Coord: (" << stop.latitude << ", " << stop.longitude << ")\n";
    }
    std::cout << "Total: " << stops.size() << " paradas\n";
}

void list_routes(TransportSystem& system)
{
    auto routes = system.get_all_routes();
    std::cout << "\n--- Rutas de Transporte ---\n";
    for (const auto& route : routes) {
        std::cout << "ID: " << route.id
                << " | Nombre: " << route.name
                << " | Tipo: " << route.transport_type
                << " | Paradas: " << route.stop_ids.size() << "\n";
    }
    std::cout << "Total: " << routes.size() << " rutas\n";
}

void find_shortest_path(TransportSystem& system)
{
    int start = read_int("ID de parada inicial: ");
    int end = read_int("ID de parada final: ");

    auto path = system.find_shortest_path(start, end);

    if (path.empty()) {
        std::cout << "No se encontró camino entre las paradas " << start << " y " << end << "\n";
    } else {
        std::cout << "Camino más corto: ";
        for (size_t i = 0; i < path.size(); ++i) {
            std::cout << path[i];
            if (i + 1 < path.size()) std::cout << " -> ";
        }
        std::cout << "\n";
    }
}

void routes_through_stop(TransportSystem& system)
{
    int stop_id = read_int("ID de parada: ");

    auto routes = system.find_routes_through_stop(stop_id);
    std::cout << "\n--- Rutas que pasan por la parada " << stop_id << " ---\n";
    for (const auto& route : routes) {
        std::cout << "Ruta: " << route.name << " (" << route.transport_type << ")\n";
    }
    std::cout << "Total: " << routes.size() << " rutas\n";
}

int main()
{
    Logger::get_instance().initialize();
    Logger::get_instance().info("Iniciando Sistema de Transporte Urbano");

    TransportSystem system;

    if (!system.initialize("data/transport.db")) {
        std::cerr << "Error al inicializar el sistema de transporte\n";
        return 1;
    }

    std::cout << "Sistema de Transporte Urbano - Inicializado correctamente\n";

    int option = 0;
    while (option != 5) {
        print_menu();
        if (!(std::cin >> option)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            option = 0;
            std::cout << "Entrada inválida. Intente de nuevo.\n";
            continue;
        }

        switch (option) {
        case 1:
            list_stops(system);
            break;
        case 2:
            list_routes(system);
            break;
        case 3:
            find_shortest_path(system);
            break;
        case 4:
            routes_through_stop(system);
            break;
        case 5:
            std::cout << "Saliendo...\n";
            break;
        default:
            std::cout << "Opción inválida\n";
            break;
        }
    }

    system.shutdown();
    Logger::get_instance().info("Sistema de Transporte Urbano finalizado");
    Logger::get_instance().shutdown();

    return 0;
}