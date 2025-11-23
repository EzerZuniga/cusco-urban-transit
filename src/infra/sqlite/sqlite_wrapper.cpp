#include "infra/sqlite_wrapper.h"
#include "infra/logger.h"
#include <iostream>

using namespace urban_transport;

SQLiteWrapper::SQLiteWrapper() : db_(nullptr) {}

SQLiteWrapper::~SQLiteWrapper() {
    cleanup();
}

bool SQLiteWrapper::open(const std::string& filename) {
    if (db_) {
        Logger::get_instance().warning("La base de datos ya está abierta");
        return true;
    }
    
    int rc = sqlite3_open(filename.c_str(), &db_);
    if (rc != SQLITE_OK) {
        Logger::get_instance().error("Error al abrir la base de datos: " + std::string(sqlite3_errmsg(db_)));
        cleanup();
        return false;
    }
    
    // Habilitar claves foráneas
    execute("PRAGMA foreign_keys = ON;");
    
    Logger::get_instance().info("Base de datos abierta: " + filename);
    return true;
}

void SQLiteWrapper::close() {
    cleanup();
}

bool SQLiteWrapper::is_open() const {
    return db_ != nullptr;
}

bool SQLiteWrapper::execute(const std::string& sql) {
    if (!db_) {
        Logger::get_instance().error("Base de datos no está abierta");
        return false;
    }
    
    char* error_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error_msg);
    
    if (rc != SQLITE_OK) {
        Logger::get_instance().error("Error en execute: " + std::string(error_msg));
        sqlite3_free(error_msg);
        return false;
    }
    
    return true;
}

bool SQLiteWrapper::execute_with_params(const std::string& sql, const std::vector<std::string>& params) {
    if (!db_) {
        Logger::get_instance().error("Base de datos no está abierta");
        return false;
    }
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        Logger::get_instance().error("Error al preparar statement: " + std::string(sqlite3_errmsg(db_)));
        return false;
    }
    
    // Vincular parámetros
    for (size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    
    rc = sqlite3_step(stmt);
    bool success = (rc == SQLITE_DONE);
    
    if (!success) {
        Logger::get_instance().error("Error en execute_with_params: " + std::string(sqlite3_errmsg(db_)));
    }
    
    sqlite3_finalize(stmt);
    return success;
}

bool SQLiteWrapper::query(const std::string& sql, RowCallback callback) {
    if (!db_) {
        Logger::get_instance().error("Base de datos no está abierta");
        return false;
    }
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        Logger::get_instance().error("Error al preparar query: " + std::string(sqlite3_errmsg(db_)));
        return false;
    }
    
    bool success = true;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int column_count = sqlite3_column_count(stmt);
        std::vector<std::string> row;
        
        for (int i = 0; i < column_count; ++i) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.push_back(text ? text : "");
        }
        
        if (!callback(row)) {
            break;
        }
    }
    
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        Logger::get_instance().error("Error durante query: " + std::string(sqlite3_errmsg(db_)));
        success = false;
    }
    
    sqlite3_finalize(stmt);
    return success;
}

bool SQLiteWrapper::query_with_params(const std::string& sql, 
                                     const std::vector<std::string>& params,
                                     RowCallback callback) {
    if (!db_) {
        Logger::get_instance().error("Base de datos no está abierta");
        return false;
    }
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        Logger::get_instance().error("Error al preparar query con parámetros: " + std::string(sqlite3_errmsg(db_)));
        return false;
    }
    
    // Vincular parámetros
    for (size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    
    bool success = true;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int column_count = sqlite3_column_count(stmt);
        std::vector<std::string> row;
        
        for (int i = 0; i < column_count; ++i) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.push_back(text ? text : "");
        }
        
        if (!callback(row)) {
            break;
        }
    }
    
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        Logger::get_instance().error("Error durante query con parámetros: " + std::string(sqlite3_errmsg(db_)));
        success = false;
    }
    
    sqlite3_finalize(stmt);
    return success;
}

bool SQLiteWrapper::begin_transaction() {
    return execute("BEGIN TRANSACTION;");
}

bool SQLiteWrapper::commit_transaction() {
    return execute("COMMIT;");
}

bool SQLiteWrapper::rollback_transaction() {
    return execute("ROLLBACK;");
}

std::string SQLiteWrapper::last_error() const {
    return db_ ? sqlite3_errmsg(db_) : "Database not open";
}

int SQLiteWrapper::last_error_code() const {
    return db_ ? sqlite3_errcode(db_) : -1;
}

void SQLiteWrapper::cleanup() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
        Logger::get_instance().info("Base de datos cerrada");
    }
}