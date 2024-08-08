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
#include <vector>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

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
    virtual bool SetPublicKey(const std::string &content, std::string *err = nullptr) = 0;  /* for Encrypt */
    virtual bool SetPrivateKey(const std::string &content, std::string *err = nullptr) = 0; /* for Decrypt & Sign */

    virtual std::string Encrypt(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) = 0;
    virtual std::string EncryptHex(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) = 0;

    virtual std::string Decrypt(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) = 0;
    virtual std::string DecryptHex(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) = 0;

    virtual std::vector<char> Sign(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) = 0;
    virtual std::string SignHex(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) = 0;

    virtual bool Verify(const std::string &msg, const std::vector<char> &sign, bool *ok = nullptr, std::string *err = nullptr) = 0;
    virtual bool VerifyHex(const std::string &msg, const std::string &sign, bool *ok = nullptr, std::string *err = nullptr) = 0;
};

/* 支持ECB（AES/ECB/PKCS5Padding） */
class I_AES_ECB
{
public:
    /* for Encrypt & Decrypt */
    virtual void SetKey(const std::string &content) = 0;

    virtual std::string Encrypt(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) = 0;
    virtual std::string Decrypt(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) = 0;
};
