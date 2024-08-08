/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"
#include "impl/cryptopp_impl.h"
#include "common.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(CryptoTest, Base64_Impl)
{
    Base64_Impl base64;

    std::string msg = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    for (int i = 0; i < 4 * 1024; ++i)
    {
        std::string encode = base64.Encode(msg);
        ASSERT_NE(msg, encode);

        std::string decode = base64.Decode(encode);
        ASSERT_EQ(msg, decode);

        msg.insert(msg.begin(), i);
        msg.push_back(i);
    }
}

TEST(CryptoTest, RSA_PKCS1v15_Impl)
{
    std::string pubKey = R"(-----BEGIN PUBLIC KEY-----
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCsqNBWIymiUoxTNRJaeTsNRcVs
3b2ZaUWWvBg5HddCdvEomOM9CeaKxg/nIcb3HjcWNnLoirmLvb1VrVb7Mk0MUp4c
TGDjEXbDaRYY9SasUnsuJfa+xp3jCCZd+XvdhZrEw75V4XWnKmbCPWxYmFgAfwv5
GID0M1CSjCMrMz4mFQIDAQAB
-----END PUBLIC KEY-----
)";

    std::string privKey = R"(-----BEGIN PRIVATE KEY-----
MIICdgIBADANBgkqhkiG9w0BAQEFAASCAmAwggJcAgEAAoGBAKyo0FYjKaJSjFM1
Elp5Ow1FxWzdvZlpRZa8GDkd10J28SiY4z0J5orGD+chxvceNxY2cuiKuYu9vVWt
VvsyTQxSnhxMYOMRdsNpFhj1JqxSey4l9r7GneMIJl35e92FmsTDvlXhdacqZsI9
bFiYWAB/C/kYgPQzUJKMIyszPiYVAgMBAAECgYEAq2EJImbWnZtoU/Ma25j3x23Y
z/2tg4gdncfoF1GKaBudthlDfUPFhTuhb190mbdxIGg9WJrSH2XfeBqNetrr48zq
39gMSY906SBw1rwB3/Fv0SW/q/NpTEGcFzaQIScw7RuPhVVslHT9EJuJjr24CAXs
iDGiE3DnLX6Kd5TtE0ECQQDWzzDNkSiU+3ljv2Hgi49o8xlLxqeJLVJb0TfdBmzT
7GqYRbv1SaEDojGZ5kTyjwtMDRoPel4O9m/wM7KfZCDxAkEAzcSFB8QPxlG0qALw
L/NPDTXqmpTERizdv3IC+Dp1O/ZAFZB4P23X97PgbRTEs/IDPKKUENQZaxSDT2uM
hj+XZQJAezlx2orJx+UWdqUD5cbJZsAFo9V6qBOKqkczj6Qu68c0QtPGBejN6AK0
B3/zz11FcsQVTgm5SUYPYrCVaCANUQJAF67RvOYVBrdqupez+2DerV+SQ/5xZL5P
O3Ss4NMhoSgjkhfRr0M+QzVB1Gw6g5eMRdNUhWdUEZ4HywFO6DizfQJAYQ7pO9H9
h2kQWhkh8JaeT0MZ4WA64Sdy98DJ4aDWe/c6KAha1K0a6f8dS2dkUtWrjqN5lkYH
Wbi0yNLZmHjrTg==
-----END PRIVATE KEY-----
)";

    RSA_PKCS1v15_Impl rsa;
    rsa.SetPublicKey(pubKey);
    rsa.SetPrivateKey(privKey);

    std::string msg = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    bool ok;
    std::string err;
    for (int i = 0; i < 600; ++i)
    {
        // RSA/EME-PKCS1-v1_5: message length of 118 exceeds the maximum of 117 for this public key
        if (msg.size() <= 117)
        {
            std::string encode = rsa.Encrypt(msg, &ok, &err);
            ASSERT_TRUE(ok) << err;
            ASSERT_NE(msg, encode);

            std::string decode = rsa.Decrypt(encode, &ok, &err);
            ASSERT_TRUE(ok) << err;
            ASSERT_EQ(msg, decode);
        }

        std::string encode = rsa.EncryptHex(msg, &ok, &err);
        ASSERT_TRUE(ok) << err;
        ASSERT_NE(msg, encode);

        std::string decode = rsa.DecryptHex(encode, &ok, &err);
        ASSERT_TRUE(ok) << err;
        ASSERT_EQ(msg, decode);

        std::vector<char> sign = rsa.Sign(msg, &ok, &err);
        ASSERT_TRUE(ok) << err;
        ASSERT_TRUE(sign.size());

        bool verify = rsa.Verify(msg, sign, &ok, &err);
        ASSERT_TRUE(ok) << err;
        ASSERT_TRUE(verify);

        std::string signHex = rsa.SignHex(msg, &ok, &err);
        ASSERT_TRUE(ok) << err;
        ASSERT_TRUE(signHex.size());

        bool verify2 = rsa.VerifyHex(msg, signHex, &ok, &err);
        ASSERT_TRUE(ok) << err;
        ASSERT_TRUE(verify2);

        msg.insert(msg.begin(), i);
        msg.push_back(i);
    }
}

TEST(CryptoTest, AES_ECB_Impl)
{
    Base64_Impl base64;
    std::string keyBase64 = "m+qS04/2CH1OweCnmXZ3TDZkCQS+hBzY";
    std::string key = base64.Decode(keyBase64);

    AES_ECB_Impl aes;
    aes.SetKey(key);

    std::string msg = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    bool ok;
    std::string err;
    for (int i = 0; i < 4 * 1024; ++i)
    {
        std::string encrypt = aes.Encrypt(msg, &ok, &err);
        ASSERT_TRUE(ok) << err;
        ASSERT_NE(msg, encrypt);

        std::string decrypt = aes.Decrypt(encrypt, &ok, &err);
        ASSERT_TRUE(ok) << err;
        ASSERT_EQ(msg, decrypt);

        msg.insert(msg.begin(), i);
        msg.push_back(i);
    }
}
