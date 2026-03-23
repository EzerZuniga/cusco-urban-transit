#ifndef DB_WRAPPER_H
#define DB_WRAPPER_H

#include <string>
#include <vector>
#include <functional>

namespace urban_transport {

class DBWrapper {
public:
    using RowCallback = std::function<bool(const std::vector<std::string>&)>;
    virtual ~DBWrapper() = default;

    virtual bool open(const std::string& conn) = 0;
    virtual void close() = 0;
    virtual bool is_open() const = 0;

    virtual bool execute(const std::string& sql) = 0;
    virtual bool execute_with_params(const std::string& sql, const std::vector<std::string>& params) = 0;

    virtual bool query(const std::string& sql, RowCallback callback) const = 0;
    virtual bool query_with_params(const std::string& sql, const std::vector<std::string>& params, RowCallback callback) const = 0;

    virtual bool begin_transaction() = 0;
    virtual bool commit_transaction() = 0;
    virtual bool rollback_transaction() = 0;

    virtual std::string last_error() const = 0;
    virtual int last_error_code() const = 0;
};

} // namespace urban_transport

#endif // DB_WRAPPER_H
