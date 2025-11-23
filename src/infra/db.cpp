#include "infra/db.h"
#include "infra/sqlite_wrapper.h"
#include "infra/logger.h"
#include <memory>

using namespace urban_transport;

class Database::Impl {
public:
    bool connect(const std::string& db_path) {
        return sqlite_.open(db_path);
    }
    
    void disconnect() {
        sqlite_.close();
    }
    
    bool is_connected() const {
        // Necesitaríamos agregar un método en SQLiteWrapper para verificar conexión
        return true; // Simplificado por ahora
    }
    
    bool execute(const std::string& sql) {
        Logger::get_instance().debug("Ejecutando SQL: " + sql);
        return sqlite_.execute(sql);
    }
    
    bool execute_with_params(const std::string& sql, const std::vector<std::string>& params) {
        Logger::get_instance().debug("Ejecutando SQL con parámetros: " + sql);
        return sqlite_.execute_with_params(sql, params);
    }
    
    bool query(const std::string& sql, RowCallback callback) {
        Logger::get_instance().debug("Consultando SQL: " + sql);
        return sqlite_.query(sql, callback);
    }
    
    bool query_with_params(const std::string& sql, 
                          const std::vector<std::string>& params,
                          RowCallback callback) {
        Logger::get_instance().debug("Consultando SQL con parámetros: " + sql);
        return sqlite_.query_with_params(sql, params, callback);
    }
    
    bool begin_transaction() {
        Logger::get_instance().debug("Iniciando transacción");
        return sqlite_.begin_transaction();
    }
    
    bool commit_transaction() {
        Logger::get_instance().debug("Confirmando transacción");
        return sqlite_.commit_transaction();
    }
    
    bool rollback_transaction() {
        Logger::get_instance().debug("Revirtiendo transacción");
        return sqlite_.rollback_transaction();
    }

private:
    SQLiteWrapper sqlite_;
};

// Implementación de Database
Database::Database() : pimpl(std::make_unique<Impl>()) {}
Database::~Database() = default;

bool Database::connect(const std::string& db_path) {
    return pimpl->connect(db_path);
}

void Database::disconnect() {
    pimpl->disconnect();
}

bool Database::is_connected() const {
    return pimpl->is_connected();
}

bool Database::execute(const std::string& sql) {
    return pimpl->execute(sql);
}

bool Database::execute_with_params(const std::string& sql, const std::vector<std::string>& params) {
    return pimpl->execute_with_params(sql, params);
}

bool Database::query(const std::string& sql, RowCallback callback) {
    return pimpl->query(sql, callback);
}

bool Database::query_with_params(const std::string& sql, 
                                const std::vector<std::string>& params,
                                RowCallback callback) {
    return pimpl->query_with_params(sql, params, callback);
}

bool Database::begin_transaction() {
    return pimpl->begin_transaction();
}

bool Database::commit_transaction() {
    return pimpl->commit_transaction();
}

bool Database::rollback_transaction() {
    return pimpl->rollback_transaction();
}