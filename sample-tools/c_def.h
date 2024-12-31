/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#ifndef C_DEF_H
#define C_DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* MSVC 不支持变长数组 VLA (Variable Length Array) */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L && !defined(_MSC_VER)
#define USE_VLA 1
#else
#define USE_VLA 0
#endif

/* 创建一维数组（指针）
 * 注意 sizeof(arr) == sizeof(void *) 而不是数组总大小
 * 注意使用 memcpy(&arr[0]) 而不是 memcpy(arr)
 */
static inline void *MALLOC_ARRAY_1D(int size, size_t item_size)
{
    assert(size > 0 && item_size > 0);
    void *data = malloc(size * item_size);
    assert(data);
    return data;
}

/* 一维数组释放 */
static inline void FREE_ARRAY_1D(void *arr)
{
    assert(arr);
    free(arr);
}

/* 创建二维数组（指针）
 * 注意 sizeof(arr) == sizeof(void *) 而不是数组总大小
 * 注意使用 memcpy(&arr[0][0]) 而不是 memcpy(arr)
 */
static inline void *MALLOC_ARRAY_2D(int rows, int cols, size_t item_size)
{
    assert(rows > 0 && cols > 0 && item_size > 0);
    void *data = malloc(rows * cols * item_size);
    assert(data);

    void **arr = (void **)malloc(rows * sizeof(void *));
    assert(arr);
    for (int i = 0; i < rows; i++)
    {
        arr[i] = (char *)data + i * cols * item_size;
    }
    return arr;
}

/* 二维数组释放 */
static inline void FREE_ARRAY_2D(void *arr)
{
    assert(arr);
    void **arr_ptr = (void **)arr;
    free(arr_ptr[0]);
    free(arr_ptr);
}

#if USE_VLA
#define NEW_ARRAY(type, array, size) type array[size];
#define NEW_ARRAY_2D(type, array, rows, cols) type array[rows][cols];
#define DELETE_ARRAY(array) (void)0;
#define DELETE_ARRAY_2D(array) (void)0;
#else /* !USE_VLA */
#define NEW_ARRAY(type, array, size) type *array = (type *)MALLOC_ARRAY_1D(size, sizeof(type));
#define NEW_ARRAY_2D(type, array, rows, cols) type **array = (type **)MALLOC_ARRAY_2D(rows, cols, sizeof(type));
#define DELETE_ARRAY(array) FREE_ARRAY_1D(array)
#define DELETE_ARRAY_2D(array) FREE_ARRAY_2D(array)
#endif /* USE_VLA */

#endif /* C_DEF_H */
