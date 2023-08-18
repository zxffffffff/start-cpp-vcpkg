/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <memory>
#include <list>
#include <map>
#include <unordered_map>
#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <iostream>
#include <sstream>
#include <chrono>

class MySQLClientPrivate;
class MySQLClient
{
public:
    /* [阻塞] 创建一个 MySQL 连接
     * host = "localhost"
     * port = 33060
     * user = "root"
     * pwd = "123456"
     */
    MySQLClient(const char* host, int port, const char* user, const char* pwd);
    ~MySQLClient();

    // Create Read Update Delete
    bool RunSQL(const std::string& sql);
    bool RunSQL(const std::string& sql, std::vector<std::vector<std::string>>& ret);

private:
    std::unique_ptr<MySQLClientPrivate> pri;
};
