/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "Example.h"
#include "Example_p.h"

Example::Example()
    : pri(std::make_unique<ExamplePrivate>())
{
}
