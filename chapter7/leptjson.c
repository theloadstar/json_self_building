#include "leptjson.h"
#include <assert.h> /*assert()*/
#include <stdlib.h> /*NULL  strtod  malloc free realloc*/
#include <stdio.h> /*sprintf()*/
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
#define PUTC(c, ch) do{ *(char*)lept_context_push(c, sizeof(char))=(ch); }while(0)
#define ISHIGHSURR(u) ((u)>=0xD800&&(u)<=0xDBFF)
#define ISLOWSURR(u)  ((u)>=0xDC00&&(u)<=0xDFFF)

/*stringify*/
#ifndef LEPT_PARSE_STRINGIFY_INIT_SIZE
#define LEPT_PARSE_STRINGIFY_INIT_SIZE 256
#endif

#define PUTS(c, s, len)     memcpy(lept_context_push(c, len), s, len)

/*为减少函数之间传递多个参数，定义json字符串结构体*/
typedef struct{
	const char* json;
	char* stack;
	size_t size, top;
}lept_context;

static int lept_parse_string(lept_context* c, lept_value* v);
static int lept_parse_array(lept_context* c, lept_value* v);
static int lept_parse_object(lept_context* c, lept_value* v);

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
		case '[' : return lept_parse_array(c,v);
		case '{' : return lept_parse_object(c,v);
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
	size_t i;
	assert(v!=NULL);
	switch(v->type){
		case LEPT_STRING:
		    free(v->u.s.s);break;
		case LEPT_ARRAY:
		    for(i=0;i<v->u.a.size;i++)
		    	lept_free(&v->u.a.e[i]);
		    free(v->u.a.e);break;
		case LEPT_OBJECT:
		    for(i=0;i<v->u.o.size;i++){
		    	free(v->u.o.m[i].k);
		    	lept_free(&v->u.o.m[i].v);
		    }
		    free(v->u.o.m);
		    break;
		default:break;
	}
	v->type = LEPT_NULL;
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
/*将4位16进制数字解析为码点*/
static const char* lept_parse_hex4(const char* p, unsigned* u) {
    /* \TODO */
    int i=0, result=0,temp = 0;
    for(;i<4;i++){
    	char ch = *(p+i);
    	if(ISDIGIT(ch))
    		temp = ch-'0';
    	else if(ch>='A'&&ch<='F')
    		temp = ch-'A'+10;
    	else if(ch>='a'&&ch<='f')
    		temp = ch-'a'+10;
    	else
    		return NULL;
    	result = result*16+temp;
    }
    *u = (unsigned)result;
    p+=4;
    return p;
}
/*将码点解析为UTF-8*/
static void lept_encode_utf8(lept_context* c, unsigned u) {
	assert(u<=0x10ffff);
    /* \TODO */
	if(u<=0x007f){
		PUTC(c, u);
	}
	else if(u>=0x0080&&u<=0x07ff){
		/*>>操作为了分割字节，即分组； &操作为了将高位置0，故第一组全部&0xff，也可以不&*/
		PUTC(c,(0xC0|((u>>6)&0xFF)));/*0xC0 = 1100 0000*/
		PUTC(c,(0x80|((u   )&0x3F)));/*0x80 = 1000 0000, 0x3F将除低六位外全置0*/
	}
	else if(u>=0x0800&&u<=0xFFFF){
		/*printf("%d %d %d\n",0xE0 | ((u >> 12) & 0xFF),0x80 | ((u >>  6) & 0x3F),0x80 | ( u        & 0x3F));
		printf("%d %d %d\n",(0xE0|((u>>12)&0xFF)),(0x80|((u>>6 )&0x3F)),(0x80|((u    )&0x3F)));*/
		PUTC(c,(0xE0|((u>>12)&0xFF)));/*0xE0=1110 0000*/
		PUTC(c,(0x80|((u>>6 )&0x3F)));/*0x80=1000 0000*/
		PUTC(c,(0x80|((u    )&0x3F)));
	}
	else if(u>=0x10000&&u<=0x10FFFF){
		PUTC(c,(0xF0|((u>>18)&0xFF)));/*0xF0=1111 0000*/
		PUTC(c,(0x80|((u>>12)&0x3F)));
		PUTC(c,(0x80|((u>> 6)&0x3F)));
		PUTC(c,(0x80|((u    )&0x3F)));
	}
}

/*refactoring*/
#define STRING_ERROR(ret) do { c->top = head; return ret; } while(0)

/*refactoring of string parse*/
/* 解析 JSON 字符串，把结果写入 str 和 len */
/* str 指向 c->stack 中的元素，需要在 c->stack  */
static int lept_parse_string_raw(lept_context* c, char** str, size_t* len) {
    size_t head = c->top;
    unsigned u, u2;
	const char* p;
	EXPECT(c,'\"');
	p = c->json;
	for(;;){
		char ch = *p++;
		switch(ch){
			case '\"':
			    *len = c->top - head;
			    /*lept_set_string(v, (const char*)lept_context_pop(c, len),len);*/
			    *str = (char*)lept_context_pop(c, *len);
			    c->json = p;
			    return LEPT_PARSE_OK;
			case '\0':
			    STRING_ERROR(LEPT_PARSE_MISS_QUOTATION_MARK);
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
			    	case 'u':
                        if (!(p = lept_parse_hex4(p, &u)))
                            STRING_ERROR(LEPT_PARSE_INVALID_UNICODE_HEX);
                        /* \TODO surrogate handling */
                        if(ISHIGHSURR(u)){
                            ch = *p++;
                            if(ch!='\\')
                               return LEPT_PARSE_INVALID_UNICODE_SURROGATE;
                            ch = *p++;
                            if(ch!='u')
                               return LEPT_PARSE_INVALID_UNICODE_SURROGATE;
                        	if(!(p=lept_parse_hex4(p, &u2)))
                        		return LEPT_PARSE_INVALID_UNICODE_SURROGATE;
                        	if(ISLOWSURR(u2)){
                        		u = 0x10000+(u-0xD800)*0x400+(u2-0xDC00);
                        	}
                        	else
                        		return LEPT_PARSE_INVALID_UNICODE_SURROGATE;
                        }
                        lept_encode_utf8(c, u);
                        break;
			    	default:
			    	STRING_ERROR(LEPT_PARSE_INVALID_STRING_ESCAPE);
			    }
			    break;
			default:
			    if((unsigned)ch<0x20){
			    	STRING_ERROR(LEPT_PARSE_INVALID_STRING_CHAR);
			    }
			    PUTC(c,ch);/*每个字符入栈*/
		}
	}
}

static int lept_parse_string(lept_context* c, lept_value* v){
	size_t len;
	char* s;
	int ret = -1;
	if((ret=lept_parse_string_raw(c,&s,&len))==LEPT_PARSE_OK)
		lept_set_string(v,s,len);
	return ret;
}

/*array*/
size_t lept_get_array_size(const lept_value* v){
	assert(v!=NULL&&v->type==LEPT_ARRAY);
	return v->u.a.size;
}
lept_value* lept_get_array_element(const lept_value* v, size_t index){
	assert(v!=NULL&&v->type==LEPT_ARRAY);
	assert(index<v->u.a.size);
	return &v->u.a.e[index];
}

static int lept_parse_array(lept_context* c, lept_value* v){
	/*size_t head = c->top;*/
	size_t size = 0, i;
	int ret;
	EXPECT(c,'[');
	/*chapter5 task2*/
	lept_parse_whitespace(c);
	/*空数组*/
	if(*c->json==']'){
		c->json++;
		v->type = LEPT_ARRAY;
		v->u.a.size = size;
		v->u.a.e = NULL;
		return LEPT_PARSE_OK;
	}
	for(;;){
		lept_value e;
		lept_init(&e);
		lept_parse_whitespace(c);
		if((ret = lept_parse_value(c,&e))!=LEPT_PARSE_OK){
			break;
		}
		memcpy(lept_context_push(c,sizeof(lept_value)),&e,sizeof(lept_value));
		size++;
		lept_parse_whitespace(c);
		if(*c->json==',')
			c->json++;
		else if(*c->json==']'){
			c->json++;
			v->type = LEPT_ARRAY;
			v->u.a.size = size;
			size *= sizeof(lept_value);
			memcpy(v->u.a.e = (lept_value*)malloc(size),lept_context_pop(c,size),size);
			return LEPT_PARSE_OK;
		}
		else{
			ret = LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
			break;
		}
	}
	for(i=0;i<size;i++)
		lept_free((lept_value*)lept_context_pop(c,sizeof(lept_value)));
	return ret;
}

/*object*/
size_t lept_get_object_size(const lept_value* v){
	assert(v!=NULL&&v->type==LEPT_OBJECT);
	return v->u.o.size;
}

const char* lept_get_object_key(const lept_value* v, size_t index){
	assert(v!=NULL&&v->type==LEPT_OBJECT&&index<v->u.o.size);
	/*printf("%s %s\n",v->u.o.m[0].k,v->u.o.m[1].k);*/
	return v->u.o.m[index].k;
}

size_t lept_get_object_key_length(const lept_value* v, size_t index){
	assert(v!=NULL&&v->type==LEPT_OBJECT&&index<v->u.o.size);
    /*printf("%zu\n",v->u.o.m[index].klen);*/
	return v->u.o.m[index].klen;
}

lept_value* lept_get_object_value(const lept_value* v, size_t index){
	assert(v!=NULL&&v->type==LEPT_OBJECT&&index<v->u.o.size);
	return &v->u.o.m[index].v;
}

static int lept_parse_object(lept_context* c, lept_value* v){
	size_t size, i;
	lept_member m;
	int ret;
    char *s = NULL;
    /*size_t len = 0;*/
	EXPECT(c, '{');
	lept_parse_whitespace(c);
	/*空对象*/
	if(*c->json == '}'){
		c->json++;
		v->type = LEPT_OBJECT;
		v->u.o.m = NULL;
		v->u.o.size = 0;
		return LEPT_PARSE_OK;
	}
	m.k = NULL;
	size = 0;
	for(;;){
		lept_init(&m.v);
		/*parse key to m.k, m.klen*/
		if(*c->json!='\"'){
			ret = LEPT_PARSE_MISS_KEY;
			break;
		}
		if((ret=lept_parse_string_raw(c,&s,&m.klen))!=LEPT_PARSE_OK)
			break;
        /*m.klen = len;*/
        memcpy(m.k = (char*)malloc(m.klen), s, m.klen);
		/*parse ws colon ws*/
		lept_parse_whitespace(c);
		if(*c->json!=':'){
			ret = LEPT_PARSE_MISS_COLON;
			break;
		}
        c->json++;
		lept_parse_whitespace(c);
		/*parse value*/
		if((ret=lept_parse_value(c,&m.v))!=LEPT_PARSE_OK){
			break;
		}
		memcpy(lept_context_push(c,sizeof(lept_member)),&m,sizeof(lept_member));
		size++;
		/*free(m.k);*/
		m.k = NULL;/* ownership is transferred to member on stack */
		/*parse ws [comma|right-curly-brace] ws*/
		lept_parse_whitespace(c);
		if(*c->json==','){
			c->json++;
			lept_parse_whitespace(c);
		}
		else if(*c->json=='}'){
			c->json++;
			v->type = LEPT_OBJECT;
			v->u.o.size = size;
			size*=sizeof(lept_member);
			memcpy(v->u.o.m=(lept_member*)malloc(size),lept_context_pop(c,size),size);
			return LEPT_PARSE_OK;
		}
		else{
			ret = LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
			break;
		}
	}
	/*pop and free members on the stacks*/
	free(m.k);
	for(i=0;i<size;i++){
		/* lept_free((lept_value*)lept_context_pop(c,sizeof(lept_member)));*/
		lept_member* m = (lept_member*)lept_context_pop(c, sizeof(lept_member));
        free(m->k);
        lept_free(&m->v);
	}
	return ret;
}

/*stringify*/
/*static void lept_stringify_string(lept_context* c, const char* s, size_t len) {
    size_t i;
    char ch;
    assert(s!=NULL);
    PUTC(c,'\"');
    for(i=0;i<len;i++){
    	ch = s[i];
    	switch(ch){
    		case '\"':PUTS(c,"\\\"",2);break;
    		case '\\':PUTS(c,"\\\\",2);break;
    		case '/' :PUTS(c,"\\/" ,2);break;
    		case '\b':PUTS(c,"\\b" ,2);break;
    		case '\f':PUTS(c,"\\f" ,2);break;
    		case '\n':PUTS(c,"\\n" ,2);break;
    		case '\r':PUTS(c,"\\r" ,2);break;
    		case '\t':PUTS(c,"\\t" ,2);break;
    		default:
	    		if(ch<0x20){
	    			char buffer[7];
	    			sprintf(buffer,"\\u%04X",ch);
	    			PUTS(c,buffer,6);
	    		}
	    		else{
	    			PUTC(c,ch);
	    		}
    	}
    }
    PUTC(c,'\"');
}*/

/*optimize*/
static void lept_stringify_string(lept_context* c, const char* s, size_t len) {
	static const char hex_digits[] = {'0','1','2','3','4','5','6','7','8','9','0','A','B','C','D','E','F'};
    size_t i, size;
    char *head, *p;
    assert(s!=NULL);
    head = p = lept_context_push(c,size = len*6+2);
    *p++ = '\"';
    for(i=0;i<len;i++){
    	unsigned char ch = (unsigned char)s[i];
    	switch(ch){
    		case '\"':*p++ = '\\';*p++ = '\"';break;
    		case '\\':*p++ = '\\';*p++ = '\\' ;break;
    		case '/' :*p++ = '\\';*p++ = '/'  ;break;
    		case '\b':*p++ = '\\';*p++ = 'b'  ;break;
    		case '\f':*p++ = '\\';*p++ = 'f'  ;break;
    		case '\n':*p++ = '\\';*p++ = 'n'  ;break;
    		case '\r':*p++ = '\\';*p++ = 'r'  ;break;
    		case '\t':*p++ = '\\';*p++ = 't'  ;break;
    		default:
	    		if(ch<0x20){
	    			*p++ = '\\';
	    			*p++ = 'u';
	    			*p++ = '0';
	    			*p++ = '0';
	    			*p++ = hex_digits[ch>>4];/*十六进制第二位*/
	    			*p++ = hex_digits[ch&15];/*十六进制首位*/
	    		}
	    		else{
	    			*p++ = ch;;
	    		}
    	}
    }
    *p++ = '\"';
    c->top -= size-(p-head);
}

static void lept_stringify_value(lept_context* c, const lept_value* v) {
	size_t i;
    switch (v->type) {
        case LEPT_NULL:   PUTS(c, "null",  4); break;
        case LEPT_FALSE:  PUTS(c, "false", 5); break;
        case LEPT_TRUE:   PUTS(c, "true",  4); break;
        case LEPT_NUMBER: c->top -= 32 - sprintf(lept_context_push(c, 32), "%.17g", v->u.n); break;
        case LEPT_STRING: lept_stringify_string(c, v->u.s.s, v->u.s.len); break;
        case LEPT_ARRAY:
            PUTC(c,'[');
            for(i=0;i<v->u.a.size;i++){
            	if(i>0)
            		PUTC(c,',');
            	lept_stringify_value(c,&v->u.a.e[i]);
            }
            PUTC(c,']');
            break;
        case LEPT_OBJECT:
            PUTC(c,'{');
            for(i=0;i<v->u.o.size;i++){
            	if(i>0)
            		PUTC(c,',');
            	lept_stringify_string(c,v->u.o.m[i].k,v->u.o.m[i].klen);
            	PUTC(c,':');
            	lept_stringify_value(c,&v->u.o.m[i].v);
            }
            PUTC(c,'}');
            break;
        default: assert(0 && "invalid type");
    }
}

char* lept_stringify(const lept_value* v, size_t* length) {
    lept_context c;
    assert(v != NULL);
    c.stack = (char*)malloc(c.size = LEPT_PARSE_STRINGIFY_INIT_SIZE);
    c.top = 0;
    lept_stringify_value(&c, v);
    if (length)
        *length = c.top;
    PUTC(&c, '\0');
    return c.stack;
}





























