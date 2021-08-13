# 重构

之前大三写OO的时候也经常重构代码，就是在**不改变代码外在行为的情况下改善程序的内部结构。**在test.c	程序中，`test_parse_true`,`test_parse_null`,`test_parse_false`这几个程序包含了很多重复代码，这违反了DRY(do not repeat yourself)原则。故需要对这三个函数进行重构。

此外，在函数`test_parse_expect_value`,`test_parse_invalid_value`中，每测试一个JSON值，都会有两行代码的重复，为此，引入如下的宏定义：

```C
#define TEST_ERROR(error, json)\
    do {\
        lept_value v;\
        v.type = LEPT_TRUE;\
        EXPECT_EQ_INT(error, lept_parse(&v, json));\
        EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));\
    }while(0)
```

引入该宏定义后，以`test_parse_invalid_value`为例，其函数结构如下所示：

```C
static void test_parse_invalid_value(){
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE,"nul");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE,"invalid");
}
```

这样子的重构的使得代码结构更容易维护，但性能方面出现了些许损失。