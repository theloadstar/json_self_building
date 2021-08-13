#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

/*line18的format为参数*/

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

/*
//在没有写解析函数lept_parse之前
//该函数第一个测试通过，因为刚开始的lept_parse默认返回值为0，但第二个测试的actual应该为2，即true类型
//测试null
*/
static void test_parse_null() {
    lept_value v;
    v.type = LEPT_TRUE;//随意，但最好别设置为null，因为解析失败后还会将其设置为null
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "null"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}
//测试true
static void test_parse_true(){
    lept_value v;
    v.type = LEPT_FALSE;//随意
    EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v, "true"));
    EXPECT_EQ_INT(LEPT_TRUE,lept_get_type(&v));
}

// test false
static void test_parse_false(){
    lept_value v;
    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v,"false"));
    EXPECT_EQ_INT(LEPT_FALSE,lept_get_type((&v)));
}

//test parse expect value
static void test_parse_expect_value(){
    lept_value v;
    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE,lept_parse(&v,""));
    //若解析失败，返回null
    EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));

    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE,lept_parse(&v," "));
    //若解析失败，返回null
    EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));
}

//test parse invalid value
static void test_parse_invalid_value(){
    lept_value v;
    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE,lept_parse(&v,"nul"));
    EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));

    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE,lept_parse(&v,"invalid"));
    EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));
}

//test root_not_singual
static void test_parse_root_not_singular(){
    lept_value v;
    v.type = LEPT_TRUE;
    EXPECT_EQ_INT(LEPT_PARSE_ROOT_NOT_SINGULAR,lept_parse(&v,"null x"));
    EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));
}

static void test_parse() {
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main() {
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
