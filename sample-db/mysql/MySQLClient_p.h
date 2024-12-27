/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "glog/logging.h"
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
#include <mysqlx/xdevapi.h>
#include "chrono.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

using namespace ::mysqlx;

/* https://dev.mysql.com/doc/dev/connector-cpp/8.0/devapi_ref.html
 * 支持最新的 X DevAPI（MySQL 5.7 以后默认在 33060 启用 X Protocol，使用 protobuf 优化传输）
 * 可以用 Session Object（coll.execute），也可以用 SQL（sess.sql）
 */
class MySQLClientPrivate
{
private:
    /* Client from_uri("mysqlx://user:pwd\@host:port/db?ssl-mode=disabled");
     * ---- or ----
     * SessionOption::USER, "user",
     * SessionOption::PWD,  "pwd",
     * SessionOption::HOST, "host",
     * SessionOption::PORT, port,
     * SessionOption::DB,   "db",
     * SessionOption::SSL_MODE, SSLMode::DISABLED
     * ClientOption::POOLING, true,
     * ClientOption::POOL_MAX_SIZE, 10,
     * ClientOption::POOL_QUEUE_TIMEOUT, 1000,
     * ClientOption::POOL_MAX_IDLE_TIME, 500,
     */
    Client client;

public:
    MySQLClientPrivate(const char *host, int port, const char *user, const char *pwd)
        : client(SessionOption::HOST, host, SessionOption::PORT, port,
                 SessionOption::USER, user, SessionOption::PWD, pwd,
                 ClientOption::POOLING, true, ClientOption::POOL_MAX_SIZE, 10)
    {
        CheckVersion();
    }

    ~MySQLClientPrivate()
    {
        client.close();
    }

    bool CheckVersion()
    {
        Session sess = client.getSession();
        RowResult res = sess.sql("show variables like 'version'").execute();
        std::stringstream version;

        version << res.fetchOne().get(1).get<std::string>();
        int major_version;
        version >> major_version;

        if (major_version < 8)
        {
            LOG(ERROR) << "[mysqlx error] version=" << major_version << " < 8";
            return false;
        }
        return true;
    }

    bool RunSQL(const std::string &sql, std::vector<std::vector<std::string>> &ret)
    {
        LOG(INFO) << "[RunSQL sql] " << sql;
        try
        {
            Session sess = client.getSession();
            Chrono chrono;
            RowResult res = sess.sql(sql.c_str()).execute();
            auto use_time = chrono.stop();
            auto rows = res.fetchAll();

            ret.clear();
            for (auto row : rows)
            {
                std::vector<std::string> fields;
                for (size_t i = 0; i < row.colCount(); ++i)
                {
                    auto field = row.get(i);
                    auto type = field.getType();
                    std::string s_val;
                    switch (type)
                    {
                    case mysqlx::abi2::r0::Value::UINT64:
                        s_val = std::to_string(field.get<uint64_t>());
                        break;
                    case mysqlx::abi2::r0::Value::INT64:
                        s_val = std::to_string(field.get<int64_t>());
                        break;
                    case mysqlx::abi2::r0::Value::FLOAT:
                        s_val = std::to_string(field.get<float>());
                        break;
                    case mysqlx::abi2::r0::Value::DOUBLE:
                        s_val = std::to_string(field.get<double>());
                        break;
                    case mysqlx::abi2::r0::Value::BOOL:
                        s_val = field.get<bool>() ? "1" : "0";
                        break;
                    case mysqlx::abi2::r0::Value::STRING:
                        s_val = field.get<std::string>();
                        break;
                    case mysqlx::abi2::r0::Value::RAW:
                        s_val = field.get<std::string>();
                        break;
                    default:
                        LOG(WARNING) << "[RunSQL warning] type=" << type;
                        break;
                    }
                    fields.push_back(s_val);
                }
                ret.push_back(std::move(fields));
            }
            LOG(INFO) << "[RunSQL res] use_time=" << use_time << "ms size=" << ret.size();
            return true;
        }
        catch (const std::exception &e)
        {
            LOG(ERROR) << "[RunSQL error] " << e.what();
            return false;
        }
    }
};
