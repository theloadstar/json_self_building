#ifndef _LEPTJSON_H_
#define _LEPTJSON_H_

//json 的七种基本类型值
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

//定义存放json值的结构体，目前只需存放NULL/BOOL值，故只需先存放一个值即可
typedef struct{
	lept_type type;
} lept_value;

/*
JSON 格式： ws value ws
ws表示white space，包括空格、制表、换行、回车，故如下返回值分别代表：
* 返回值解析无错误
* 返回值解析缺少内容
* 返回解析出现了除7种类型以及ws以外的无效类型
* 返回值在解析完以上三个部分后还有其他内容
*/

//解析函数返回值类型
enum {
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR
};

//解析函数
int lept_prase(lept_value* v, const char *json);

//获取解析结果
lept_type lept_get_type(const lept_value* v);


#endif