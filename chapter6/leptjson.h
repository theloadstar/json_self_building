#ifndef _LEPTJSON_H_
#define _LEPTJSON_H_

#include <stddef.h> /* size_t */

/*json 的七种基本类型值*/
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

typedef struct lept_value lept_value;
typedef struct lept_member lept_member;
/*定义存放json值的结构体，目前只需存放NULL/BOOL值，故只需先存放一个值即可*/
struct lept_value{
	union{
		struct {lept_member* m;size_t size;}o;/*object*/
		struct {lept_value* e; size_t size; }a;/*array*/
		struct {char* s; size_t len;}s;/*string*/
		double n;                      /*number*/
	}u;
	lept_type type;
};

struct lept_member{
	char* k;
	size_t klen;
	lept_value v;
};

/*this one is ok as well
typedef struct lept_value{
	union{
		struct {struct lept_value* e; size_t size; }a;
		struct {char* s; size_t len;}s;
		double n;
	}u;
	lept_type type;
} lept_value;
*/

/*解析函数返回值类型*/
enum {
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR,
	LEPT_PARSE_NUMBER_TOO_BIG,
    LEPT_PARSE_MISS_QUOTATION_MARK,
    LEPT_PARSE_INVALID_STRING_ESCAPE,
    LEPT_PARSE_INVALID_STRING_CHAR,
    LEPT_PARSE_INVALID_UNICODE_HEX,
    LEPT_PARSE_INVALID_UNICODE_SURROGATE,
    LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET
};
/*init value*/
#define lept_init(v) do{ (v)->type = LEPT_NULL; }while(0)
/*set null*/
#define lept_set_null(v) lept_free(v);

int lept_parse(lept_value* v, const char *json);

/*获取解析结果*/
lept_type lept_get_type(const lept_value* v);

/*获取json数字*/
double lept_get_number(const lept_value* v);

void lept_set_string(lept_value* v, const char* s, size_t len);/*设置字符串的值*/
void lept_free(lept_value* v);/*将string类型的json内部字符串空间free，并将类型设为null*/

int lept_get_boolean(const lept_value* v);/*得到v的bool值*/
void lept_set_boolean(lept_value* v, int b);/*set bool*/

double lept_get_number(const lept_value* v);
void lept_set_number(lept_value* v, double n);

/*使用const修饰，防止返回的指针改变所指向地址的值*/
const char* lept_get_string(const lept_value* v);
size_t lept_get_string_length(const lept_value* v);
void lept_set_string(lept_value* v, const char* s, size_t len);

/*array*/
size_t lept_get_array_size(const lept_value* v);
lept_value* lept_get_array_element(const lept_value* v, size_t index);

/*object*/
size_t lept_get_object_size(const lept_value* v);
const char* lept_get_object_key(const lept_value* v, size_t index);
size_t lept_get_object_key_length(const lept_value* v, size_t index);
lept_value* lept_get_object_value(const lept_value* v, size_t index);


#endif
