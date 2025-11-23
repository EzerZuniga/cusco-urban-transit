#include "infra/connection_pool.h"
#include "infra/logger.h"
#include <stdexcept>

using namespace urban_transport;

ConnectionPool& ConnectionPool::get_instance() {
    static ConnectionPool instance;
    return instance;
}

void ConnectionPool::initialize(const std::string& db_path, size_t pool_size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        Logger::get_instance().warning("El pool de conexiones ya está inicializado");
        return;
    }
    
    db_path_ = db_path;
    pool_size_ = pool_size;
    
    // Crear conexiones iniciales
    for (size_t i = 0; i < pool_size_; ++i) {
        auto connection = std::make_shared<SQLiteWrapper>();
        if (connection->open(db_path_)) {
            connections_.push(connection);
        } else {
            Logger::get_instance().error("Error al crear conexión en el pool");
        }
    }
    
    initialized_ = true;
    Logger::get_instance().info("Pool de conexiones inicializado con " + 
                               std::to_string(connections_.size()) + " conexiones");
}

void ConnectionPool::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    while (!connections_.empty()) {
        auto connection = connections_.front();
        connection->close();
        connections_.pop();
    }
    
    initialized_ = false;
    Logger::get_instance().info("Pool de conexiones cerrado");
}

ConnectionPool::~ConnectionPool() {
    shutdown();
}

std::shared_ptr<SQLiteWrapper> ConnectionPool::get_connection() {
    std::unique_lock<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        throw std::runtime_error("Pool de conexiones no inicializado");
    }
    
    // Esperar hasta que haya una conexión disponible
    condition_.wait(lock, [this]() { return !connections_.empty(); });
    
    auto connection = connections_.front();
    connections_.pop();
    
    return connection;
}

void ConnectionPool::return_connection(std::shared_ptr<SQLiteWrapper> connection) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        connections_.push(connection);
        condition_.notify_one();
    }
}