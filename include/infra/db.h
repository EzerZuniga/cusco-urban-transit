#ifndef DB_H
#define DB_H

#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace urban_transport {

class Database {
public:
    Database();
    ~Database();
    
    bool connect(const std::string& db_path);
    void disconnect();
    bool is_connected() const;
    
    // Ejecutar consultas
    bool execute(const std::string& sql);
    bool execute_with_params(const std::string& sql, 
                           const std::vector<std::string>& params);
    
    // Consultas que retornan resultados
    using RowCallback = std::function<bool(const std::vector<std::string>&)>;
    bool query(const std::string& sql, RowCallback callback);
    bool query_with_params(const std::string& sql, 
                         const std::vector<std::string>& params,
                         RowCallback callback);
    
    // Transacciones
    bool begin_transaction();
    bool commit_transaction();
    bool rollback_transaction();

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace urban_transport

#endif // DB_H