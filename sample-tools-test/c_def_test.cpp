/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include <gtest/gtest.h>
#include "c_def.h"
#include "cpp_def.h"
#include "fmt/format.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable : 4566)
#endif

TEST(c_def, MALLOC_ARRAY)
{
    constexpr int size = 12;

    int32_t arr[size];
    int32_t *malloc_arr = (int32_t *)MALLOC_ARRAY_1D(size, sizeof(int32_t));

    for (int i = 0; i < size; ++i)
    {
        arr[i] = (i + 1);
    }
    memmove(&malloc_arr[0], arr, sizeof(arr));

    fmt::print("size={}, item_size={}, arr={}({:#x}), malloc_arr={}({:#x}) \n",
               size,
               sizeof(int32_t),
               sizeof(arr), size_t(arr),
               sizeof(malloc_arr), size_t(malloc_arr));
    for (int i = 0; i < size; ++i)
    {
        fmt::print("arr[{}]={}({:#x}), malloc_arr[{}]={}({:#x}) \n",
                   i, arr[i], size_t(&arr[i]),
                   i, malloc_arr[i], size_t(&malloc_arr[i]));
        EXPECT_EQ(arr[i], malloc_arr[i]);
    }

    FREE_ARRAY_1D(malloc_arr);
}

TEST(c_def, MALLOC_ARRAY_2D)
{
    constexpr int rows = 4;
    constexpr int cols = 3;

    int32_t arr[rows][cols];
    int32_t **malloc_arr = (int32_t **)MALLOC_ARRAY_2D(rows, cols, sizeof(int32_t));

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            arr[i][j] = 10000 * (i + 1) + (j + 1);
        }
    }
    memmove(&malloc_arr[0][0], arr, sizeof(arr));

    fmt::print("size={}*{}, item_size={}, arr={}({:#x}), malloc_arr={}({:#x}) \n",
               rows, cols,
               sizeof(int32_t),
               sizeof(arr), size_t(arr),
               sizeof(malloc_arr), size_t(malloc_arr));
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            fmt::print("arr[{}][{}]={}({:#x}), malloc_arr[{}][{}]={}({:#x}) \n",
                       i, j, arr[i][j], size_t(&arr[i][j]),
                       i, j, malloc_arr[i][j], size_t(&malloc_arr[i][j]));
            EXPECT_EQ(arr[i][j], malloc_arr[i][j]);
        }
    }

    FREE_ARRAY_2D(malloc_arr);
}
