/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "gtest/gtest.h"
#include "impl/cryptopp_impl.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

TEST(CryptoTest, Base64_Impl)
{
    std::string raw = "abc123!@#$%^&*()_-+=*/\\'\"?你好";
    Base64_Impl base64;

    std::string encode = base64.Encode(raw);
    ASSERT_NE(raw, encode);

    std::string decode = base64.Decode(encode);
    ASSERT_EQ(raw, decode);
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

    std::string raw = "abc123!@#$%^&*()_-+=*/\\'\"?你好";

    RSA_PKCS1v15_Impl rsa;
    rsa.SetPublicKey(pubKey);
    rsa.SetPrivateKey(privKey);

    std::string encode = rsa.Encrypt(raw);
    ASSERT_NE(raw, encode);

    std::string decode = rsa.Decrypt(encode);
    ASSERT_EQ(raw, decode);
}
