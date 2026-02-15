#include <cstdio>

// 为旧版本LUA库提供兼容性符号
FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
    return _iob;
}