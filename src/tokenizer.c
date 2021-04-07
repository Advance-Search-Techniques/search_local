#include "tokenizer.h"
#include <ctype.h>

int tokenize(void* pCtx, int flag, const char* pText, int nText,
             int (*xToken)(//插入token->字符串的映射
             void* pCtx,//上下文
             int tflags, //一般为0(准确)或上述值
             const char* pToken,//当前串中所对应的索引token(通常为整个串中的子串)
             int nToken, int iStart, int iEnd))
{
    int rc = SQLITE_OK;
    size_t i;
    size_t j = 0;
    wchar_t temp[2];
    char* utf8 = NULL;
    memset(temp, 0, sizeof(temp));
    wchar_t* pText_unicode = utf8ToUnicode(pText);
   /* wchar_t* pText_unicode = multiToUnicode(pText);*/
    size_t unicode_len = wcslen(pText_unicode);
    if (flag == FTS5_TOKENIZE_DOCUMENT)
    {
        for (i = 0; i < unicode_len; i++)
        {
            temp[0] = pText_unicode[i];
            /* if (temp[0] == L' ' || temp[0] == L'\t' || temp[0] == L'\n')
                 continue;*/
            utf8 = unicodeToUtf8(temp);
            rc = xToken(pCtx, 0, utf8, strlen(utf8), j, j + strlen(utf8));
            if ((temp[0]&0x80) != 0x80)
            {
                temp[0] = iswlower(temp[0]) ? towupper(temp[0]) : towlower(temp[0]);
                free(utf8);
                utf8 = unicodeToUtf8(temp);
                rc = xToken(pCtx, FTS5_TOKEN_COLOCATED, utf8, strlen(utf8), j, j + strlen(utf8));
            }
            j += strlen(utf8);
            free(utf8);
        }
    }
    if (flag == FTS5_TOKENIZE_QUERY)
    {
        rc = xToken(pCtx, 0, pText, nText, 0, nText - 1);
    }
    free(pText_unicode);
    pText_unicode = NULL;
    utf8 = NULL;
    return rc;
}