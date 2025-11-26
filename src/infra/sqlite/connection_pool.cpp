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
        Logger::get_instance().warning("Connection pool already initialized");
        return;
    }

    db_path_ = db_path;
    pool_size_ = pool_size > 0 ? pool_size : pool_size_;

    for (size_t i = 0; i < pool_size_; ++i) {
        auto connection = std::make_shared<SQLiteWrapper>();
        if (connection->open(db_path_)) {
            connections_.push(connection);
        } else {
            Logger::get_instance().error("Failed to create connection in pool");
        }
    }

    initialized_ = true;
    Logger::get_instance().info("Connection pool initialized with " + std::to_string(connections_.size()) + " connections");
}

void ConnectionPool::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    while (!connections_.empty()) {
        auto connection = connections_.front();
        if (connection) connection->close();
        connections_.pop();
    }

    initialized_ = false;
    Logger::get_instance().info("Connection pool shutdown");
}

ConnectionPool::~ConnectionPool() {
    shutdown();
}

std::shared_ptr<SQLiteWrapper> ConnectionPool::get_connection() {
    std::unique_lock<std::mutex> lock(mutex_);

    if (!initialized_) {
        throw std::runtime_error("Connection pool not initialized");
    }

    condition_.wait(lock, [this]() { return !connections_.empty(); });

    auto connection = connections_.front();
    connections_.pop();

    return connection;
}

void ConnectionPool::return_connection(std::shared_ptr<SQLiteWrapper> connection) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_ || !connection) return;

    connections_.push(std::move(connection));
    condition_.notify_one();
}