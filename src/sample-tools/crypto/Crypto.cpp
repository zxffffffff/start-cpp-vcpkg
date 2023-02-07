#include "Crypto.h"
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

using namespace CryptoPP;

std::string Crypto::Base64Encode(const std::string& msg)
{
	std::string encode;
	StringSource a(msg, true, new Base64Encoder(new StringSink(encode)));
	return encode;
}

std::string Crypto::Base64Decode(const std::string& msg)
{
	std::string decode;
	StringSource a(msg, true, new Base64Decoder(new StringSink(decode)));
	return decode;
}

std::string Crypto::RSAEncrypt(const std::string& pubFilename, const std::string& msg)
{
	FileSource pubFile(pubFilename.c_str(), true, new Base64Decoder);
	RSAES_PKCS1v15_Encryptor pub(pubFile);

	std::string strSeed = "seed";
	RandomPool randPool;
	randPool.IncorporateEntropy((unsigned char*)strSeed.c_str(), strSeed.length());

    std::string result;
    try
    {
        StringSource a(msg, true, new PK_EncryptorFilter(randPool, pub, new Base64Encoder(new StringSink(result))));
    }
    catch (const std::exception& e)
    {
        //e.what();
    }
    return result;
}

std::string Crypto::RSADecrypt(const std::string& privFilename, const std::string& msg)
{
	FileSource privFile(privFilename.c_str(), true, new Base64Decoder);
	RSAES_PKCS1v15_Decryptor priv(privFile);

	std::string strSeed = "seed";
	RandomPool randPool;
	randPool.IncorporateEntropy((unsigned char*)strSeed.c_str(), 0);

	std::string result;
	try
	{
		StringSource a(msg, true, new Base64Decoder(new PK_DecryptorFilter(randPool, priv, new StringSink(result))));
	}
	catch (const std::exception& e)
	{
		//e.what();
	}
	return result;
}
