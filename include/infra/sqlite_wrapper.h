#ifndef SQLITE_WRAPPER_H
#define SQLITE_WRAPPER_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace urban_transport {

class SQLiteWrapper {
public:
    SQLiteWrapper();
    ~SQLiteWrapper();
    
    bool open(const std::string& filename);
    void close();
    bool is_open() const;
    
    // Operaciones básicas
    bool execute(const std::string& sql);
    bool execute_with_params(const std::string& sql, 
                           const std::vector<std::string>& params);
    
    using RowCallback = std::function<bool(const std::vector<std::string>&)>;
    bool query(const std::string& sql, RowCallback callback) const;
    bool query_with_params(const std::string& sql, 
                         const std::vector<std::string>& params,
                         RowCallback callback) const;
    
    // Transacciones
    bool begin_transaction();
    bool commit_transaction();
    bool rollback_transaction();
    
    // Último error
    std::string last_error() const;
    int last_error_code() const;

private:
    sqlite3* db_ = nullptr;
    
    void cleanup();
};

} // namespace urban_transport

#endif // SQLITE_WRAPPER_H