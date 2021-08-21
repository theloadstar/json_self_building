本节为解析器部分的最后一个内容，即JSON对象的解析

# JSON对象

与数组类似，不同的是对象使用花括号`{}` (`U+007B` `U+007D`) 进行解析，并且由对象成员`member`组成。对象成员为键值对，其中键为JSON字符串，值为JSON值。（数组内部的成员即为JSON值）。完整语法：

```JSON
member = string ws %x3A ws value
object = %x7B ws [ member *( ws %x2C ws member ) ] ws %x7D
```

对应符号：

| 码点 | 符号 |
| :--: | :--: |
| 007B |  {   |
| 007D |  }   |
| 002C |  ,   |
| 003A |  :   |

使用动态数组的结构来存储对象，动态功能将在第八节加入，具体的数据结构实现如下：

```C
typedef struct lept_value lept_value;
typedef struct lept_member lept_member;

struct lept_value {
    union {
        struct { lept_member* m; size_t size; }o;
        struct { lept_value* e; size_t size; }a;
        struct { char* s; size_t len; }s;
        double n;
    }u;
    lept_type type;
};

struct lept_member {
    char* k; size_t klen;   /* member key string, key string length */
    lept_value v;           /* member value */
};
```

即存储对象的结构为`lept_value`+字符串

对应的访问函数；

```C
size_t lept_get_object_size(const lept_value* v);
const char* lept_get_object_key(const lept_value* v, size_t index);
size_t lept_get_object_key_length(const lept_value* v, size_t index);
lept_value* lept_get_object_value(const lept_value* v, size_t index);
```

# String_Parse重构

由于JSON的键值也是一个字符串，而我们使用`lept_value`存储的话会造成`type`字段的浪费，而`lept_parse_string()` 是直接地把解析的结果写进一个 `lept_value`，故我们需要将`lept_parse_string()` 拆分重构，具体的代码如下：（Task1）

```C
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
```

重构前后均通过测试