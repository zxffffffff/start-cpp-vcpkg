/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#pragma once

#include <string>

class Crypto
{
public:
    static std::string Base64Encode(const std::string& msg);
    static std::string Base64Decode(const std::string& msg);

    static std::string RSAEncrypt(const std::string& pubFilename, const std::string& msg);
    static std::string RSADecrypt(const std::string& privFilename, const std::string& msg);
};
