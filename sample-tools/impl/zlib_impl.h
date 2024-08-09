/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include "../interface/compression_interface.h"
#include <iomanip>
#include <sstream>
#include <cassert>

#include "zlib.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

class Zlib_Impl : public I_Compression
{
public:
    virtual std::string Compress(const std::string &data) override
    {
        if (data.empty())
            return "";

        uLongf compressedSize = compressBound(data.size() + 1024);
        std::string compressedData(compressedSize, '\0');

        int result;
        for (int i = 0; i < 20; ++i)
        {
            result = compress(reinterpret_cast<Bytef *>(&compressedData[0]), &compressedSize,
                              reinterpret_cast<const Bytef *>(data.data()), data.size());
            if (result == Z_BUF_ERROR)
            {
                /* 动态扩展缓冲区 */
                compressedSize *= 2;
                compressedData.resize(compressedSize, '\0');
                continue;
            }
            break;
        }

        if (result != Z_OK)
        {
            // throw std::runtime_error("Compression failed");
            return "";
        }

        compressedData.resize(compressedSize);
        return compressedData;
    }

    virtual std::string Uncompress(const std::string &data) override
    {
        if (data.empty())
            return "";

        uLongf uncompressedSize = compressBound(data.size() + 1024);
        std::string decompressedData(uncompressedSize, '\0');

        int result;
        for (int i = 0; i < 20; ++i)
        {
            result = uncompress(reinterpret_cast<Bytef *>(&decompressedData[0]), &uncompressedSize,
                                reinterpret_cast<const Bytef *>(data.data()), data.size());
            if (result == Z_BUF_ERROR)
            {
                /* 动态扩展缓冲区 */
                uncompressedSize *= 2;
                decompressedData.resize(uncompressedSize, '\0');
                continue;
            }
            break;
        }

        if (result != Z_OK)
        {
            // throw std::runtime_error("Decompression failed");
            return "";
        }

        decompressedData.resize(uncompressedSize);
        return decompressedData;
    }
};
