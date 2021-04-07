#include "util.h"

util::ErrorMap::ErrorMap()
{
	sqlite3Error[SQLITE_OK] = "SQLITE_OK";
	sqlite3Error[SQLITE_ERROR] = "SQLITE_ERROR";
	sqlite3Error[SQLITE_INTERNAL] = "SQLITE_INTERNAL";
	sqlite3Error[SQLITE_PERM] = "SQLITE_PERM";
	sqlite3Error[SQLITE_ABORT] = "SQLITE_ABORT";
	sqlite3Error[SQLITE_BUSY] = "SQLITE_BUSY";
	sqlite3Error[SQLITE_LOCKED] = "SQLITE_LOCKED";
	sqlite3Error[SQLITE_NOMEM] = "SQLITE_NOMEM";
	sqlite3Error[SQLITE_READONLY] = "SQLITE_READONLY";
	sqlite3Error[SQLITE_INTERRUPT] = "SQLITE_INTERRUPT";
	sqlite3Error[SQLITE_IOERR] = "SQLITE_IOERR";
	sqlite3Error[SQLITE_CORRUPT] = "SQLITE_CORRUPT";
	sqlite3Error[SQLITE_NOTFOUND] = "SQLITE_NOTFOUND";
	sqlite3Error[SQLITE_FULL] = "SQLITE_FULL";
	sqlite3Error[SQLITE_CANTOPEN] = "SQLITE_CANTOPEN";
	sqlite3Error[SQLITE_PROTOCOL] = "SQLITE_PROTOCOL";
	sqlite3Error[SQLITE_EMPTY] = "SQLITE_EMPTY";
	sqlite3Error[SQLITE_SCHEMA] = "SQLITE_SCHEMA";
	sqlite3Error[SQLITE_TOOBIG] = "SQLITE_TOOBIG";
	sqlite3Error[SQLITE_CONSTRAINT] = "SQLITE_CONSTRAINT";
	sqlite3Error[SQLITE_MISMATCH] = "SQLITE_MISMATCH";
	sqlite3Error[SQLITE_MISUSE] = "SQLITE_MISUSE";
	sqlite3Error[SQLITE_NOLFS] = "SQLITE_NOLFS";
	sqlite3Error[SQLITE_AUTH] = "SQLITE_AUTH";
	sqlite3Error[SQLITE_FORMAT] = "SQLITE_FORMAT";
	sqlite3Error[SQLITE_RANGE] = "SQLITE_RANGE";
	sqlite3Error[SQLITE_NOTADB] = "SQLITE_NOTADB";
	sqlite3Error[SQLITE_NOTICE] = "SQLITE_NOTICE";
	sqlite3Error[SQLITE_WARNING] = "SQLITE_WARNING";
	sqlite3Error[SQLITE_ROW] = "SQLITE_ROW";
	sqlite3Error[SQLITE_DONE] = "SQLITE_DONE";
}

const char* util::ErrorMap::operator[](int code)
{
	return sqlite3Error[code];
}

std::string util::sqlite3VsnPrintf(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char tmp[MAXLEN];
	std::vsnprintf(tmp, MAXLEN, format, ap);
	va_end(ap);
	std::string s(tmp);
	return s;
}
