#include "leptjson.h"
#include <assert.h> /*assert()*/
#include <stdlib.h> /*NULL  strtod*/
#include <stdio.h>

/*
//个人没明白这个宏的意义，因为在lept_parse_value里都是经过switch分支进入到对应的解析里的，那么第一个字符
//一定是对应相等的，如此一来，此处的assert有何作用？
*/
#define EXPECT(c,ch) do{ assert(*c->json==(ch));c->json++;}while(0)

/*为减少函数之间传递多个参数，定义json字符串结构体*/
typedef struct{
	const char* json;
}lept_context;

/*
*由于json语法较为简单，这里不用写类似于编译里的词法分析器token，直接按照字符进行判断即可，判断的规则如下：
n ➔ null
t ➔ true
f ➔ false
" ➔ string
0-9/- ➔ number
[ ➔ array
{ ➔ object
*/

/*
//使用static关键字使得函数只在声明的文件中使用，其他文件不可见，可定义同名函数
//ws = *(%x20 / %x09 / %x0A / %x0D)
//跳过ws，注意ws的解析不会出现错误，故无返回值
//可以直接操作c->json,但写成p使得代码可读性更强
*/
static void lept_parse_whitespace(lept_context* c){
	const char* p = c->json;
	while(*p==' '||*p=='\t'||*p=='\n'|*p=='\r')
		p++;
	c->json = p;
}

static int lept_parse_null(lept_context* c, lept_value* v){
	EXPECT(c,'n');
	if(c->json[0]!='u'||c->json[1]!='l'||c->json[2]!='l'){
		return LEPT_PARSE_INVALID_VALUE;
	}
	c->json+=3;
	v->type = LEPT_NULL;
	return LEPT_PARSE_OK;
}

static int lept_parse_true(lept_context* c, lept_value* v){
	EXPECT(c,'t');
	if(c->json[0]!='r'||c->json[1]!='u'||c->json[2]!='e'){
		return LEPT_PARSE_INVALID_VALUE;
	}
	c->json+=3;
	v->type = LEPT_TRUE;
	return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v){
	EXPECT(c,'f');
	if(c->json[0]!='a'||c->json[1]!='l'||c->json[2]!='s'||c->json[3]!='e'){
		return LEPT_PARSE_INVALID_VALUE;
	}
	c->json+=4;
	v->type = LEPT_FALSE;
	return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v){
	char *end;
	v->n = strtod(c->json, &end);
	/*end 为第一个不能转换的字符的指针*/
	if(c->json==end)
		return LEPT_PARSE_INVALID_VALUE;
	c->json = end;
	v->type = LEPT_NUMBER;
	return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c,lept_value* v){
	switch(*c->json){
		case 'n' : return lept_parse_null(c,v);
		case 't' : return lept_parse_true(c,v);
		case 'f' : return lept_parse_false(c,v);
		case '\0': /*printf("LEPT_PARSE_EXPECT_VALUE\n");*/return LEPT_PARSE_EXPECT_VALUE;
		default:   /*printf("LEPT_PARSE_INVALID_VALUE\n");*/return lept_parse_number(c,v);
	}
}

/*解析函数
//json 格式： ws value ws*/
int lept_parse(lept_value* v, const char* json){
	lept_context c;
	/*出现多个value时返回not singular*/
	int ret = 0;
	/*v 不得为空指针*/
	assert(v!=NULL);
	c.json = json;
	/*初值设为null，这样若解析失败，返回null值*/
	v->type = LEPT_NULL;
	/*先处理一个ws*/
	lept_parse_whitespace(&c);
	ret = lept_parse_value(&c,v);
	/*若ws value ws解析后还存在字符，返回not singular*/
	if(ret==LEPT_PARSE_OK){
		lept_parse_whitespace(&c);
		if(*c.json!='\0')
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
	}
	return ret;
}

lept_type lept_get_type(const lept_value* v){
	assert(v!=NULL);
	return v->type;
}

double lept_get_number(const lept_value* v){
	assert(v!=NULL&&v->type==LEPT_NUMBER);
	return v->n;
}



























