#ifndef TEXTGPS_INTERFACE_H
#define TEXTGPS_INTERFACE_H

//compile sqlite3 as c style
#ifdef __cplusplus
extern "C" {
#endif
#include "sqlite3.h"
#include "tool.h"
#ifdef __cplusplus
}
#endif

#include <nan.h>
#include <vector>
#include <string>
#include <unordered_set>
#include <tuple>
#include <algorithm>
#include <set>
#include "log.h"

/**
 * typical dynamic link lib macro
**/
#ifdef BUILD_FOR_SHARE
#define TEXTGPS_API_EXPORT __declspec(dllexport)
#define TEXTGPS_API_IMPORT __declspec(dllimport)
#else
#define TEXTGPS_API_EXPORT
#define TEXTGPS_API_IMPORT
#endif

#ifdef DEBUG
#define LOGGER if (rc != SQLITE_OK) \
{\
	logger->update(__TIME__, __FILE__, __LINE__, TextGpsLogger::Critical);\
}
#else
#define LOGGER
#endif // DEBUG


#define SQLITE3_CODE(rc,db) if (rc!=SQLITE_OK)\
						 {\
							sqlite3_errmsg(db);\
							exit(EXIT_FAILURE);\
					     }
#define RC_ERROR_V2 if (rc != SQLITE_OK && rc != SQLITE_ROW && rc!=SQLITE_DONE)\
					return rc


#define All_Sort_Routing(target)										   \
target* self = Nan::ObjectWrap::Unwrap<target>(info.This());		   	   \
	if (!info[0]->IsUndefined()) {										   \
		return Nan::ThrowSyntaxError(									   \
			Nan::New("all sort methods have no parameter").ToLocalChecked()\
		);																   \
	}
// StringContainer contains contents from the usual select * query
typedef std::vector<std::vector<std::string>> StringContainer;

// InfoContainer contains lists from title to content,query locations and scores
typedef std::vector<std::tuple<std::vector<std::string>,
	std::vector<std::pair<int64_t,int64_t>>,std::vector<int64_t>>> InfoContainer;

// an element of InfoContainer
typedef std::tuple<std::vector<std::string>, std::vector<std::pair<int64_t, int64_t>>, 
	std::vector<int64_t>> InfoTuple;

//final result of InfoContainer.The Reason why not to use InfoContainer to return is because
//swig python does not support c++ tuple.
typedef std::vector<std::pair<std::vector<std::string>, std::vector<int64_t>>> InfoResult;

//An element of InfoResult
typedef std::pair<std::vector<std::string>, std::vector<int64_t>> pairResult;


/**
 * DataBase is an interface class
 * which wraps some sqlite3 api and
 * implements all functions be used
 * in TextGps python end
**/
class DataBase : public Nan::ObjectWrap
{
public:
	static v8::Persistent<v8::FunctionTemplate> constructor;
	static NAN_MODULE_INIT(Init);
	static NAN_GETTER(ResultGetter);
	static NAN_SETTER(ResultSetter);
	static NAN_METHOD(New);
	static NAN_METHOD(Open);
	static NAN_METHOD(Close);
	static NAN_METHOD(Execute);
	static NAN_METHOD(Commit);
	static NAN_METHOD(SortByScoreAscent);
	static NAN_METHOD(SortByScoreDescent);
	static NAN_METHOD(SortBySizeAscent);
	static NAN_METHOD(SortBySizeDescent);
	static NAN_METHOD(SortByCTimeAscent);
	static NAN_METHOD(SortByCTimeDescent);
	static NAN_METHOD(SortByMTimeAscent);
	static NAN_METHOD(SortByMTimeDescent);
	static NAN_METHOD(SortByTitleAscent);
	static NAN_METHOD(SortByTitleDescent);
	static NAN_METHOD(SortByFormatAscent);
	static NAN_METHOD(SortByFormatDescent);
	static NAN_METHOD(SortByPathAscent);
	static NAN_METHOD(SortByPathDescent);
	static NAN_METHOD(SetPivot);
	std::vector<InfoContainer> icArray;
	DataBase()=default;
	~DataBase();
	int open(const char*);
	int bindParameters(std::vector<std::string>&);
	int prepare(const char*);
	int step();
	int reset();
	int finalize();
	int close();
	int execute(const char*);
	int execute(const char*, const StringContainer&);
	void commit()
	{
		int rc=execute("COMMIT;");
		LOGGER
	}

	void rollback()
	{
		int rc=execute("ROLLBACK;");
		if (rc != SQLITE_OK)
		{
			logger->update(__TIME__, __FILE__, __LINE__, TextGpsLogger::Critical);
		}
	}
	StringContainer fetchAll();
	void virtualFetchAll();
	void sortByScoreDescent();
	void sortByScoreAscent();
	void sortBySizeDescent();
	void sortBySizeAscent();
	void sortByCTimeDescent();
	void sortByCTimeAscent();
	void sortByMTimeDescent();
	void sortByMTimeAscent();
	void sortByTitleDescent();
	void sortByTitleAscent();
	void sortByFormatDescent();
	void sortByFormatAscent();
	void sortByPathDescent();
	void sortByPathAscent();
	void setPivot(bool);
	InfoResult getInfo();
	InfoResult getResult() const;
	sqlite3* getDB() const { return db; }
	std::string getError (size_t error)
	{
		return std::string(sqlite3_errmsg(db));
	}
	void setQuery(std::string q)
	{
		query=q.c_str();
		/*query = q;*/
	}
	void setCase(bool value);
	void setTop(int value) { top = value; }
	void queryArraySearch(std::vector<std::string> queries,std::vector<std::string> logics);
private:
	sqlite3* db;
	sqlite3_stmt* stmt;
	std::string dbFile;
	InfoContainer ic;
	std::string query;
	InfoResult ir;
	bool caseSensitive;
	int top = -1;
	TextGpsLogger* logger;
	void containerToResult();
};

inline std::string insertSpace(std::string& s)
{
	std::wstring newString = utf8ToUnicode(s.c_str());
	/*std::wstring newString = multiToUnicode(s.c_str());*/
	for (size_t i = 0; i < newString.length(); i++)
	{
		if (i % 2 != 0)
			newString.insert(i, L" ");
	}
	std::string s1 = unicodeToUtf8(const_cast<wchar_t*>(newString.c_str()));
	return s1;
}

#endif