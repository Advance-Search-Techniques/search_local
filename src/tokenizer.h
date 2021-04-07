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
                            int (*xToken)(//����token->�ַ�����ӳ��
                            void* pCtx,//������
                            int tflags, //һ��Ϊ0(׼ȷ)������ֵ
                            const char* pToken,//��ǰ��������Ӧ������token(ͨ��Ϊ�������е��Ӵ�)
                            int nToken, int iStart, int iEnd)); //pToken���ֽڴ�С);
};

int tokenize(void*, int, const char*, int,
                    int (*xToken)(//����token->�ַ�����ӳ��
                    void* pCtx,//������
                    int tflags, //һ��Ϊ0(׼ȷ)������ֵ
                    const char* pToken,//��ǰ��������Ӧ������token(ͨ��Ϊ�������е��Ӵ�)
                    int nToken, int iStart, int iEnd));
