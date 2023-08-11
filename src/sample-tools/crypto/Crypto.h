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

    static std::string RSAEncryptString(const std::string& pubFilename, const std::string& msg);
    static std::string RSAEncryptStringHex(const std::string& pubFilename, const std::string& msg);

    static std::string RSADecryptString(const std::string& privFilename, const std::string& msg);
    static std::string RSADecryptStringHex(const std::string& privFilename, const std::string& msg);

    static std::string RSASignString(const std::string& privFilename, const std::string& msg);
    static std::string RSASignStringHex(const std::string& privFilename, const std::string& msg);

    static std::string encryptAES(const std::string& str_in, const std::string& key);
    static std::string decryptAES(const std::string& str_in, const std::string& key); /* ecb模式 没有iv */
};
