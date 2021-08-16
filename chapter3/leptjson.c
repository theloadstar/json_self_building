#include "leptjson.h"
#include <assert.h> /*assert()*/
#include <stdlib.h> /*NULL  strtod  malloc free realloc*/
#include <stdio.h>
#include <errno.h>/*errno, ERANGE*/
#include <string.h>/*memcpy*/
#include <math.h>/*HUGE_VAL*/

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
	char* stack;
	size_t size, top;
}lept_context;

static int lept_parse_string(lept_context* c, lept_value* v);

/*可以直接操作c->json,但写成p使得代码可读性更强
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
	v->u.n = strtod(c->json, &end);
    /*LEPT_PARSE_NUMBER_TOO_BIG*/
    if(errno==ERANGE&&(v->u.n>=HUGE_VAL||v->u.n<=-HUGE_VAL)){
    	return LEPT_PARSE_NUMBER_TOO_BIG;
    }
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
		case '\"': return lept_parse_string(c,v);
		default:   /*printf("LEPT_PARSE_INVALID_VALUE\n");*/return lept_parse_number(c,v);
	}
}

/*解析函数*/
/*json 格式： ws value ws*/
int lept_parse(lept_value* v, const char* json){
	lept_context c;
    /*printf("json address:%d\n",json);*/
	/*出现多个value时返回not singular*/
	int ret = 0;
	/*init c*/
	c.stack = NULL;
	c.size=c.top = 0;
	lept_init(v);
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
		if(*c.json!='\0'){
			v->type = LEPT_NULL;
			ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
		}
	}
	assert(c.top==0);
	free(c.stack);
	return ret;
}

/*设置字符串的值*/
void lept_set_string(lept_value* v, const char* s, size_t len){
	/*v不得为空指针 s不得为空指针或者s为空，但len为0，即传入空串*/
	assert(v!=NULL&&(s!=NULL||len==0));
	/*释放原来的v内部的字符串空间*/
	lept_free(v);
	v->u.s.s = (char*)malloc(len+1);/*末尾补/0*/
	memcpy(v->u.s.s,s,len);
	v->u.s.len = len;
	v->type = LEPT_STRING;
}

const char* lept_get_string(const lept_value* v){
	assert(v!=NULL&&v->type==LEPT_STRING);
	return v->u.s.s;
}

size_t lept_get_string_length(const lept_value* v){
	assert(v!=NULL&&v->type==LEPT_STRING);
	return v->u.s.len;
}

void lept_free(lept_value* v){
	assert(v!=NULL);
	if(v->type==LEPT_STRING){
		free(v->u.s.s);
	}
	v->type = LEPT_NULL;/*避免重复释放*/
}

lept_type lept_get_type(const lept_value* v){
	assert(v!=NULL);
	return v->type;
}

double lept_get_number(const lept_value* v){
	assert(v!=NULL&&v->type==LEPT_NUMBER);
	return v->u.n;
}

void lept_set_number(lept_value* v, double n){
	lept_free(v);
	v->u.n = n;
	v->type = LEPT_NUMBER;
}
/*boolean*/
int lept_get_boolean(const lept_value* v){
	assert(v!=NULL&&(v->type==LEPT_TRUE||v->type==LEPT_FALSE));
	/*printf("gettest\n");
	printf(v->type==LEPT_TRUE?"GET_TRUE\n":"GET_FALSE\n");*/
	return v->type==LEPT_TRUE;
}
void lept_set_boolean(lept_value* v, int b){
	assert(v!=NULL);
	v->type = b?LEPT_TRUE:LEPT_FALSE;
}

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif

static void* lept_context_push(lept_context* c, size_t size){
	void* ret;
	assert(size>0);
	if(c->top+size>c->size){
		if(c->size==0){
			c->size = LEPT_PARSE_STACK_INIT_SIZE;
		}
		while(c->top+size>c->size){
			c->size+=c->size>>1;
		}
		c->stack = (char*)realloc(c->stack,c->size);
	}
	ret = c->stack+c->top;
	c->top+=size;
	return ret;
}

static void* lept_context_pop(lept_context* c, size_t size){
	assert(c->top>=size);
	return c->stack+(c->top-=size);
}

#define PUTC(c, ch) do{ *(char*)lept_context_push(c, sizeof(char))=(ch); }while(0)

static int lept_parse_string(lept_context* c, lept_value* v){
	/*备份栈顶*/
	size_t head = c->top, len;
	const char* p;
	EXPECT(c,'\"');
	p = c->json;
	while(1){
		char ch = *p++;
		switch(ch){
			case '\"':
			    len = c->top - head;
			    lept_set_string(v, (const char*)lept_context_pop(c, len),len);
			    c->json = p;
			    return LEPT_PARSE_OK;
			case '\0':
			    c->top = head;
			    return LEPT_PARSE_MISS_QUOTATION_MARK;
			case '\\':
			    ch = *p++;
			    switch(ch){
			    	case '\"': PUTC(c,'\"');break;
			    	case '\\': PUTC(c,'\\');break;
			    	case '/':  PUTC(c,'/'); break;
			    	case 'b':  PUTC(c,'\b');break;
			    	case 'f':  PUTC(c,'\f');break;
			    	case 'n':  PUTC(c,'\n');break;
			    	case 'r':  PUTC(c,'\r');break;
			    	case 't':  PUTC(c,'\t');break;
			    	default:
			    	    c->top = head;
			    	    return LEPT_PARSE_INVALID_STRING_ESCAPE;
			    }
			    break;
			default:
			    PUTC(c,ch);/*每个字符入栈*/
		}
	}
}

























