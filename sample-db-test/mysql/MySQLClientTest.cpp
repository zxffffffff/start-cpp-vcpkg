/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/
#include "mysql/MySQLClient.h"
#include "gtest/gtest.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1900)
/* msvc兼容utf-8: https://support.microsoft.com/en-us/kb/980263 */
#if (_MSC_VER >= 1700)
#pragma execution_character_set("utf-8")
#endif
#pragma warning(disable:4566)
#endif

namespace MySQLClientTest
{
    // 跳过测试
    constexpr bool skip_test = 1;

    const char* host = "localhost";
    int port = 33060;
    const char* user = "root";
    const char* pwd = "123456";
    // database = 'test_db'
    // table = 'test_table'
}
using namespace MySQLClientTest;

// DDL（Data Definition Language）数据库定义语言
TEST(MySQLClient, DDL) {
    if (skip_test) return;

    MySQLClient client(host, port, user, pwd);

    // 初始化
    client.RunSQL("drop database test_db");

    // 创建数据库（schema）create database ...
    ASSERT_TRUE(client.RunSQL("create database test_db"));

    ASSERT_TRUE(client.RunSQL("use test_db"));

    // 创建表（collection）create table ...
    std::string sql = R"(create table if not exists `test_table`(
            `id` int unsigned auto_increment,
            `title` varchar(100) not null,
            `author` varchar(40) not null,
            `submission_date` date,
            primary key ( `id` )
        )engine=innodb default charset=utf8;)";
    ASSERT_TRUE(client.RunSQL(sql));

    // 删除表 drop table ...
    ASSERT_TRUE(client.RunSQL("drop table test_table"));

    // 删除数据库 drop database ...
    ASSERT_TRUE(client.RunSQL("drop database test_db"));
}

// DML（Data Manipulation Language）数据操纵语言
TEST(MySQLClient, DML) {
    if (skip_test) return;

    MySQLClient client(host, port, user, pwd);

    // 初始化
    client.RunSQL("drop database test_db");
    ASSERT_TRUE(client.RunSQL("create database test_db"));
    ASSERT_TRUE(client.RunSQL("use test_db"));
    std::string sql = R"(create table if not exists `test_table`(
            `id` int unsigned auto_increment,
            `title` varchar(100) not null,
            `author` varchar(40) not null,
            `submission_date` date,
            primary key ( `id` )
        )engine=innodb default charset=utf8;)";
    ASSERT_TRUE(client.RunSQL(sql));

    // 增 insert into table ... values ...
    std::stringstream ss;
    ss << "insert into test_table (title,author,submission_date) values";
    for (int i = 0; i < 10; ++i) {
        auto title = "\'title_" + std::to_string(i) + '\'';
        auto author = i % 2 ? "\'author_A\'" : "\'author_B\'";
        auto submission_date = "CURDATE()";
        if (i > 0)
            ss << ",";
        ss << " (" << title << "," << author << "," << submission_date << ")";
    }
    ss << ";";
    sql = ss.str();
    ASSERT_TRUE(client.RunSQL(sql));

    // 删 delete from table where ...
    ASSERT_TRUE(client.RunSQL("delete from test_table where (id<=3);"));

    // 改 update table set ... where ...
    ASSERT_TRUE(client.RunSQL("update test_table set title='updated' where (id>=6);"));

    // 查 select ... from table where ...
    std::vector<std::vector<std::string>> ret;
    ASSERT_TRUE(client.RunSQL("select id,title,submission_date from test_table where (id>2 && id<8);", ret));
    ASSERT_EQ(ret.size(), 4);
    ASSERT_EQ(ret[0].size(), 3);

    ASSERT_EQ(ret[0][0], "4");
    ASSERT_EQ(ret[1][0], "5");
    ASSERT_EQ(ret[2][0], "6");
    ASSERT_EQ(ret[3][0], "7");

    ASSERT_EQ(ret[0][1], "title_3");
    ASSERT_EQ(ret[1][1], "title_4");
    ASSERT_EQ(ret[2][1], "updated");
    ASSERT_EQ(ret[3][1], "updated");

    // date 按字节保存，无法直接读取 string
    //ASSERT_EQ(ret[0][2], "2023-02-21");
    //ASSERT_EQ(ret[1][2], "2023-02-21");
    //ASSERT_EQ(ret[2][2], "2023-02-21");
    //ASSERT_EQ(ret[3][2], "2023-02-21");

    // 插入或替换（存在就删除） replace into ...
    // 插入或更新（存在就更新） insert into ... on duplicate key update ...
    // 插入或忽略（存在就忽略） insert ignore into ...
    // todo ...


    // 删除数据库
    ASSERT_TRUE(client.RunSQL("drop database test_db"));
}

// DCL（Data Control Language）数据库控制语言  授权，角色控制等
TEST(MySQLClient, DCL) {
    if (skip_test) return;

    MySQLClient client(host, port, user, pwd);

    /* 创建用户 create user ... identified by ...
     * 创建用户'user_foo'，只是创建用户并没有权限
     * 'localhost'表示只能在本地登录，'%'通配符表示可以远程连接
     * 密码是'123456'
     */
    ASSERT_TRUE(client.RunSQL(R"(create user 'user_foo'@'localhost' identified by '123456';)"));

    // 授权 grant ... on ... to
    // todo ...

    /* 删除用户 drop user ...
     */
    ASSERT_TRUE(client.RunSQL(R"(drop user 'user_foo'@'localhost';)"));
}

// TCL（Transaction Control Language）事务控制语言
TEST(MySQLClient, TCL) {
    if (skip_test) return;

    MySQLClient client(host, port, user, pwd);

    // 初始化
    client.RunSQL("drop database test_db");
    ASSERT_TRUE(client.RunSQL("create database test_db"));
    ASSERT_TRUE(client.RunSQL("use test_db"));
    std::string sql = R"(create table if not exists `test_table`(
            `id` int unsigned auto_increment,
            `title` varchar(100) not null,
            `author` varchar(40) not null,
            `submission_date` date,
            primary key ( `id` )
        )engine=innodb default charset=utf8;)";
    ASSERT_TRUE(client.RunSQL(sql));

    /* ACID：
     * 原子性（Atomicity）   事务
     * 一致性（Consistency） binlog记录表结构变更；三种格式
     * 隔离性（Isolation）   事务四种隔离级别：    脏读  不可重读  幻读
     *                       - read uncommitted      1       1       1      不建议
     *                       - read committed                1       1      建议
     *                       - repeatable read                       1      InnoDB 默认使用
     *                       - serializable                                 
     * 持久性（Durability）  事务日志 redo log、undo log
     */
    
    // 开启一个事务 begin、start transaction
    ASSERT_TRUE(client.RunSQL("begin;"));
    
    // 事物保存点，支持多个 savepoint 
    // ...

    // 默认开启自动提交，SET AUTOCOMMIT=0 禁止自动提交
    
    // 提交事务 commit
    ASSERT_TRUE(client.RunSQL("commit;"));
    // 回滚事务 rollback
    ASSERT_TRUE(client.RunSQL("rollback;"));
    
    // 隔离级别 set session/global transaction isolation level ...
    // 主要通过锁实现隔离，不同级别锁针对 update(脏读) 和 insert(幻读) 锁的粒度不同
    ASSERT_TRUE(client.RunSQL("set session transaction isolation level read uncommitted;")); // 脏读
    
    // 两种 select 读锁
    // lock in share mode   共享锁，可以多个会话读
    // for update           排他锁，和 update 操作一样级别的锁，禁止其他 update
    ASSERT_TRUE(client.RunSQL("select * from test_table where author='author_B' for update;"));

    // 删除数据库
    ASSERT_TRUE(client.RunSQL("drop database test_db"));
}
