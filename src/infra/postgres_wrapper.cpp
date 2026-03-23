#include "infra/postgres_wrapper.h"
#include "infra/logger.h"
#include <sstream>

using namespace urban_transport;

PostgresWrapper::PostgresWrapper() : conn_(nullptr) {}

PostgresWrapper::~PostgresWrapper() {
    close();
}

bool PostgresWrapper::open(const std::string& conn) {
    if (conn_) {
        Logger::get_instance().warning("La base de datos ya está abierta (Postgres)");
        return true;
    }

    conn_ = PQconnectdb(conn.c_str());
    if (PQstatus(conn_) != CONNECTION_OK) {
        Logger::get_instance().error(std::string("Error al conectar a Postgres: ") + PQerrorMessage(conn_));
        PQfinish(conn_);
        conn_ = nullptr;
        return false;
    }

    Logger::get_instance().info("Conectado a Postgres");
    return true;
}

void PostgresWrapper::close() {
    if (conn_) {
        PQfinish(conn_);
        conn_ = nullptr;
        Logger::get_instance().info("Conexión Postgres cerrada");
    }
}

bool PostgresWrapper::is_open() const {
    return conn_ != nullptr && PQstatus(const_cast<PGconn*>(conn_)) == CONNECTION_OK;
}

bool PostgresWrapper::execute(const std::string& sql) {
    if (!is_open()) {
        Logger::get_instance().error("Postgres no está conectado");
        return false;
    }

    PGresult* res = PQexec(conn_, sql.c_str());
    if (!res) return false;
    ExecStatusType status = PQresultStatus(res);
    bool ok = (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK);
    if (!ok) {
        Logger::get_instance().error(std::string("Error execute Postgres: ") + PQresultErrorMessage(res));
    }
    PQclear(res);
    return ok;
}

bool PostgresWrapper::execute_with_params(const std::string& sql, const std::vector<std::string>& params) {
    if (!is_open()) {
        Logger::get_instance().error("Postgres no está conectado");
        return false;
    }

    std::vector<const char*> paramValues;
    paramValues.reserve(params.size());
    for (const auto& p : params) paramValues.push_back(p.c_str());

    PGresult* res = PQexecParams(conn_, sql.c_str(), static_cast<int>(paramValues.size()), nullptr, paramValues.data(), nullptr, nullptr, 0);
    if (!res) return false;
    ExecStatusType status = PQresultStatus(res);
    bool ok = (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK);
    if (!ok) {
        Logger::get_instance().error(std::string("Error execute_with_params Postgres: ") + PQresultErrorMessage(res));
    }
    PQclear(res);
    return ok;
}

bool PostgresWrapper::query(const std::string& sql, RowCallback callback) const {
    if (!is_open()) {
        Logger::get_instance().error("Postgres no está conectado");
        return false;
    }

    PGresult* res = PQexec(const_cast<PGconn*>(conn_), sql.c_str());
    if (!res) return false;
    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_TUPLES_OK) {
        Logger::get_instance().error(std::string("Error query Postgres: ") + PQresultErrorMessage(res));
        PQclear(res);
        return false;
    }

    int nrows = PQntuples(res);
    int ncols = PQnfields(res);
    bool cont = true;
    for (int r = 0; r < nrows && cont; ++r) {
        std::vector<std::string> row;
        row.reserve(ncols);
        for (int c = 0; c < ncols; ++c) {
            char* val = PQgetvalue(res, r, c);
            row.emplace_back(val ? val : "");
        }
        cont = callback(row);
    }

    PQclear(res);
    return true;
}

bool PostgresWrapper::query_with_params(const std::string& sql, const std::vector<std::string>& params, RowCallback callback) const {
    if (!is_open()) {
        Logger::get_instance().error("Postgres no está conectado");
        return false;
    }

    std::vector<const char*> paramValues;
    paramValues.reserve(params.size());
    for (const auto& p : params) paramValues.push_back(p.c_str());

    PGresult* res = PQexecParams(const_cast<PGconn*>(conn_), sql.c_str(), static_cast<int>(paramValues.size()), nullptr, paramValues.data(), nullptr, nullptr, 0);
    if (!res) return false;
    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_TUPLES_OK) {
        Logger::get_instance().error(std::string("Error query_with_params Postgres: ") + PQresultErrorMessage(res));
        PQclear(res);
        return false;
    }

    int nrows = PQntuples(res);
    int ncols = PQnfields(res);
    bool cont = true;
    for (int r = 0; r < nrows && cont; ++r) {
        std::vector<std::string> row;
        row.reserve(ncols);
        for (int c = 0; c < ncols; ++c) {
            char* val = PQgetvalue(res, r, c);
            row.emplace_back(val ? val : "");
        }
        cont = callback(row);
    }

    PQclear(res);
    return true;
}

bool PostgresWrapper::begin_transaction() {
    return execute("BEGIN;");
}

bool PostgresWrapper::commit_transaction() {
    return execute("COMMIT;");
}

bool PostgresWrapper::rollback_transaction() {
    return execute("ROLLBACK;");
}

std::string PostgresWrapper::last_error() const {
    return conn_ ? PQerrorMessage(const_cast<PGconn*>(conn_)) : std::string("Not connected");
}

int PostgresWrapper::last_error_code() const {
    return conn_ ? 0 : -1;
}
