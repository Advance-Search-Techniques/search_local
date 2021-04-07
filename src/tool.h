#pragma once
#include <Windows.h>
#include <wchar.h>
#include <stdlib.h>

wchar_t* multiToUnicode(const char*);

char* unicodeToMulti(wchar_t*);

char* unicodeToUtf8(wchar_t*);

wchar_t* utf8ToUnicode(const char*);