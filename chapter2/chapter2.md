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

将test文件中的所有测试**ERROR**代码都进行了如上重构，**重新cmake**后测试通过。

![chapter2_ERROR_refactoring_cmake](../graph/chapter2_ERROR_refactoring_cmake.png)

<font color = "red">注意！！！</font>修改文件的时候在chapter2里修改，我说呢，每次cmake后结果都一样。。。

![chapter2_ERROR_refactoring_cmake2](../graph/chapter2_ERROR_refactoring_cmake2.png)

# 数字

为简单起见，leptjson使用双精度double存储JSON数字。先来看一下JSON数字的语法：

```json
number = [ "-" ] int [ frac ] [ exp ]
int = "0" / digit1-9 *digit
frac = "." 1*digit
exp = ("e" / "E") ["-" / "+"] 1*digit
```

JSON数字由符号、整数、小数以及指数组成。其中只有整数部分为必需。注意符号只可为`-`，不可为`+`，即以`+`开头的数字为格式错误。整数部分若以0开始，则只能为单个数字0；否则，为由`1-9`组成的若干数字。

小数部分比较直观，就是小数点后是一或多个数字（0-9)

JSON 可使用科学记数法，指数部分由大写 E 或小写 e 开始，然后可有正负号，之后是一或多个数字（0-9）

![chapter2_json_number](../graph/chapter2_json_number.png)

---

为存储数字，在`lept_value`中添加成员:`double n`。同时添加函数`lept_get_number`来获取json的number值。注意，只有lept_type==LEPT_NUMBER时才会存储number，故使用assert断言。

```c
lept_type lept_get_number(const lept_value* v){
	assert(v!=NULL&&v->type==LEPT_NUMBER);
	return v->n;
}
```

