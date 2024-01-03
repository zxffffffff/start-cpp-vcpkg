/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/crypto_interface.h"
#include <iomanip>
#include <sstream>
#include <cassert>

#include "cryptopp/base64.h"
#include "cryptopp/aes.h"
#include "cryptopp/rsa.h"
#include "cryptopp/modes.h"
#include "cryptopp/files.h"
#include "cryptopp/hex.h"
#include "cryptopp/randpool.h"
#include "cryptopp/osrng.h"
#include "cryptopp/cryptlib.h"
#include <cryptopp/filters.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class Base64_Impl : public I_Base64
{
public:
    virtual std::string Encode(const std::string &msg) override
    {
        using namespace CryptoPP;

        std::string encode;
        StringSource a(msg, true, new Base64Encoder(new StringSink(encode), false));
        return encode;
    }

    virtual std::string Decode(const std::string &msg) override
    {
        using namespace CryptoPP;

        std::string decode;
        StringSource a(msg, true, new Base64Decoder(new StringSink(decode)));
        return decode;
    }
};

class RSA_PKCS1v15_Impl : public I_RSA_PKCS1v15
{
    CryptoPP::RandomPool randPool;
    CryptoPP::RSA::PublicKey publicKey;
    CryptoPP::RSA::PrivateKey privateKey;

    std::string &strReplace(std::string &input, const std::string &oldStr, const std::string &newStr)
    {
        size_t pos = 0;
        while ((pos = input.find(oldStr, pos)) != std::string::npos)
        {
            input.replace(pos, oldStr.length(), newStr);
            pos += newStr.length();
        }
        return input;
    }

    std::string crypto_PemToOneline(std::string pemKey)
    {
        strReplace(pemKey, "-----BEGIN PUBLIC KEY-----", "");
        strReplace(pemKey, "-----END PUBLIC KEY-----", "");
        strReplace(pemKey, "-----BEGIN PRIVATE KEY-----", "");
        strReplace(pemKey, "-----END PRIVATE KEY-----", "");
        strReplace(pemKey, "\r\n", "");
        strReplace(pemKey, "\n", "");
        strReplace(pemKey, " ", "");
        return pemKey;
    }

public:
    virtual bool SetPublicKey(const std::string &content, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            std::string pemPublicKey = crypto_PemToOneline(content);
            StringSource strSource(pemPublicKey, true, new Base64Decoder);
            publicKey.Load(strSource);
            return true;
        }
        catch (const std::exception &e)
        {
            if (err)
                *err = e.what();
            return false;
        }
    }

    virtual bool SetPrivateKey(const std::string &content, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            std::string pemPublicKey = crypto_PemToOneline(content);
            StringSource strSource(pemPublicKey, true, new Base64Decoder);
            privateKey.Load(strSource);
            return true;
        }
        catch (const std::exception &e)
        {
            if (err)
                *err = e.what();
            return false;
        }
    }

    virtual std::string Encrypt(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            RSAES_PKCS1v15_Encryptor pub(publicKey);

            std::string result;
            StringSource a(msg, true, new PK_EncryptorFilter(randPool, pub, new Base64Encoder(new StringSink(result), false)));
            if (ok)
                *ok = true;
            return result;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return "";
        }
    }

    virtual std::string EncryptHex(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            RSAES_PKCS1v15_Encryptor pub(publicKey);

            size_t nLen = msg.length();
            std::string result;
            size_t fixedLen = pub.FixedMaxPlaintextLength();
            for (int i = 0; i < nLen; i += fixedLen)
            {
                size_t len = fixedLen < (nLen - i) ? fixedLen : (nLen - i);
                std::string sPlain = msg.substr(i, len);
                std::string sOut;
                StringSource(sPlain, true, new PK_EncryptorFilter(randPool, pub, new StringSink(sOut)));
                result += sOut;
            }
            if (ok)
                *ok = true;
            return result;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return "";
        }
    }

    virtual std::string Decrypt(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            RSAES_PKCS1v15_Decryptor priv(privateKey);

            std::string result;
            StringSource a(msg, true, new Base64Decoder(new PK_DecryptorFilter(randPool, priv, new StringSink(result))));
            if (ok)
                *ok = true;
            return result;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return "";
        }
    }

    virtual std::string DecryptHex(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            RSAES_PKCS1v15_Decryptor priv(privateKey);

            size_t nLen = msg.length();
            std::string result;
            int fixedLen = priv.FixedCiphertextLength();
            for (int i = 0; i < nLen; i += fixedLen)
            {
                size_t len = fixedLen < (nLen - i) ? fixedLen : (nLen - i);
                std::string sOut;
                StringSource((byte *)(msg.c_str() + i), len, true, new PK_DecryptorFilter(randPool, priv, new StringSink(sOut)));
                result += sOut;
            }
            if (ok)
                *ok = true;
            return result;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return "";
        }
    }

    virtual std::vector<char> Sign(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            RSASSA_PKCS1v15_SHA_Signer signer(privateKey);

            size_t length = signer.MaxSignatureLength();
            SecByteBlock signature(length);

            size_t szlength = signer.SignMessage(randPool, (const byte *)msg.c_str(), msg.length(), signature);
            std::vector<char> result(szlength);
            memcpy(result.data(), signature.data(), szlength);
            if (ok)
                *ok = true;
            return result;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return {};
        }
    }

    virtual std::string SignHex(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            RSASSA_PKCS1v15_SHA_Signer signer(privateKey);
            
            size_t length = signer.MaxSignatureLength();
            SecByteBlock signature(length);

            size_t szlength = signer.SignMessage(randPool, (const byte *)msg.c_str(), msg.length(), signature);
            signature.resize(szlength);

            std::stringstream ss;
            ss << std::hex << std::uppercase;
            for (int i = 0; i < signature.size(); ++i)
            {
                ss << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(signature[i]));
            }
            if (ok)
                *ok = true;
            return ss.str();
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return "";
        }
    }

    virtual bool Verify(const std::string &msg, const std::vector<char> &sign, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            RSASSA_PKCS1v15_SHA_Verifier verifier(publicKey);
            bool result = verifier.VerifyMessage((const byte *)msg.data(), msg.size(), (const byte *)sign.data(), sign.size());
            if (ok)
                *ok = true;
            return result;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return false;
        }
    }

    virtual bool VerifyHex(const std::string &msg, const std::string &sign_hex, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            std::string sign;
            for (int i = 0; i < sign_hex.length(); i += 2)
            {
                std::string byteString = sign_hex.substr(i, 2);
                char byte = (char)strtol(byteString.c_str(), NULL, 16);
                sign.push_back(byte);
            }

            RSASSA_PKCS1v15_SHA_Verifier verifier(publicKey);
            bool result = verifier.VerifyMessage((const byte *)msg.data(), msg.size(), (const byte *)sign.data(), sign.size());
            if (ok)
                *ok = true;
            return result;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return false;
        }
    }
};

class AES_ECB_Impl : public I_AES_ECB
{
    std::string key;

public:
    virtual void SetKey(const std::string &content) override
    {
        key = content;
    }

    virtual std::string Encrypt(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            std::string str_out;
            ECB_Mode<AES>::Encryption encryption((unsigned char *)key.c_str(), key.length());
            StringSource encryptor(msg, true, new StreamTransformationFilter(encryption, new StringSink(str_out), BlockPaddingSchemeDef::PKCS_PADDING));
            if (ok)
                *ok = true;
            return str_out;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return "";
        }
    }

    virtual std::string Decrypt(const std::string &msg, bool *ok = nullptr, std::string *err = nullptr) override
    {
        using namespace CryptoPP;
        try
        {
            std::string str_out;
            ECB_Mode<AES>::Decryption decryption((unsigned char *)key.c_str(), key.length());
            StringSource decryptor(msg, true, new StreamTransformationFilter(decryption, new StringSink(str_out), BlockPaddingSchemeDef::PKCS_PADDING));
            if (ok)
                *ok = true;
            return str_out;
        }
        catch (const std::exception &e)
        {
            if (ok)
                *ok = false;
            if (err)
                *err = e.what();
            return "";
        }
    }
};
