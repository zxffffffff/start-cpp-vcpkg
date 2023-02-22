/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#pragma once
#include "Common.h"

class ExamplePrivate;
class Example
{
public:
    Example();
    ~Example();

private:
    std::unique_ptr<ExamplePrivate> pri;
};
