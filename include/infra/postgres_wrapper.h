#ifndef POSTGRES_WRAPPER_H
#define POSTGRES_WRAPPER_H

#include "infra/db_wrapper.h"
#include <libpq-fe.h>
#include <string>
#include <vector>

namespace urban_transport {

class PostgresWrapper : public DBWrapper {
public:
    PostgresWrapper();
    ~PostgresWrapper();

    bool open(const std::string& conn) override;
    void close() override;
    bool is_open() const override;

    bool execute(const std::string& sql) override;
    bool execute_with_params(const std::string& sql, const std::vector<std::string>& params) override;

    bool query(const std::string& sql, RowCallback callback) const override;
    bool query_with_params(const std::string& sql, const std::vector<std::string>& params, RowCallback callback) const override;

    bool begin_transaction() override;
    bool commit_transaction() override;
    bool rollback_transaction() override;

    std::string last_error() const override;
    int last_error_code() const override;

private:
    PGconn* conn_ = nullptr;
};

} // namespace urban_transport

#endif // POSTGRES_WRAPPER_H
