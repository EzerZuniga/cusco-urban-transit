#ifndef SQLITE_WRAPPER_H
#define SQLITE_WRAPPER_H

#include "infra/db_wrapper.h"
#include <string>
#include <vector>
#include <memory>

namespace urban_transport {

#ifndef USE_POSTGRES
#include <sqlite3.h>

class SQLiteWrapper : public DBWrapper {
#else
// When building with USE_POSTGRES the SQLite implementation is not used.
// Provide a lightweight stub so headers can be included without requiring sqlite3 headers.
class SQLiteWrapper : public DBWrapper {
#endif
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
    
    bool query(const std::string& sql, RowCallback callback) const override;
    bool query_with_params(const std::string& sql, 
                         const std::vector<std::string>& params,
                         RowCallback callback) const override;
    
    // Transacciones
    bool begin_transaction() override;
    bool commit_transaction() override;
    bool rollback_transaction() override;
    
    // Último error
    std::string last_error() const override;
    int last_error_code() const override;

private:
#ifndef USE_POSTGRES
    sqlite3* db_ = nullptr;
    void cleanup();
#else
    // stub state
    void cleanup() {}
#endif
};

} // namespace urban_transport

#endif // SQLITE_WRAPPER_H