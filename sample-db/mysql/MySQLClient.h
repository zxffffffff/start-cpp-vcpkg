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

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

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
