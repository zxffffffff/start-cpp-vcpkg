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
#pragma warning(disable : 4566)
#endif

/* 状态流转参考 doc */
enum class ServerState : int
{
    Closed,  /* 初始状态 */
    Closing, /* -> Closed */

    Listen,       /* -> */
    Listening,    /* success */
    ListenFailed, /* err */
};

enum class ConnectionState : int
{
    Closed,  /* 初始状态 */
    Closing, /* -> Closed */

    Connecting, /* -> */
    Connected,  /* success */
    ConnFailed, /* err */

    // InitRequest, /* -> */
    // Inited,      /* success */
    // InitFailed,  /* err */

    Shutdown, /* 读写异常、心跳超时 -> Closing */
};

inline bool CheckIsRunning(ServerState state)
{
    switch (state)
    {
    case ServerState::Closed:
    case ServerState::Closing:
    case ServerState::ListenFailed:
        return false;
    case ServerState::Listen:
    case ServerState::Listening:
        return true;
    }
    return false;
}

inline bool CheckIsRunning(ConnectionState state)
{
    switch (state)
    {
    case ConnectionState::Closed:
    case ConnectionState::Closing:
    case ConnectionState::ConnFailed:
    case ConnectionState::Shutdown:
        return false;
    case ConnectionState::Connecting:
    case ConnectionState::Connected:
        return true;
    }
    return false;
}

inline const char *ToString(ServerState state)
{
    switch (state)
    {
    case ServerState::Closed:
        return "Closed";
    case ServerState::Closing:
        return "Closing";
    case ServerState::Listen:
        return "Listen";
    case ServerState::Listening:
        return "Listening";
    case ServerState::ListenFailed:
        return "ListenFailed";
    }
    return "???";
}

inline const char *ToString(ConnectionState state)
{
    switch (state)
    {
    case ConnectionState::Closed:
        return "Closed";
    case ConnectionState::Closing:
        return "Closing";
    case ConnectionState::Connecting:
        return "Connecting";
    case ConnectionState::Connected:
        return "Connected";
    case ConnectionState::ConnFailed:
        return "ConnFailed";
    case ConnectionState::Shutdown:
        return "Shutdown";
    }
    return "???";
}
