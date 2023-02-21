/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "MySQLClient.h"
#include "MySQLClient_p.h"

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
