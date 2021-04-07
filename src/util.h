#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "sqlite3.h"
#ifdef __cplusplus
}
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include <Windows.h>
#define ERROR_LOG(x) std::fprintf(stderr,\
	"SQLITE3 RETURNS AN ERROR:%s AT LINE %d,FILE %s\n",x, \
	__LINE__,__FILE__)

#define MAXLEN 1024

namespace util {
	
	class ErrorMap
	{
	public:
		ErrorMap();
		const char* operator[] (int code);
	private:
		std::unordered_map<int,const char*> sqlite3Error;
	};

	std::string sqlite3VsnPrintf(const char* format, ...);
}