/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#pragma once
#include <algorithm>
#include <string>

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

/* 状态流转参考 doc */
enum class ServerStates : int
{
    Closed,         /* 初始状态 */
    Closing,        /* -> Closed */

    Listen,         /* -> */
    Listening,      /* success */
    ListenFailed,   /* err */
};

enum class ConnectionStates : int
{
    Closed,         /* 初始状态 */
    Closing,        /* -> Closed */

    Connecting,     /* -> */
    Connected,      /* success */
    ConnFailed,     /* err */

    // InitRequest, /* -> */
    // Inited,      /* success */
    // InitFailed,  /* err */

    Shutdown,       /* 读写异常、心跳超时 -> Closing */
};

inline bool CheckIsRunning(ServerStates state)
{
    switch (state)
    {
    case ServerStates::Closed:       return false;
    case ServerStates::Closing:      return false;
    case ServerStates::Listen:       return true;
    case ServerStates::Listening:    return true;
    case ServerStates::ListenFailed: return false;
    default:                         return false;
    }
}

inline bool CheckIsRunning(ConnectionStates state)
{
    switch (state)
    {
    case ConnectionStates::Closed:         return false;
    case ConnectionStates::Closing:        return false;
    case ConnectionStates::Connecting:     return true;
    case ConnectionStates::Connected:      return true;
    case ConnectionStates::ConnFailed:     return false;
    // case ConnectionStates::InitRequest: return true;
    // case ConnectionStates::Inited:      return true;
    // case ConnectionStates::InitFailed:  return false;
    case ConnectionStates::Shutdown:       return false;
    default:                               return false;
    }
}

inline const char* ToString(ServerStates state)
{
    switch (state)
    {
    case ServerStates::Closed:       return "Closed";
    case ServerStates::Closing:      return "Closing";
    case ServerStates::Listen:       return "Listen";
    case ServerStates::Listening:    return "Listening";
    case ServerStates::ListenFailed: return "ListenFailed";
    default:                         return "???";
    }
}

inline const char* ToString(ConnectionStates state)
{
    switch (state)
    {
    case ConnectionStates::Closed:         return "Closed";
    case ConnectionStates::Closing:        return "Closing";
    case ConnectionStates::Connecting:     return "Connecting";
    case ConnectionStates::Connected:      return "Connected";
    case ConnectionStates::ConnFailed:     return "ConnFailed";
    // case ConnectionStates::InitRequest: return "InitRequest";
    // case ConnectionStates::Inited:      return "Inited";
    // case ConnectionStates::InitFailed:  return "InitFailed";
    case ConnectionStates::Shutdown:       return "Shutdown";
    default:                               return "???";
    }
}
