#ifndef _TEXTGPS_LOG_
#define _TEXTGPS_LOG_

#include <stdio.h>
#include "sqlite3.h"
#include <string>

enum class ErrorCode
{
    RUNTIME_ERROR = 0,
    INPUT_ERROR,
    OUTPUT_ERROR,
    LOGICAL_ERROR
};

class TextGpsLogger
{
public:
    constexpr static const char* Critical = "[CRITICAL]";
    constexpr static const char* Warning = "[WARNING]";
    constexpr static const char* Info = "[INFO]";
    constexpr static const char* Debug = "[DEBUG]";
    explicit TextGpsLogger(const char* logPath, sqlite3* _db): db(_db)
    {
        fopen_s(&fileHandle, logPath, "a");
    }
    void update(const char* time,const char* file,int line,const char* level)
    {
        fprintf(fileHandle, "(From kernel) %s:%s---%s---%d---ERROR:%s\n",
            level, time, file, line, sqlite3_errmsg(this->db));
    }
    void update(const char* level)
    {
        fprintf(fileHandle, "(From kernel) %s:%s---%s---%d---ERROR:%s\n",
            level, __TIME__, __FILE__, __LINE__, sqlite3_errmsg(this->db));
    }
    void update(const char* level, const char* reason)
    {
        fprintf(fileHandle, "(From kernel) %s:%s---%s---%d---ERROR:%s\n",
            level, __TIME__, __FILE__, __LINE__, reason);
    }

private:
    sqlite3* db;
    FILE* fileHandle;
};

#endif //_TEXTGPS_LOG_