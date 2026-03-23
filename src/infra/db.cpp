#include "infra/db.h"
#include "infra/db_wrapper.h"
#if defined(USE_POSTGRES)
#include "infra/postgres_wrapper.h"
#else
#include "infra/sqlite_wrapper.h"
#endif
#include "infra/logger.h"
#include <memory>

using namespace urban_transport;

class Database::Impl {
public:
    Impl() = default;

    bool connect(const std::string& conn) {
        if (db_ && db_->is_open()) {
            db_->close();
        }

#if defined(USE_POSTGRES)
        db_ = std::make_unique<PostgresWrapper>();
#else
        db_ = std::make_unique<SQLiteWrapper>();
#endif
        if (!db_) return false;
        Logger::get_instance().debug(std::string("Connecting to DB: ") + conn);
        return db_->open(conn);
    }
    
    void disconnect() {
        if (db_) {
            db_->close();
            db_.reset();
        }
    }
    
    bool is_connected() const {
        return db_ && db_->is_open();
    }
    
    bool execute(const std::string& sql) {
        if (!db_) {
            Logger::get_instance().error("Database wrapper is not initialized");
            return false;
        }
        Logger::get_instance().debug("Ejecutando SQL: " + sql);
        return db_->execute(sql);
    }
    
    bool execute_with_params(const std::string& sql, const std::vector<std::string>& params) {
        if (!db_) {
            Logger::get_instance().error("Database wrapper is not initialized");
            return false;
        }
        Logger::get_instance().debug("Ejecutando SQL con parámetros: " + sql);
        return db_->execute_with_params(sql, params);
    }
    
    bool query(const std::string& sql, RowCallback callback) const {
        if (!db_) {
            Logger::get_instance().error("Database wrapper is not initialized");
            return false;
        }
        Logger::get_instance().debug("Consultando SQL: " + sql);
        return db_->query(sql, callback);
    }
    
    bool query_with_params(const std::string& sql, 
                          const std::vector<std::string>& params,
                          RowCallback callback) const {
        if (!db_) {
            Logger::get_instance().error("Database wrapper is not initialized");
            return false;
        }
        Logger::get_instance().debug("Consultando SQL con parámetros: " + sql);
        return db_->query_with_params(sql, params, callback);
    }
    
    bool begin_transaction() {
        if (!db_) {
            Logger::get_instance().error("Database wrapper is not initialized");
            return false;
        }
        Logger::get_instance().debug("Iniciando transacción");
        return db_->begin_transaction();
    }
    
    bool commit_transaction() {
        if (!db_) {
            Logger::get_instance().error("Database wrapper is not initialized");
            return false;
        }
        Logger::get_instance().debug("Confirmando transacción");
        return db_->commit_transaction();
    }
    
    bool rollback_transaction() {
        if (!db_) {
            Logger::get_instance().error("Database wrapper is not initialized");
            return false;
        }
        Logger::get_instance().debug("Revirtiendo transacción");
        return db_->rollback_transaction();
    }

private:
    std::unique_ptr<DBWrapper> db_;
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

bool Database::query(const std::string& sql, RowCallback callback) const {
    return pimpl->query(sql, callback);
}

bool Database::query_with_params(const std::string& sql, 
                                const std::vector<std::string>& params,
                                RowCallback callback) const {
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
