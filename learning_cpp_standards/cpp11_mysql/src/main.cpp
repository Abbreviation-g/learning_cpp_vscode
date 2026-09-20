#include <iostream>

#include <mysql.h>

namespace {
    constexpr const char *kHost = "127.0.0.1";
    constexpr unsigned int kPort = 3306;
    constexpr const char *kDatabase = nullptr;
    constexpr const char *kUser = "root";
    constexpr const char *kPassword = "password";
} // namespace

int main() {
    MYSQL *connection = mysql_init(nullptr);
    if (connection == nullptr) {
        std::cerr << "mysql_init failed" << std::endl;
        return 1;
    }

    if (mysql_real_connect(connection, kHost, kUser, kPassword, kDatabase,
            kPort, nullptr, 0) == nullptr) {
        std::cerr << "MySQL connection failed: " << mysql_error(connection)
                  << std::endl;
        mysql_close(connection);
        return 1;
    }

    if (mysql_ping(connection) != 0) {
        std::cerr << "MySQL ping failed: " << mysql_error(connection)
                  << std::endl;
        mysql_close(connection);
        return 1;
    }

    std::cout << "MySQL connection succeeded."
              << " Host: " << kHost << ", Port: " << kPort;
    if (kDatabase != nullptr) {
        std::cout << ", Database: " << kDatabase;
    }
    std::cout << std::endl;

    std::cout << "Connection URL: mysql://" << kUser << "@" << kHost << ":"
              << kPort << "/" << (kDatabase == nullptr ? "" : kDatabase)
              << std::endl;

    if (mysql_query(connection, "SELECT VERSION()") != 0) {
        std::cerr << "SELECT VERSION() failed: " << mysql_error(connection)
                  << std::endl;
        mysql_close(connection);
        return 1;
    }

    MYSQL_RES *result = mysql_store_result(connection);
    if (result == nullptr) {
        std::cerr << "Failed to read MySQL version result: "
                  << mysql_error(connection) << std::endl;
        mysql_close(connection);
        return 1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row != nullptr && row[0] != nullptr) {
        std::cout << "MySQL version: " << row[0] << std::endl;
    }

    mysql_free_result(result);

    mysql_close(connection);
    return 0;
}