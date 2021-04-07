#include "tool.h"

wchar_t* multiToUnicode(const char* pText)
{
    UINT len = MultiByteToWideChar(CP_ACP, 0, pText, -1, NULL, NULL);
    WCHAR* unicode = (WCHAR*)malloc(sizeof(WCHAR) * (len + 1));
    memset(unicode, L'\0', len + 1);
    MultiByteToWideChar(CP_ACP, 0, pText, -1, unicode, len);
    return unicode;
}

char* unicodeToUtf8(wchar_t* unicode)
{
    UINT len = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, NULL, NULL, NULL);
    char* utf8 = (char*)malloc(sizeof(char) * (len + 1));
    memset(utf8, 0, len + 1);
    WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8, len, NULL, NULL);
    return utf8;
}

wchar_t* utf8ToUnicode(const char* utf8)
{
    UINT len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, NULL);
    WCHAR* unicode = (WCHAR*)malloc(sizeof(WCHAR) * (len + 1));
    memset(unicode, L'\0', len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unicode, len);
    return unicode;
}

char* unicodeToMulti(wchar_t* unicode)
{
    UINT len = WideCharToMultiByte(CP_ACP, 0, unicode, -1, NULL, NULL, NULL, NULL);
    char* multi = (char*)malloc(sizeof(char) * (len + 1));
    memset(multi, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, unicode, -1, multi, len, NULL, NULL);
    return multi;
}