#pragma once
#define UTF8_CHINESE
#ifdef BUILD_FOR_SHARE
#define TOKEN_API_EXPORT __declspec(dllexport)
#define TOKEN_API_IMPORT __declspec(dllimport)
#else
#define TOKEN_API_EXPORT
#define TOKEN_API_IMPORT
#endif
#ifdef UTF8_CHINESE
#define MAX_BYTES 4
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "tool.h"

typedef struct SimpleTokenizer SimpleTokenizer;

struct TOKEN_API_EXPORT SimpleTokenizer
{
    int (*simpleTokenizer)(void*, int, const char*, int,
                            int (*xToken)(//插入token->字符串的映射
                            void* pCtx,//上下文
                            int tflags, //一般为0(准确)或上述值
                            const char* pToken,//当前串中所对应的索引token(通常为整个串中的子串)
                            int nToken, int iStart, int iEnd)); //pToken的字节大小);
};

int tokenize(void*, int, const char*, int,
                    int (*xToken)(//插入token->字符串的映射
                    void* pCtx,//上下文
                    int tflags, //一般为0(准确)或上述值
                    const char* pToken,//当前串中所对应的索引token(通常为整个串中的子串)
                    int nToken, int iStart, int iEnd));
