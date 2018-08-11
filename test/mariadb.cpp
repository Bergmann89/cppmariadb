#include <memory>
#include <type_traits>
#include <gtest/gtest.h>
#include <mariadb.h>

#include "mock.h"

using namespace ::testing;
using namespace ::mariadb;

/**********************************************************************************************************/
TEST(MariaDbTests, MariaDB_connect)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_init(nullptr))
        .WillOnce(Return(reinterpret_cast<MYSQL*>(0x123)));
    EXPECT_CALL(mock, mysql_real_connect(reinterpret_cast<MYSQL*>(0x123), StrEq("testhost"), StrEq("testuser"), StrEq("password"), StrEq("database"), 3306, nullptr, 0))
        .WillOnce(Invoke([](MYSQL *mysql, const char*, const char*, const char*, const char*, unsigned int, const char*, unsigned long){
            return mysql;
        }));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x123)))
        .Times(1);

    auto con = database::connect("testhost", 3306, "testuser", "password", "database", client_flags::empty());

    EXPECT_EQ(reinterpret_cast<MYSQL*>(0x123), con.handle());
}

TEST(MariaDbTests, MariaDB_errorCode)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(reinterpret_cast<MYSQL*>(0x123)))
        .WillOnce(Return(1000));
    auto ret = database::error_code(reinterpret_cast<MYSQL*>(0x123));
    EXPECT_EQ(error_code::ErrorFirst, ret);
}

TEST(MariaDbTests, MariaDB_errorMessage)
{
    {
    StrictMock<MariaDbMock> mock;
    std::string msg("test");
    EXPECT_CALL(mock, mysql_error(reinterpret_cast<MYSQL*>(0x123)))
        .WillOnce(Return(msg.data()));
    auto ret = database::error_msg(reinterpret_cast<MYSQL*>(0x123));
    EXPECT_EQ(msg, ret);
    }

    {
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_error(reinterpret_cast<MYSQL*>(0x123)))
        .WillOnce(Return(nullptr));
    auto ret = database::error_msg(reinterpret_cast<MYSQL*>(0x123));
    EXPECT_EQ(std::string(), ret);
    }
}

/**********************************************************************************************************/
TEST(MariaDbTests, Connection_fieldcount)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_field_count(reinterpret_cast<MYSQL*>(0x5514)))
        .WillOnce(Return(5));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x5514)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x5514));
    auto ret = con.fieldcount();
    EXPECT_EQ(5, ret);
}

TEST(MariaDbTests, Connection_escape)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x5514), _, StrEq("'teststring'"), 12))
        .WillOnce(DoAll(
            WithArgs<1>(Invoke([](char* str){
                memcpy(str, "\\'teststring\\'", 14);
            })),
            Return(14)));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x5514)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x5514));
    auto ret = con.escape("'teststring'");
    EXPECT_EQ(std::string("\\'teststring\\'"), ret);
}

/**********************************************************************************************************/
TEST(MariaDbTests, Connection_execute_queryFailed)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(1));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    EXPECT_THROW(con.execute("SELECT * FROM blubb"), ::mariadb::exception);
}

TEST(MariaDbTests, Connection_execute_storeResultFailed_fieldCountGreaterZero)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_store_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_field_count(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(5));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    EXPECT_THROW(con.execute("SELECT * FROM blubb"), ::mariadb::exception);
}

TEST(MariaDbTests, Connection_execute_storeResultFailed_fieldCountIsZero)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_store_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_field_count(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    con.execute("SELECT * FROM blubb");
    ASSERT_FALSE(static_cast<bool>(con.result()));
}

TEST(MariaDbTests, Connection_execute_success)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_store_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(reinterpret_cast<MYSQL_RES*>(0x8888)));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x8888)))
        .Times(1);
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    con.execute("SELECT * FROM blubb");
    ASSERT_TRUE(static_cast<bool>(con.result()));
    EXPECT_EQ  (reinterpret_cast<MYSQL_RES*>(0x8888), con.result()->handle());
}

/**********************************************************************************************************/
TEST(MariaDbTests, Connection_executeStored_queryFailed)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(1));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    EXPECT_THROW(con.execute_stored("SELECT * FROM blubb"), ::mariadb::exception);
}

TEST(MariaDbTests, Connection_executeStored_storeResultFailed_fieldCountGreaterZero)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_store_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_field_count(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(5));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    EXPECT_THROW(con.execute_stored("SELECT * FROM blubb"), ::mariadb::exception);
}

TEST(MariaDbTests, Connection_executeStored_storeResultFailed_fieldCountIsZero)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_store_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_field_count(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    con.execute_stored("SELECT * FROM blubb");
    ASSERT_FALSE(static_cast<bool>(con.result()));
}

TEST(MariaDbTests, Connection_executeStored_success)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_store_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(reinterpret_cast<MYSQL_RES*>(0x8888)));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x8888)))
        .Times(1);
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    auto ret = con.execute_stored("SELECT * FROM blubb");
    EXPECT_EQ  (ret, con.result());
    ASSERT_TRUE(static_cast<bool>(ret));
    EXPECT_EQ  (reinterpret_cast<MYSQL_RES*>(0x8888), ret->handle());
}

/**********************************************************************************************************/
TEST(MariaDbTests, Connection_executeUsed_queryFailed)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(1));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    EXPECT_THROW(con.execute_used("SELECT * FROM blubb"), ::mariadb::exception);
}

TEST(MariaDbTests, Connection_executeused_useResultFailed_fieldCountGreaterZero)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_use_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_field_count(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(5));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    EXPECT_THROW(con.execute_used("SELECT * FROM blubb"), ::mariadb::exception);
}

TEST(MariaDbTests, Connection_executeUsed_useResultFailed_fieldCountIsZero)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_use_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_field_count(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    con.execute_used("SELECT * FROM blubb");
    ASSERT_FALSE(static_cast<bool>(con.result()));
}

TEST(MariaDbTests, Connection_executeUsed_success)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_errno(_)).Times(AnyNumber());
    EXPECT_CALL(mock, mysql_error(_)).Times(AnyNumber());

    InSequence seq;
    EXPECT_CALL(mock, mysql_real_query(reinterpret_cast<MYSQL*>(0x6818), StrEq("SELECT * FROM blubb"), 19))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_use_result(reinterpret_cast<MYSQL*>(0x6818)))
        .WillOnce(Return(reinterpret_cast<MYSQL_RES*>(0x8888)));
    EXPECT_CALL(mock, mysql_fetch_row(reinterpret_cast<MYSQL_RES*>(0x8888)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x8888)))
        .Times(1);
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x6818)))
        .Times(1);

    connection con(reinterpret_cast<MYSQL*>(0x6818));
    auto ret = con.execute_used("SELECT * FROM blubb");
    EXPECT_EQ  (ret, con.result());
    ASSERT_TRUE(static_cast<bool>(ret));
    EXPECT_EQ  (reinterpret_cast<MYSQL_RES*>(0x8888), ret->handle());
}

/**********************************************************************************************************/
TEST(MariaDbTests, Statement_set_validIndex)
{
    StrictMock<MariaDbMock> mock;
    statement s("SELECT * FROM ?table?");
    EXPECT_NO_THROW(s.set(0, "test"));
}

TEST(MariaDbTests, Statement_set_invalidIndex)
{
    StrictMock<MariaDbMock> mock;
    statement s("SELECT * FROM ?table?");
    EXPECT_THROW(s.set(4, "test"), ::mariadb::exception);
}

TEST(MariaDbTests, Statement_set_validName)
{
    StrictMock<MariaDbMock> mock;
    statement s("SELECT * FROM ?table?");
    EXPECT_NO_THROW(s.set("table", "test"));
}

TEST(MariaDbTests, Statement_set_invalidName)
{
    StrictMock<MariaDbMock> mock;
    statement s("SELECT * FROM ?table?");
    EXPECT_THROW(s.set("foo", "test"), ::mariadb::exception);
}

TEST(MariaDbTests, Statement_query)
{
    StrictMock<MariaDbMock> mock;
    EXPECT_CALL(mock, mysql_real_escape_string(reinterpret_cast<MYSQL*>(0x123), _, StrEq("test"), 4))
        .WillOnce(DoAll(
            WithArgs<1>(Invoke([](char* str){
                memcpy(str, "test", 4);
            })),
            Return(4)));
    EXPECT_CALL(mock, mysql_close(reinterpret_cast<MYSQL*>(0x123)))
        .Times(1);

    connection c(reinterpret_cast<MYSQL*>(0x123));

    statement s("SELECT * FROM ?table?");
    s.set("table", "test");
    auto ret = s.query(c);
    EXPECT_EQ(std::string("SELECT * FROM 'test'"), ret);
}

/**********************************************************************************************************/
TEST(MariaDbTests, Result_rowindex_next_current)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_fetch_row(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(reinterpret_cast<MYSQL_ROW>(0x15160)))
        .WillOnce(Return(reinterpret_cast<MYSQL_ROW>(0x15161)));
    EXPECT_CALL(mock, mysql_data_seek(reinterpret_cast<MYSQL_RES*>(0x51651), 0))
        .Times(1);
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    EXPECT_EQ(-1, result.rowindex());
    auto row = result.next();
    ASSERT_TRUE(static_cast<bool>(row));
    EXPECT_EQ  (reinterpret_cast<MYSQL_ROW>(0x15160), row->handle());
    EXPECT_EQ  (0, result.rowindex());

    row = result.next();
    ASSERT_TRUE(static_cast<bool>(row));
    EXPECT_EQ  (reinterpret_cast<MYSQL_ROW>(0x15161), row->handle());
    EXPECT_EQ  (1, result.rowindex());

    auto current = result.current();
    EXPECT_EQ(row, current);

    result.rowindex(0);
    EXPECT_EQ(0, result.rowindex());
}

TEST(MariaDbTests, Result_columncount)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(10));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    auto ret = result.columncount();
    EXPECT_EQ(10, ret);
}

TEST(MariaDbTests, Result_columns)
{
    static const std::string name0      ("index");
    static const std::string name1      ("username");
    static const std::string name2      ("password");
    static const std::string table      ("user");
    static const std::string database   ("database");

    MYSQL_FIELD fields[3];
    fields[0].name              = const_cast<char*>(name0.c_str());
    fields[0].org_name          = const_cast<char*>(name0.c_str());
    fields[0].table             = const_cast<char*>(table.c_str());
    fields[0].org_table         = const_cast<char*>(table.c_str());
    fields[0].db                = const_cast<char*>(database.c_str());
    fields[0].catalog           = nullptr;
    fields[0].def               = nullptr;
    fields[0].length            = 32;
    fields[0].max_length        = 2;
    fields[0].name_length       = static_cast<unsigned int>(name0.size());
    fields[0].org_name_length   = static_cast<unsigned int>(name0.size());
    fields[0].table_length      = static_cast<unsigned int>(table.size());
    fields[0].org_table_length  = static_cast<unsigned int>(table.size());
    fields[0].db_length         = static_cast<unsigned int>(database.size());
    fields[0].catalog_length    = 0;
    fields[0].def_length        = 0;
    fields[0].flags             = 0;
    fields[0].decimals          = 0;
    fields[0].charsetnr         = 0;
    fields[0].type              = MYSQL_TYPE_LONG;
    fields[0].extension         = nullptr;

    fields[1].name              = const_cast<char*>(name1.c_str());
    fields[1].org_name          = const_cast<char*>(name1.c_str());
    fields[1].table             = const_cast<char*>(table.c_str());
    fields[1].org_table         = const_cast<char*>(table.c_str());
    fields[1].db                = const_cast<char*>(database.c_str());
    fields[1].catalog           = nullptr;
    fields[1].def               = nullptr;
    fields[1].length            = 128;
    fields[1].max_length        = 48;
    fields[1].name_length       = static_cast<unsigned int>(name1.size());
    fields[1].org_name_length   = static_cast<unsigned int>(name1.size());
    fields[1].table_length      = static_cast<unsigned int>(table.size());
    fields[1].org_table_length  = static_cast<unsigned int>(table.size());
    fields[1].db_length         = static_cast<unsigned int>(database.size());
    fields[1].catalog_length    = 0;
    fields[1].def_length        = 0;
    fields[1].flags             = 0;
    fields[1].decimals          = 0;
    fields[1].charsetnr         = 0;
    fields[1].type              = MYSQL_TYPE_STRING;
    fields[1].extension         = nullptr;

    fields[2].name              = const_cast<char*>(name2.c_str());
    fields[2].org_name          = const_cast<char*>(name2.c_str());
    fields[2].table             = const_cast<char*>(table.c_str());
    fields[2].org_table         = const_cast<char*>(table.c_str());
    fields[2].db                = const_cast<char*>(database.c_str());
    fields[2].catalog           = nullptr;
    fields[2].def               = nullptr;
    fields[2].length            = 128;
    fields[2].max_length        = 42;
    fields[2].name_length       = static_cast<unsigned int>(name2.size());
    fields[2].org_name_length   = static_cast<unsigned int>(name2.size());
    fields[2].table_length      = static_cast<unsigned int>(table.size());
    fields[2].org_table_length  = static_cast<unsigned int>(table.size());
    fields[2].db_length         = static_cast<unsigned int>(database.size());
    fields[2].catalog_length    = 0;
    fields[2].def_length        = 0;
    fields[2].flags             = 0;
    fields[2].decimals          = 0;
    fields[2].charsetnr         = 0;
    fields[2].type              = MYSQL_TYPE_STRING;
    fields[2].extension         = nullptr;

    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_fetch_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(fields));
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(3));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    auto ret = result.columns();

    ASSERT_EQ(3,                    ret.size());

    EXPECT_EQ(name0,                ret.at(0).name);
    EXPECT_EQ(name0,                ret.at(0).original_name);
    EXPECT_EQ(table,                ret.at(0).table);
    EXPECT_EQ(table,                ret.at(0).original_table);
    EXPECT_EQ(database,             ret.at(0).database);
    EXPECT_EQ(32,                   ret.at(0).length);
    EXPECT_EQ(2,                    ret.at(0).max_length);
    EXPECT_EQ(column_flags::empty(), ret.at(0).flags);
    EXPECT_EQ(0,                    ret.at(0).decimals);
    EXPECT_EQ(0,                    ret.at(0).charset_number);
    EXPECT_EQ(column_type::Long,     ret.at(0).type);

    EXPECT_EQ(name1,                ret.at(1).name);
    EXPECT_EQ(name1,                ret.at(1).original_name);
    EXPECT_EQ(table,                ret.at(1).table);
    EXPECT_EQ(table,                ret.at(1).original_table);
    EXPECT_EQ(database,             ret.at(1).database);
    EXPECT_EQ(128,                  ret.at(1).length);
    EXPECT_EQ(48,                   ret.at(1).max_length);
    EXPECT_EQ(column_flags::empty(), ret.at(1).flags);
    EXPECT_EQ(0,                    ret.at(1).decimals);
    EXPECT_EQ(0,                    ret.at(1).charset_number);
    EXPECT_EQ(column_type::String,   ret.at(1).type);

    EXPECT_EQ(name2,                ret.at(2).name);
    EXPECT_EQ(name2,                ret.at(2).original_name);
    EXPECT_EQ(table,                ret.at(2).table);
    EXPECT_EQ(table,                ret.at(2).original_table);
    EXPECT_EQ(database,             ret.at(2).database);
    EXPECT_EQ(128,                  ret.at(2).length);
    EXPECT_EQ(42,                   ret.at(2).max_length);
    EXPECT_EQ(column_flags::empty(), ret.at(2).flags);
    EXPECT_EQ(0,                    ret.at(2).decimals);
    EXPECT_EQ(0,                    ret.at(2).charset_number);
    EXPECT_EQ(column_type::String,   ret.at(2).type);
}

/**********************************************************************************************************/
TEST(MariaDbTests, StoredResult_rowoffset)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_row_tell(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(reinterpret_cast<MYSQL_ROW_OFFSET>(0)));
    EXPECT_CALL(mock, mysql_row_seek(reinterpret_cast<MYSQL_RES*>(0x51651), reinterpret_cast<MYSQL_ROW_OFFSET>(4)))
        .WillOnce(Return(reinterpret_cast<MYSQL_ROW_OFFSET>(0)));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    auto offset = result.rowoffset();
    EXPECT_EQ(reinterpret_cast<MYSQL_ROW_OFFSET>(0), offset);

    result.rowoffset(reinterpret_cast<MYSQL_ROW_OFFSET>(4));
}

TEST(MariaDbTests, StoredResult_rowcount)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_rows(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(100));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    auto ret = result.rowcount();
    EXPECT_EQ(100, ret);
}

/**********************************************************************************************************/
TEST(MariaDbTests, UsedResult_dtor)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_fetch_row(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(reinterpret_cast<MYSQL_ROW>(0x0001)))
        .WillOnce(Return(reinterpret_cast<MYSQL_ROW>(0x0002)))
        .WillOnce(Return(reinterpret_cast<MYSQL_ROW>(0x0003)))
        .WillOnce(Return(reinterpret_cast<MYSQL_ROW>(0x0004)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_used result(reinterpret_cast<MYSQL_RES*>(0x51651));
}

/**********************************************************************************************************/
static const char* RowData[3] =
{
    "2",
    "bergmann89",
    "secret"
};

static unsigned long RowLengths[3] =
{
    1,
    10,
    6
};

TEST(MariaDbTests, Row_columns)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_fetch_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(nullptr));
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(0));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    row.columns();
}

TEST(MariaDbTests, Row_size)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(3));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    auto ret = row.size();
    EXPECT_EQ(3u, ret);
}

TEST(MariaDbTests, Row_at_invalidIndex)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(3));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    EXPECT_THROW(row.at(3), ::mariadb::exception);
}

TEST(MariaDbTests, Row_at_validIndex)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(3));
    EXPECT_CALL(mock, mysql_fetch_lengths(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(&RowLengths[0]));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    EXPECT_NO_THROW(row.at(1));
}

static const std::string RowDataName0("index");
static const std::string RowDataName1("username");
static const std::string RowDataName2("password");

TEST(MariaDbTests, Row_at_invalidName)
{
    MYSQL_FIELD fields[3];
    memset(&fields[0], 0, sizeof(fields));
    fields[0].name        = const_cast<char*>(RowDataName0.c_str());
    fields[0].name_length = static_cast<unsigned int>(RowDataName0.size());
    fields[1].name        = const_cast<char*>(RowDataName1.c_str());
    fields[1].name_length = static_cast<unsigned int>(RowDataName1.size());
    fields[2].name        = const_cast<char*>(RowDataName2.c_str());
    fields[2].name_length = static_cast<unsigned int>(RowDataName2.size());

    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_fetch_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(fields));
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(3));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    EXPECT_THROW(row.at("blubb"), ::mariadb::exception);
}

TEST(MariaDbTests, Row_at_validName)
{
    MYSQL_FIELD fields[3];
    memset(&fields[0], 0, sizeof(fields));
    fields[0].name        = const_cast<char*>(RowDataName0.c_str());
    fields[0].name_length = static_cast<unsigned int>(RowDataName0.size());
    fields[1].name        = const_cast<char*>(RowDataName1.c_str());
    fields[1].name_length = static_cast<unsigned int>(RowDataName1.size());
    fields[2].name        = const_cast<char*>(RowDataName2.c_str());
    fields[2].name_length = static_cast<unsigned int>(RowDataName2.size());

    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_fetch_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(fields));
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(3));
    EXPECT_CALL(mock, mysql_fetch_lengths(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillOnce(Return(&RowLengths[0]));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    EXPECT_NO_THROW(row.at("username"));
}

TEST(MariaDbTests, Row_iterator)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(3));
    EXPECT_CALL(mock, mysql_fetch_lengths(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(&RowLengths[0]));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    auto it = row.begin();
    ASSERT_TRUE(it != row.end());
    ++it;
    ASSERT_TRUE(it != row.end());
    ++it;
    ASSERT_TRUE(it != row.end());
    ++it;
    ASSERT_TRUE(it == row.end());
}

TEST(MariaDbTests, Row_const_iterator)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(3));
    EXPECT_CALL(mock, mysql_fetch_lengths(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(&RowLengths[0]));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    auto it = row.cbegin();
    ASSERT_TRUE(it != row.cend());
    ++it;
    ASSERT_TRUE(it != row.cend());
    ++it;
    ASSERT_TRUE(it != row.cend());
    ++it;
    ASSERT_TRUE(it == row.cend());
}

TEST(MariaDbTests, Row_reverse_iterator)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(3));
    EXPECT_CALL(mock, mysql_fetch_lengths(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(&RowLengths[0]));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    auto it = row.rbegin();
    ASSERT_TRUE(it != row.rend());
    ++it;
    ASSERT_TRUE(it != row.rend());
    ++it;
    ASSERT_TRUE(it != row.rend());
    ++it;
    ASSERT_TRUE(it == row.rend());
}

TEST(MariaDbTests, Row_const_reverse_iterator)
{
    StrictMock<MariaDbMock> mock;
    InSequence seq;
    EXPECT_CALL(mock, mysql_num_fields(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(3));
    EXPECT_CALL(mock, mysql_fetch_lengths(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .WillRepeatedly(Return(&RowLengths[0]));
    EXPECT_CALL(mock, mysql_free_result(reinterpret_cast<MYSQL_RES*>(0x51651)))
        .Times(1);

    result_stored result(reinterpret_cast<MYSQL_RES*>(0x51651));
    row row(result, const_cast<MYSQL_ROW>(&RowData[0]));
    auto it = row.crbegin();
    ASSERT_TRUE(it != row.crend());
    ++it;
    ASSERT_TRUE(it != row.crend());
    ++it;
    ASSERT_TRUE(it != row.crend());
    ++it;
    ASSERT_TRUE(it == row.crend());
}

/**********************************************************************************************************/
TEST(MariaDbTests, Field_index)
{
    mariadb::result_stored   result (reinterpret_cast<MYSQL_RES*>(0x51651));
    mariadb::row             row    (result, const_cast<MYSQL_ROW>(&RowData[0]));
    mariadb::field           field  (row, 1, RowData[1], RowLengths[1]);
    EXPECT_EQ(1, field.index());
}

TEST(MariaDbTests, Field_isNull)
{
    mariadb::result_stored  result  (reinterpret_cast<MYSQL_RES*>(0x51651));
    mariadb::row            row     (result, const_cast<MYSQL_ROW>(&RowData[0]));
    mariadb::field          field0  (row, 1, nullptr, 0);
    mariadb::field          field1  (row, 1, "",      0);
    mariadb::field          field2  (row, 1, "asd",   3);

    EXPECT_TRUE (field0.is_null());
    EXPECT_FALSE(field1.is_null());
    EXPECT_FALSE(field2.is_null());
}

TEST(MariaDbTests, Field_isEmpty)
{
    mariadb::result_stored  result  (reinterpret_cast<MYSQL_RES*>(0x51651));
    mariadb::row            row     (result, const_cast<MYSQL_ROW>(&RowData[0]));
    mariadb::field          field0  (row, 1, nullptr, 0);
    mariadb::field          field1  (row, 1, "",      0);
    mariadb::field          field2  (row, 1, "asd",   3);

    EXPECT_TRUE (field0.is_empty());
    EXPECT_TRUE (field1.is_empty());
    EXPECT_FALSE(field2.is_empty());
}

TEST(MariaDbTests, Field_data)
{
    static const char* data = "test";

    mariadb::result_stored  result  (reinterpret_cast<MYSQL_RES*>(0x51651));
    mariadb::row            row     (result, const_cast<MYSQL_ROW>(&RowData[0]));
    mariadb::field          field   (row, 1, data, 4);

    EXPECT_EQ(data, field.data());
}

TEST(MariaDbTests, Field_size)
{
    static const char* data = "test";

    mariadb::result_stored  result  (reinterpret_cast<MYSQL_RES*>(0x51651));
    mariadb::row            row     (result, const_cast<MYSQL_ROW>(&RowData[0]));
    mariadb::field          field   (row, 1, data, 4);

    EXPECT_EQ(4, field.size());
}

TEST(MariaDbTests, Field_implicit_bool_cast)
{
    mariadb::result_stored  result  (reinterpret_cast<MYSQL_RES*>(0x51651));
    mariadb::row            row     (result, const_cast<MYSQL_ROW>(&RowData[0]));
    mariadb::field          field0  (row, 1, nullptr, 0);
    mariadb::field          field1  (row, 1, "",      0);
    mariadb::field          field2  (row, 1, "asd",   3);

    EXPECT_FALSE(static_cast<bool>(field0));
    EXPECT_FALSE(static_cast<bool>(field1));
    EXPECT_TRUE (static_cast<bool>(field2));
}

TEST(MariaDbTests, Field_get)
{
    mariadb::result_stored  result  (reinterpret_cast<MYSQL_RES*>(0x51651));
    mariadb::row            row     (result, const_cast<MYSQL_ROW>(&RowData[0]));
    mariadb::field          field0  (row, 1, "123", 3);
    mariadb::field          field1  (row, 1, "asd",   3);

    EXPECT_EQ(123,                field0.get<int>());
    EXPECT_EQ(std::string("asd"), field1.get<std::string>());
}