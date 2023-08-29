/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <algorithm>
#include <string>

class I_Base64
{
public:
    virtual std::string Encode(const std::string &msg) = 0;
    virtual std::string Decode(const std::string &msg) = 0;
};

/* 密钥格式为PKCS#8，长度为1024，无密码
 * 支持分段加密（RSA/ECB/PKCS1Padding）
 * PEM格式
 */
class I_RSA_PKCS1v15
{
public:
    virtual void SetPublicKey(const std::string &content) = 0;  /* for Encrypt */
    virtual void SetPrivateKey(const std::string &content) = 0; /* for Decrypt & Sign */

    virtual std::string Encrypt(const std::string &msg) = 0;
    virtual std::string EncryptHex(const std::string &msg) = 0;

    virtual std::string Decrypt(const std::string &msg) = 0;
    virtual std::string DecryptHex(const std::string &msg) = 0;

    virtual std::string Sign(const std::string &msg) = 0;
    virtual std::string SignHex(const std::string &msg) = 0;
};

/* 支持ECB（AES/ECB/PKCS5Padding） */
class I_AES_ECB
{
public:
    virtual void SetKey(const std::string &content) = 0; /* for Encrypt & Decrypt */
    
    virtual std::string Encrypt(const std::string &msg) = 0;
    virtual std::string Decrypt(const std::string &msg) = 0;
};
