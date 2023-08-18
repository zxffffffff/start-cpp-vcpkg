/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "cryptopp/aes.h"
#include "cryptopp/base64.h"
#include "cryptopp/cbcmac.h"
#include "cryptopp/seckey.h"
#include "cryptopp/rsa.h"
#include "cryptopp/modes.h"
#include "cryptopp/files.h"
#include "cryptopp/hex.h"
#include "cryptopp/randpool.h"
#include "cryptopp/osrng.h"
#include "cryptopp/cryptlib.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

class Crypto
{
public:
    static std::string Base64Encode(const std::string &msg)
    {
        using namespace CryptoPP;

        std::string encode;
        StringSource a(msg, true, new Base64Encoder(new StringSink(encode)));
        return encode;
    }
    static std::string Base64Decode(const std::string &msg)
    {
        using namespace CryptoPP;

        std::string decode;
        StringSource a(msg.c_str(), true, new Base64Decoder(new StringSink(decode)));
        return decode;
    }

    static std::string RSAEncryptString(const std::string &pubFilename, const std::string &msg)
    {
        using namespace CryptoPP;

        FileSource pubFile(pubFilename.c_str(), true, new Base64Decoder);
        RSAES_PKCS1v15_Encryptor pub(pubFile);

        std::string strSeed = "seed";
        RandomPool randPool;
        randPool.IncorporateEntropy((unsigned char *)strSeed.c_str(), strSeed.length());

        try
        {
            std::string result;
            StringSource a(msg.c_str(), true, new PK_EncryptorFilter(randPool, pub, new Base64Encoder(new StringSink(result))));
            return result;
        }
        catch (const std::exception &e)
        {
            //LOG(ERROR) << __func__
            //           << " err=" << e.what();
            return "";
        }
    }
    static std::string RSAEncryptStringHex(const std::string &pubFilename, const std::string &msg)
    {
        using namespace CryptoPP;

        FileSource pubFile(pubFilename.c_str(), true, new Base64Decoder);
        RSAES_PKCS1v15_Encryptor pub(pubFile);

        std::string strSeed = "seed";
        RandomPool randPool;
        randPool.IncorporateEntropy((unsigned char *)strSeed.c_str(), strSeed.length());

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
        return result;
    }

    static std::string RSADecryptString(const std::string &privFilename, const std::string &msg)
    {
        using namespace CryptoPP;

        FileSource privFile(privFilename.c_str(), true, new Base64Decoder);
        RSAES_PKCS1v15_Decryptor priv(privFile);

        std::string strSeed = "seed";
        RandomPool randPool;
        randPool.IncorporateEntropy((unsigned char *)strSeed.c_str(), 0);

        try
        {
            std::string result;
            StringSource a(msg.c_str(), true, new Base64Decoder(new PK_DecryptorFilter(randPool, priv, new StringSink(result))));
            return result;
        }
        catch (const std::exception &e)
        {
            //LOG(ERROR) << __func__
            //           << " err=" << e.what();
            return "";
        }
    }
    static std::string RSADecryptStringHex(const std::string &privFilename, const std::string &msg)
    {
        using namespace CryptoPP;

        FileSource privFile(privFilename.c_str(), true, new Base64Decoder);
        RSAES_PKCS1v15_Decryptor priv(privFile);

        std::string strSeed = "seed";
        RandomPool randPool;
        randPool.IncorporateEntropy((unsigned char *)strSeed.c_str(), 0);

        size_t nLen = msg.length();
        std::string result;
        int fixedLen = priv.FixedCiphertextLength();
        for (int i = 0; i < nLen; i += fixedLen)
        {
            try
            {
                size_t len = fixedLen < (nLen - i) ? fixedLen : (nLen - i);
                std::string sOut;
                StringSource((byte *)(msg.c_str() + i), len, true, new PK_DecryptorFilter(randPool, priv, new StringSink(sOut)));
                result += sOut;
            }
            catch (const std::exception &e)
            {
                //LOG(ERROR) << __func__
                //           << " err=" << e.what();
                return "";
            }
        }
        return result;
    }

    static std::string RSASignString(const std::string &privFilename, const std::string &msg)
    {
        using namespace CryptoPP;

        FileSource privFile(privFilename.c_str(), true, new Base64Decoder);
        RSASSA_PKCS1v15_SHA_Signer signer(privFile);
        size_t length = signer.MaxSignatureLength();
        SecByteBlock signature(length);

        std::string strSeed = "seed";
        RandomPool randPool;
        randPool.IncorporateEntropy((unsigned char *)strSeed.c_str(), 0);

        size_t szlength = signer.SignMessage(randPool, (const byte *)msg.c_str(), msg.length(), signature);
        signature.resize(szlength);
        return std::string((const char *)signature.data(), signature.size());
    }
    static std::string RSASignStringHex(const std::string &privFilename, const std::string &msg)
    {
        using namespace CryptoPP;

        FileSource privFile(privFilename.c_str(), true, new Base64Decoder);
        RSASSA_PKCS1v15_SHA_Signer signer(privFile);
        size_t length = signer.MaxSignatureLength();
        SecByteBlock signature(length);

        std::string strSeed = "seed";
        RandomPool randPool;
        randPool.IncorporateEntropy((unsigned char *)strSeed.c_str(), 0);

        size_t szlength = signer.SignMessage(randPool, (const byte *)msg.c_str(), msg.length(), signature);
        signature.resize(szlength);
        // return std::string((const char *)signature.data(), signature.size());

        std::stringstream ss;
        ss << std::hex << std::uppercase;
        for (int i = 0; i < signature.size(); ++i)
        {
            ss << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(signature[i]));
        }
        return ss.str();
    }

    static std::string encryptAES(const std::string &str_in, const std::string &key)
    {
        using namespace CryptoPP;

        std::string str_out;
        ECB_Mode<AES>::Encryption encryption((unsigned char *)key.c_str(), key.length());
        StringSource encryptor(str_in, true, new StreamTransformationFilter(encryption, new StringSink(str_out)));
        return str_out;
    }
    static std::string decryptAES(const std::string &str_in, const std::string &key) /* ecb模式 没有iv */
    {
        using namespace CryptoPP;

        std::string str_out;
        ECB_Mode<AES>::Decryption decryption((unsigned char *)key.c_str(), key.length());
        StringSource decryptor((byte *)str_in.c_str(), str_in.length(), true, new StreamTransformationFilter(decryption, new StringSink(str_out)));
        return str_out;
    }
};
