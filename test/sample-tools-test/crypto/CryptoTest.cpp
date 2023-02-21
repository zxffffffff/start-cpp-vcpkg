/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "crypto/Crypto.h"
#include "gtest/gtest.h"

TEST(CryptoTest, EncodeAndDecode) {
    std::string raw = "abc123!@#$%^&*()_-+=*/\\'\"?ÄãºÃ";

    std::string encode = Crypto::Base64Encode(raw);
    ASSERT_NE(raw, encode);

    std::string decode = Crypto::Base64Decode(encode);
    ASSERT_EQ(raw, decode);
}
