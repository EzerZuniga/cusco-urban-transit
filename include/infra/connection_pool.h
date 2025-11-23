#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H

#include "sqlite_wrapper.h"
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace urban_transport {

class ConnectionPool {
public:
    static ConnectionPool& get_instance();
    
    void initialize(const std::string& db_path, size_t pool_size = 5);
    void shutdown();
    
    std::shared_ptr<SQLiteWrapper> get_connection();
    void return_connection(std::shared_ptr<SQLiteWrapper> connection);

private:
    ConnectionPool() = default;
    ~ConnectionPool();
    
    std::string db_path_;
    size_t pool_size_ = 5;
    std::queue<std::shared_ptr<SQLiteWrapper>> connections_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool initialized_ = false;
    
    // Eliminar copia
    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;
};

} // namespace urban_transport

#endif // CONNECTION_POOL_H