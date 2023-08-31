/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "MySQLClient.h"
#include "MySQLClient_p.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

MySQLClient::MySQLClient(const char* host, int port, const char* user, const char* pwd)
    : pri(std::make_unique<MySQLClientPrivate>(host, port, user, pwd))
{
}

MySQLClient::~MySQLClient()
{
}

bool MySQLClient::RunSQL(const std::string& sql)
{
    std::vector<std::vector<std::string>> ret;
    return pri->RunSQL(sql, ret);
}

bool MySQLClient::RunSQL(const std::string& sql, std::vector<std::vector<std::string>>& ret)
{
    return pri->RunSQL(sql, ret);
}
