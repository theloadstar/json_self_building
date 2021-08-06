# leptjson.h头文件

## JSON结构

JSON为树状结构，包含六种数据类型

* null: 表示为 null
* boolean: 表示为 true 或 false
* number: 一般的浮点数表示方式，在下一单元详细说明
* string: 表示为 "..."
* array: 表示为 [ ... ]
* object: 表示为 { ... }

如下例子所示：

```C++
{
    "title": "Design Patterns",
    "subtitle": "Elements of Reusable Object-Oriented Software",
    "author": [
        "Erich Gamma",
        "Richard Helm",
        "Ralph Johnson",
        "John Vlissides"
    ],
    "year": 2009,
    "weight": 1.8,
    "hardcover": true,
    "publisher": {
        "Company": "Pearson Education",
        "Country": "India"
    },
    "website": null
}
```

主要需求：

1. 把 JSON 文本解析为一个树状数据结构（parse）。
2. 提供接口访问该数据结构（access）。
3. 把数据结构转换成 JSON 文本（stringify）。

![json_requements](../graph/json_requements.png)

chapter1主要进行bool和null的解析

### 语法子集

chapter1的json语法使用 [RFC7159](https://tools.ietf.org/html/rfc7159) 中的 [ABNF](https://tools.ietf.org/html/rfc5234) 表示:

```C
JSON-text = ws value ws
ws = *(%x20 / %x09 / %x0A / %x0D)
value = null / false / true 
null  = "null"
false = "false"
true  = "true"
```

其中，`%x`表示16进制，`/`表示多选一，`*`表示零个或多个，`()`表示整体。这里的数字对应相应的ascii码，表示空格、制表符、换行符、回车符。

## include 防范

一个头文件在同一个文件下只能被包含一次，但由于头文件也能包含其他头文件，就可能出现某个文件包含了一个头文件多次的情况，使用include防范避免此问题。

```C++
#ifndef _LEPTJSON_H_
#define _LEPTJSON_H_
/*头文件内容*/
#endif
```

leptjson.h文件的开头结尾如上所示：`ifndef`为预编译指令，`_LEPTJSON_H_`可以为任意的的名字，这里使用_开头是考虑到该名称在其他地方不太可能被定义，`_H_`结尾则为一种习惯，若有多个头文件，则可以使用`项目名称_目录_文件名称_H_`的形式。

编译器第一次遇到该文件时，此时`_LEPTJSON_H_`还未被定义，执行内容，定义`_LEPTJSON_H_`。之后若再include该头文件时，因为此时`_LEPTJSON_H_`已被定义，不满足line1的内容，故line1后的内容不再执行。即<font color = "red">它忽略第一次include以外的所有内容，而不是防止编译器将文件include两次</font>。

[参考](https://blog.csdn.net/Bubbler_726/article/details/104618748)

## C 枚举类型

C有一种称为`enum`的枚举型数据类型，其写法与结构体类似，不同的是内部直接写枚举值，如：

```C
enum typeName{ valueName1, valueName2, valueName3, ...... };
```

可以看成一个集合，内部的枚举值均为一些命名的<font color = "red">整型变量（包括char类型）</font>，从0开始，递增+1，也可以给第n个整型变量赋初值n，之后的枚举值在n的基础上递增+1，但不改变第n个前面的枚举值。即显式说明了枚举常量的值时，未指定的枚举名的值将依着最后一个指定值向后依次递增（注意是最后一个指定值）。`typeName`为集合的名字，类型的定义以`;`结束。

使用`typeNmae`定义枚举变量，<font color = "red">枚举变量只能被赋予枚举值</font>，否则需要进行类型转换。

[参考](https://www.cnblogs.com/lanhaicode/p/10620028.html)

---

```C
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;
```

由前文知，JSON主要有六种变量类型，若false/true算为两种，则有七种类型，使用枚举得到以上的集合类型`lept_type`。

目前，只需实现NULL/TRUE/FALSE类型的解析，我们定义一个用来存放JSON值的结构体`lept_value`,之后会逐步完善该结构体。

```C
typedef struct{
	lept_type type;
} lept_value;
```

之后声明两个函数，一个为解析函数，一个为获取某个json值类型的函数，即获取解析结果的函数

解析函数：

```C
int lept_prase(lept_value* v, const char *json);
```

解析函数的参数为一个负责存储解析结果的v和解析前的字符串json；由于我们不希望改变json的内容，故使用const实现；该函数的返回值为错误类型，如下所示；

获取解析结果：

```C
lept_type lept_get_type(const lept_value* v);
```

解析函数的返回值类型如下：

```C
enum {
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR
};
```

结合json语法，各个类型分别表示：

* 返回值解析无错误
* 返回值解析缺少内容
* 返回解析出现了除7种类型以及ws以外的无效类型
* 返回值在解析完`ws value ws`后还有其他内容

# test.c 文件

