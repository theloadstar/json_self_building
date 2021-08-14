#include "leptjson.h"
#include <assert.h> /*assert()*/
#include <stdlib.h> /*NULL  strtod*/
#include <stdio.h>

/*
//个人没明白这个宏的意义，因为在lept_parse_value里都是经过switch分支进入到对应的解析里的，那么第一个字符
//一定是对应相等的，如此一来，此处的assert有何作用？
*/
#define EXPECT(c,ch) do{ assert(*c->json==(ch));c->json++;}while(0)
#define ISDIGIT(c)       ((c)>='0'&&(c)<='9')
#define ISDIGIT1TO9(c)   ((c)>='1'&&(c)<='9')
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

static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type){
	int i;
	EXPECT(c, literal[0]);
	for(i=0;literal[i+1];i++){
		if(c->json[i]!=literal[i+1])
			return LEPT_PARSE_INVALID_VALUE;
	}
	c->json+=i;
	v->type = type;
	return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v){
	/*判断格式正确性*/
	const char* p = c->json;/*常量指针，防止通过p修改json的值*/ 
	char *end;
	/*[-]*/
	if(*p=='-')p++;
	if(*p=='0')p++;
	else if(ISDIGIT1TO9(*p)){
		p++;
		while(ISDIGIT(*p))
			p++;
	}
	else
		return LEPT_PARSE_INVALID_VALUE;
	if(*p=='.'){
		p++;
		if(ISDIGIT(*p)){
			while(ISDIGIT(*p))
				p++;
		}
		else
			return LEPT_PARSE_INVALID_VALUE;
	}
	if(*p=='e'||*p=='E'){
		p++;
		if(*p=='+'||*p=='-')
			p++;
		if(ISDIGIT(*p)){
			while(ISDIGIT(*p))
				p++;
		}
		else
			return LEPT_PARSE_INVALID_VALUE;
	}
	/*get num*/
	v->n = strtod(c->json, &end);
	/*end 为第一个不能转换的字符的指针*/
	if(c->json==end)
		return LEPT_PARSE_INVALID_VALUE;
	c->json = p;
	v->type = LEPT_NUMBER;
	return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c,lept_value* v){
	switch(*c->json){
		case 'n' : return lept_parse_literal(c,v,"null",LEPT_NULL);
		case 't' : return lept_parse_literal(c,v,"true",LEPT_TRUE);
		case 'f' : return lept_parse_literal(c,v,"false",LEPT_FALSE);
		case '\0': /*printf("LEPT_PARSE_EXPECT_VALUE\n");*/return LEPT_PARSE_EXPECT_VALUE;
		default:   /*printf("LEPT_PARSE_INVALID_VALUE\n");*/return lept_parse_number(c,v);
	}
}

/*解析函数
//json 格式： ws value ws*/
int lept_parse(lept_value* v, const char* json){
	lept_context c;
    // printf("json address:%d\n",json);
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
    // printf("c.json address:%d\n",c.json);
    // printf("%s\n",json+5);
	/*若ws value ws解析后还存在字符，返回not singular*/
	if(ret==LEPT_PARSE_OK){
		lept_parse_whitespace(&c);
		if(*c.json!='\0'){
			v->type = LEPT_NULL;
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
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



























