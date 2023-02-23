/****************************************************************************
** MIT License
**
** Author	: xiaofeng.zhu
** Support	: zxffffffff@outlook.com, 1337328542@qq.com
**
****************************************************************************/

#include "mysql/MySQLClient.h"
#include "gtest/gtest.h"

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
     *                       - Read Uncommitted      1       1       1      不建议
     *                       - Read Committed                1       1      建议
     *                       - Repeatable Read                       1      InnoDB 默认使用
     *                       - Serializable                                 
     * 持久性（Durability）  事务日志 redo log、undo log
     */

    std::thread t1([] {
        MySQLClient client(host, port, user, pwd);
        ASSERT_TRUE(client.RunSQL("use test_db"));

        // 开启一个事务 BEGIN、START TRANSACTION
        ASSERT_TRUE(client.RunSQL("begin;"));

        // 事物保存点，支持多个 SAVEPOINT 
        // ...

        // 执行业务
        std::stringstream ss;
        ss << "insert into test_table (title,author,submission_date) values";
        for (int i = 0; i < 1000; ++i) {
            auto title = "\'title_" + std::to_string(i) + '\'';
            auto author = i % 2 ? "\'author_A\'" : "\'author_B\'";
            auto submission_date = "CURDATE()";
            if (i > 0)
                ss << ",";
            ss << " (" << title << "," << author << "," << submission_date << ")";
        }
        ss << ";";
        std::string sql = ss.str();
        ASSERT_TRUE(client.RunSQL(sql));

        // 提交事务 COMMIT 回滚事务 ROLLBACK
        ASSERT_TRUE(client.RunSQL("commit;"));

        // 隔离级别 SET TRANSACTION
    });

    std::thread t2([] {
        MySQLClient client(host, port, user, pwd);
        ASSERT_TRUE(client.RunSQL("use test_db"));

        // read
    });

    t1.join();
    t2.join();

    // 删除数据库
    ASSERT_TRUE(client.RunSQL("drop database test_db"));
}

TEST(MySQLClient, DeadTest) {
    if (skip_test) return;

    MySQLClient client(host, port, user, pwd);

    // 初始化
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

    // todo ...


    // 删除数据库
    ASSERT_TRUE(client.RunSQL("drop database test_db"));
}
