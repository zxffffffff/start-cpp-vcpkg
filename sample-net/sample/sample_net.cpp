/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "tcp_client.h"
#include "impl/libuv_tcp_client_impl.h"

#include "tcp_server.h"
#include "impl/libuv_tcp_server_impl.h"

#include "http_server.h"
#include "impl/boost_http_parser.h"

#include "http_client.h"
#include "impl/curl_http_client.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif
