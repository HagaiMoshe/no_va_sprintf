/* a Snprintf implementation that do not use the va args
 * implemented using macros
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>


static inline void inverseString(char* _str, uint32_t _size)
{
	char tmp = 0;
	_size--;
	for(uint32_t i = 0; i < _size; i++, _size--)
	{
		tmp = _str[_size];
		_str[_size] = _str[i];
		_str[i] = tmp;
	}
}

static inline size_t ullToa(char* _str, uint64_t _num, uint8_t _base)
{
	uint8_t digit = 0, i = 0;
	_base = (16 != _base) ? 10:16; // support only base 10 and 16

	do{
        digit = _num % _base;
        _str[i++] = (digit < 10) ? digit + '0' : digit + 'A' - 10;
        _num = _num/_base;
	}while(_num != 0);

	inverseString(_str, i);
	_str[i] = 0;
	return i;
}

static inline size_t llToa(char* _str, const int64_t _num)
{
	if(_num < 0)
	{
		_str[0] = '-';
		return ullToa(_str+1, -_num, 10)+1;
	}
	else
		return ullToa(_str, _num, 10);
}

static inline uint32_t insertToBuf_int64(char* _outBuf, int64_t _n)
{
	char tmpBuf[25] = {0};
	size_t len = llToa(tmpBuf, _n);
	memcpy(_outBuf, tmpBuf, len);
	return len;
}

static inline uint32_t insertToBuf_uint64(char* _outBuf, uint64_t _n, uint8_t _base)
{
	char tmpBuf[25] = {0};
	size_t len = ullToa(tmpBuf, _n, _base);
	memcpy(_outBuf, tmpBuf, len);
	return len;
}

static inline uint32_t insertToBuf_string(char* _outBuf, char* s)
{
	size_t slen = strlen(s);
	memcpy(_outBuf, s, slen);
	return slen;
}


uint32_t mySprintf(char* _outBuf, const char* _template, uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _e, uint64_t _f, uint64_t _g, uint64_t _h, uint64_t _i, uint64_t _j)
{
	uint64_t pramList[10] = {_a, _b, _c, _d, _e, _f, _g, _h, _i, _j};
	uint32_t outBufIndx = 0, templateIndx = 0, paramIndx = 0;

	// TODO: add protection from pramList overflow
	while(0 != _template[templateIndx])
	{
		if('%' != _template[templateIndx])
		{
			_outBuf[outBufIndx++] = _template[templateIndx++];
		}
		else
		{
			switch(_template[++templateIndx])
			{
				case 'c':
					_outBuf[outBufIndx] = (char)pramList[paramIndx++];
					outBufIndx += sizeof(char);
					break;

				case 'u':
					outBufIndx += insertToBuf_uint64(&_outBuf[outBufIndx], (uint64_t)pramList[paramIndx++], 10);
					break;

				case 'x':
					outBufIndx += insertToBuf_uint64(&_outBuf[outBufIndx], (uint64_t)pramList[paramIndx++], 16);
					break;

				case 'l':
					if('l' == _template[templateIndx +1])
						templateIndx++;

					if('u' == _template[templateIndx +1])
					{
						templateIndx++;
						outBufIndx += insertToBuf_uint64(&_outBuf[outBufIndx], (uint64_t)pramList[paramIndx++], 10);
						break;
					}
				case 'd':
					outBufIndx += insertToBuf_int64(&_outBuf[outBufIndx], (int64_t)pramList[paramIndx++]);
					break;

				case 's':
					outBufIndx += insertToBuf_string(&_outBuf[outBufIndx], (char*)(size_t)pramList[paramIndx++]);
					break;

				case '%':
					_outBuf[outBufIndx++] = '%';
					break;

				default:
					templateIndx--;
			}//switch

			templateIndx++;
		}//else
	}//while

	_outBuf[outBufIndx] = 0;
	return outBufIndx;
}

enum {__ZERO = 0};
#define ARG_CHECK(VAL) ERROR_ ## VAL // error on this line means that the MY_SPRINTF is receiving too many arguments
#define ERROR___ZERO

#define DISCARD_EXTRA_PARAM(OUT_BUF,TEMPLATE,A,B,C,D,E,F,G,H,I,J,LAST,...) \
	OUT_BUF,TEMPLATE,(uint64_t)A,(uint64_t)B,(uint64_t)C,(uint64_t)D,(uint64_t)E,(uint64_t)F,(uint64_t)G,(uint64_t)H,(uint64_t)I,(uint64_t)J ARG_CHECK(LAST)
#define SPRINTF(...) mySprintf(DISCARD_EXTRA_PARAM(__VA_ARGS__,__ZERO,__ZERO,__ZERO,__ZERO,__ZERO,__ZERO,__ZERO,__ZERO,__ZERO,__ZERO,__ZERO,EMPTY))

// example
int main()
{
	char tmp[100] = {0};
	SPRINTF(tmp,"test\n");
	printf("%s", tmp);
	SPRINTF(tmp,"%s %d %u %d %c %%\n","test",0,258,-563, 't');
	printf("%s", tmp);
	SPRINTF(tmp,"test %d %l %ll %llu\n", 2, -4, 5222, 10000000000);
	printf("%s", tmp);

	return 0;
}
