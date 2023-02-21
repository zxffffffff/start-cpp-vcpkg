/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#ifdef _WIN32
# ifndef SAMPLE_DYNAMIC_LIB_EXPORTS
#  define SAMPLE_DYNAMIC_LIB_API __declspec(dllexport)
# else
#  define SAMPLE_DYNAMIC_LIB_API __declspec(dllimport)
# endif
#else
# define SAMPLE_DYNAMIC_LIB_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
// todo
#ifdef __cplusplus
}
#endif //__cplusplus
