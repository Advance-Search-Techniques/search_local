#include "interface.h"
#include <algorithm>
#include <iostream>
#include <cctype>

extern "C" {
#include "auxfunc.h"
#include "tool.h"
#include <wctype.h>
}

v8::Persistent<v8::FunctionTemplate> DataBase::constructor;

NAN_MODULE_INIT(DataBase::Init) {
	v8::Local<v8::Context> context = target->CreationContext();
	v8::Local<v8::FunctionTemplate> ctor = Nan::New<v8::FunctionTemplate>(DataBase::New);
	constructor.Reset(target->GetIsolate(),ctor);
	ctor->InstanceTemplate()->SetInternalFieldCount(1);
	ctor->SetClassName(Nan::New("TextGps").ToLocalChecked());

	Nan::SetAccessor(ctor->InstanceTemplate(),Nan::New("result").ToLocalChecked(),
	DataBase::ResultGetter,DataBase::ResultSetter);

	Nan::SetPrototypeMethod(ctor,"result",Result);
	Nan::SetPrototypeMethod(ctor,"open",Open);

	target->Set(context,Nan::New("TextGps").ToLocalChecked(),
	ctor->GetFunction(context).ToLocalChecked());
}


NAN_METHOD(DataBase::New) {
	v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
	if (!info.IsConstructCall()) {
		v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
		info.GetReturnValue().Set(cons->NewInstance(context,0,nullptr).ToLocalChecked());
	} else {
		DataBase* database = new DataBase();
		database->Wrap(info.Holder());
		info.GetReturnValue().Set(info.Holder());
	}
}

NAN_METHOD(DataBase::Open) {
	if (!info[0]->IsString()) {
		return Nan::ThrowTypeError(Nan::New(
			"Argument Path must be string type").ToLocalChecked());
	}
	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	const char* path = *Nan::Utf8String(info[0]);
	int rc = self->open(path);
	info.GetReturnValue().Set(rc);
}

NAN_METHOD(DataBase::Close) {
	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	int rc = self->close();
	info.GetReturnValue().Set(rc);
}

NAN_METHOD(DataBase::Execute) {
	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	if (!info[0]->IsString()) {
		return Nan::ThrowTypeError(Nan::New(
			"Argument sql sentence must be string type"
		).ToLocalChecked());
	}
	if (!info[1]->IsNull() || !info[1]->IsUndefined()) {
		v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
		if (info[1]->IsArray()) {
			v8::Local<v8::Object> obj = info[1]->ToObject(context).ToLocalChecked();
			v8::Local<v8::Array> props = obj->GetPropertyNames(context).ToLocalChecked();
			StringContainer stringContainer;
			for (int i = 0; i<props->Length();++i) {
				std::vector<std::string> sc;
				v8::Local<v8::Object> subObj = 
				props->Get(context,i).ToLocalChecked()->ToObject(context).ToLocalChecked();
				v8::Local<v8::Array> subProps = subObj->GetPropertyNames(context).ToLocalChecked();
				for (int j = 0;j<subProps->Length();++j) {
					sc.push_back(*Nan::Utf8String(subProps->Get(context,i).ToLocalChecked()));
				}
				stringContainer.push_back(sc);
			}
			int rc = self->execute(*Nan::Utf8String(info[0]),stringContainer);
			info.GetReturnValue().Set(rc);
		} else {
			return Nan::ThrowTypeError(Nan::New("To use step,make sure the second
			 parameter must be array of array"));
		}
	} else {
		int rc = self->execute(*Nan::Utf8String(info[0]));
		info.GetReturnValue().Set(rc);
	}
}
DataBase::~DataBase()
{
	free(rContainer.results);
	rContainer.results = NULL;
#ifdef DEBUG 
	delete logger;
	logger = nullptr;
#endif
}
void DataBase::containerToResult()
{
	ir.clear();
	std::pair<std::vector<std::string>, std::vector<int64_t>> tmp;
	if (top >= ic.size() || top == -1)
		top = ic.size();
	for (int i=0;i<top;++i)
	{
		if (!pivot)
		{
			if (rContainer.results != NULL)
			{
				free(rContainer.results);
				rContainer.results = NULL;
			}
			return;
		}
		tmp.first.insert(tmp.first.end(), std::get<0>(ic[i]).begin(),
			std::get<0>(ic[i]).end());
		for (auto& e : std::get<1>(ic[i]))
		{
			tmp.second.push_back(e.first);
			tmp.second.push_back(e.second);
		}
		tmp.second.insert(tmp.second.end(), std::get<2>(ic[i]).begin(),
			std::get<2>(ic[i]).end());
		ir.push_back(tmp);
		tmp.first.clear();
		tmp.second.clear();
	}
}
int DataBase::open(const char* path)
{
	db = nullptr;
	dbFile = path;
	stmt = nullptr;
	caseSensitive = true;
	int rc;
	rContainer = initResultsContainer(1024);
	pivot = true;
	rc = sqlite3_open_v2(dbFile.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
#ifdef DEBUG
	logger = new TextGpsLogger(logPath, db);
#endif
	LOGGER
	rc=execute("BEGIN TRANSACTION;");
	LOGGER
	return rc;
}

int DataBase::execute(const char* sql)
{
	int rc;
	rc = sqlite3_exec(db, sql, NULL, NULL, NULL);
	LOGGER
	return rc;
}

int DataBase::execute(const char* sql, const StringContainer& sc)
{
	int rc = prepare(sql);
	for (std::vector<std::string>& params : sc)
	{
		rc = bindParameters(params);
		rc = step();
		/*if (rc == SQLITE_DONE)
			break;*/
		rc = reset();
	}
	rc = finalize();
	return rc;
}



int DataBase::close()
{
	int rc = sqlite3_close_v2(db);
	LOGGER
	return rc;
}

int DataBase::prepare(const char* sql)
{
	int rc = sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &stmt, NULL);
	LOGGER
	return rc;
}

int DataBase::bindParameters(std::vector<std::string>& params)
{
	int rc=SQLITE_OK;
	for (int i = 0; i < params.size(); i++)
	{
		rc = sqlite3_bind_text(stmt, i+1, params[i].c_str(), -1, SQLITE_TRANSIENT);
		LOGGER
	}
	return rc;
}

int DataBase::step()
{
	int rc = sqlite3_step(stmt);
	LOGGER
	return rc;
}

int DataBase::reset()
{
	int rc = sqlite3_reset(stmt);
	LOGGER
	return rc;
}

int DataBase::finalize()
{
	int rc = sqlite3_finalize(stmt);
	LOGGER
	return rc;
}

StringContainer DataBase::fetchAll()
{
	int rc;
	StringContainer result;
	std::vector<std::string> tmp;
	int nCols;
	while (1)
	{
		rc = step();
		if (rc == SQLITE_DONE)
			break;
		nCols = sqlite3_column_count(stmt);
		for (int i = 0; i < nCols; i++)
		{
			tmp.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i))));
		}
		result.push_back(tmp);
		tmp.clear();
	}
	rc = finalize();
	LOGGER
	return result;
}

void DataBase::virtualFetchAll()
{
	int rc;
	ic.clear();
	std::vector<std::string> tmp;
	std::vector<std::pair<int64_t,int64_t>> loc;
	std::vector<std::int64_t> num;
	while(1)
	{
		rc = step();
		if (!pivot)
		{
			if (rContainer.results != NULL)
			{
				free(rContainer.results);
				rContainer.results=NULL;
			}
			break;
		}
		if (rc == SQLITE_DONE)
			break;
	}
	rc = finalize();
	LOGGER
	for (size_t i = 0; i < rContainer.len; i++)
	{
		if (!pivot)
		{
			ic.clear();
			tmp.clear();
			num.clear();
			loc.clear();
			if (rContainer.results != NULL)
			{
				free(rContainer.results);
				rContainer.results=NULL;
			}
			return;
		}
		int matchs=0;
		tmp.push_back(std::string(rContainer.results[i].title));
		/*tmp.push_back(std::string(rContainer.results[i].title));*/
		tmp.push_back(std::string(rContainer.results[i].extension));
		tmp.push_back(std::string(rContainer.results[i].cTime));
		tmp.push_back(std::string(rContainer.results[i].mTime));
		tmp.push_back(std::string(rContainer.results[i].path));
		tmp.push_back(std::string(rContainer.results[i].size));
		std::wstring temp = utf8ToUnicode(rContainer.results[i].content);
		for (size_t j = 0; j < rContainer.results[i].pos.length; j++)
		{
			std::wstring compare=temp.substr(rContainer.results[i].pos.offsets[j].start,
				rContainer.results[i].pos.offsets[j].len);
			if (caseSensitive)
			{
				for (auto& c : compare)
				{
					if ((c & 0x80) != 0x80)
						c = iswlower(c) ? c : towlower(c);
				}
				for (auto& c : query)
				{
					if ((c & 0x80) != 0x80)
						c = islower(c) ? c : tolower(c);
				}
			}
			std::string d = unicodeToUtf8(const_cast<wchar_t*>(compare.c_str()));
			if (d == query)
			{
				loc.push_back(std::make_pair(static_cast<int64_t>(rContainer.results[i].pos.offsets[j].start),
					static_cast<int64_t>(rContainer.results[i].pos.offsets[j].end)));
				matchs += 1;
			}
		}
		if (!loc.empty())
		{
			tmp.push_back(std::string(rContainer.results[i].content));
			/*tmp.push_back(std::string(rContainer.results[i].content));*/
			num.push_back(static_cast<int64_t>(rContainer.results[i].rowId));
			num.push_back(static_cast<int64_t>(matchs));
			ic.push_back(std::make_tuple(tmp,loc,num));
		}
		free(rContainer.results[i].title);
		rContainer.results[i].title = NULL;
		free(rContainer.results[i].extension);
		rContainer.results[i].extension = NULL;
		free(rContainer.results[i].cTime);
		rContainer.results[i].cTime = NULL;
		free(rContainer.results[i].mTime);
		rContainer.results[i].mTime = NULL;
		free(rContainer.results[i].path);
		rContainer.results[i].path = NULL;
		free(rContainer.results[i].size);
		rContainer.results[i].size = NULL;
		free(rContainer.results[i].content);
		rContainer.results[i].content = NULL;
		tmp.clear();
		num.clear();
		loc.clear();
	}
	rContainer.clear(&rContainer);
}

void DataBase::sortByScoreDescent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult&b) {
		return *(a.second.end() - 1) > * (b.second.end() - 1); });
}

void DataBase::sortByScoreAscent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return *(a.second.end() - 1) < * (b.second.end() - 1); });
}

void DataBase::sortBySizeDescent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return std::stoll(a.first[5]) > std::stoll(b.first[5]); });
}

void DataBase::sortBySizeAscent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return std::stoll(a.first[5]) < std::stoll(b.first[5]); });
}

void DataBase::sortByCTimeDescent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return std::stoll(a.first[2]) > std::stoll(b.first[2]); });
}

void DataBase::sortByCTimeAscent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return std::stoll(a.first[2]) < std::stoll(b.first[2]); });
}

void DataBase::sortByMTimeDescent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return std::stoll(a.first[3]) > std::stoll(b.first[3]); });
}

void DataBase::sortByMTimeAscent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return std::stoll(a.first[3]) < std::stoll(b.first[3]); });
}

void DataBase::sortByTitleDescent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return a.first[0] < b.first[0]; });
}

void DataBase::sortByTitleAscent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return a.first[0] > b.first[0]; });
}
void DataBase::sortByFormatDescent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return a.first[1] < b.first[1]; });
}

void DataBase::sortByFormatAscent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return a.first[1] > b.first[1]; });
}

void DataBase::sortByPathDescent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return a.first[4] < b.first[4]; });
}

void DataBase::sortByPathAscent()
{
	std::stable_sort(ir.begin(), ir.end(), [](const pairResult& a, const pairResult& b) {
		return a.first[4] > b.first[4]; });
}

void DataBase::setPivot(bool value)
{
	pivot = value;
}

InfoResult DataBase::getInfo()
{
	containerToResult();
	sortByScoreDescent();
	return ir;
}

InfoResult DataBase::getResult() const
{
	return ir;
}


//void DataBase::queryArraySearch(std::vector<std::string> queries, std::vector<std::string> logics)
//{
//	int rc;
//	std::string sqls = "select func(test) from test where content match ";
//	for (auto query : queries)
//	{
//		setQuery(query);
//		std::string queryWithSpace = insertSpace(query);
//		std::string sql=sqls + "\'" + queryWithSpace + "\';";
//		rc = prepare(sql.c_str());
//		LOGGER
//		virtualFetchAll();
//		icArray.push_back(ic);
//	}
//	ic=*icArray.begin();
//	for (size_t i = 0; i < logics.size(); i++)
//	{
//		if (!pivot)
//		{
//			ic.clear();
//			icArray.clear();
//			icTable.clear();
//			if (rContainer.results != NULL)
//			{
//				free(rContainer.results);
//				rContainer.results=NULL;
//			}
//			return;
//		}
//		for (const auto& each : ic)
//		{
//			icTable.insert(each);
//		}
//
//		for (const auto& each : icArray[i + 1])
//		{
//			icTable.insert(each);
//		}
//
//		if (logics[i] == "OR")
//		{
//			ic.clear();
//			size_t totolBuckets = icTable.bucket_count();
//			for (size_t i = 0; i < totolBuckets; i++)
//			{
//				if (icTable.bucket_size(i) == 1)
//				{
//					auto it = icTable.begin(i);
//					ic.push_back(const_cast<InfoTuple&>(*it));
//				}
//				if (icTable.bucket_size(i) > 1)
//				{
//					InfoTuple temp;
//					std::get<0>(temp) = std::get<0>(const_cast<InfoTuple&>(*icTable.begin(i)));
//					std::get<2>(temp).push_back(*std::get<2>(const_cast<InfoTuple&>(*icTable.begin(i))).begin());
//					std::get<2>(temp).push_back(0);
//					std::for_each(icTable.begin(i), icTable.end(i), [&temp](const decltype(*icTable.begin(i))& a) {
//						std::get<1>(temp).insert(std::get<1>(temp).end(),
//							std::get<1>(const_cast<InfoTuple&>((a))).begin(), 
//							std::get<1>(const_cast<InfoTuple&>((a))).end());
//						*(std::get<2>(temp).end() - 1) += *(std::get<2>(const_cast<InfoTuple&>((a))).end() - 1);
//						});
//					std::stable_sort(std::get<1>(temp).begin(), std::get<1>(temp).end(),
//						[](const std::pair<int64_t, int64_t>& a, const std::pair<int64_t, int64_t>& b) {
//							return a.first < b.first;
//						});
//					ic.push_back(temp);
//				}
//			}
//		}
//		else if (logics[i] == "AND")
//		{
//			ic.clear();
//			size_t totolBuckets = icTable.bucket_count();
//			for (size_t i = 0; i < totolBuckets; i++)
//			{
//				if (icTable.bucket_size(i) > 1)
//				{
//					InfoTuple temp;
//					std::get<0>(temp) = std::get<0>(const_cast<InfoTuple&>(*icTable.begin(i)));
//					std::get<2>(temp).push_back(*std::get<2>(const_cast<InfoTuple&>(*icTable.begin(i))).begin());
//					std::get<2>(temp).push_back(0);
//					std::for_each(icTable.begin(i), icTable.end(i), [&temp](const decltype(*icTable.begin(i))& a) {
//						std::get<1>(temp).insert(std::get<1>(temp).end(),
//							std::get<1>(const_cast<InfoTuple&>((a))).begin(),
//							std::get<1>(const_cast<InfoTuple&>((a))).end());
//						*(std::get<2>(temp).end() - 1) += *(std::get<2>(const_cast<InfoTuple&>((a))).end() - 1);
//						});
//					std::stable_sort(std::get<1>(temp).begin(), std::get<1>(temp).end(),
//						[](const std::pair<int64_t, int64_t>& a, const std::pair<int64_t, int64_t>& b) {
//							return a.first < b.first;
//						});
//					ic.push_back(temp);
//				}
//			}
//		}
//		else if (logics[i] == "NOT")
//		{
//			if (i==0)
//				ic = icArray[i];
//			size_t totolBuckets = icTable.bucket_count();
//			for (size_t i = 0; i < totolBuckets; i++)
//			{
//				if (icTable.bucket_size(i) > 1)
//				{
//					std::for_each(icTable.begin(i), icTable.end(i),
//						[this](const decltype(*icTable.begin(i))& a) {
//							auto it = std::find(ic.begin(), ic.end(), a);
//							if (it != ic.end())
//								ic.erase(it);
//						});
//				}
//			}
//		}
//		else
//		{
//			logger->update(TextGpsLogger::Critical,"Wrong logic");
//		}
//		icTable.clear();
//	}
//	icArray.clear();
//}

void DataBase::queryArraySearch(std::vector<std::string> queries, std::vector<std::string> logics)
{
	int rc;
	std::string sqls = "select func(test) from test where content match ";
	icArray.clear();
	ic.clear();
	for (auto query : queries)
	{
		setQuery(query);
		std::string queryWithSpace = insertSpace(query);
		std::string sql = sqls + "\'" + queryWithSpace + "\';";
		rc = prepare(sql.c_str());
		LOGGER
		virtualFetchAll();
		icArray.push_back(ic);
	}
	ic.clear();
	InfoContainer temp_ic;
	std::copy(icArray[0].begin(), icArray[0].end(), std::back_inserter(ic));
	std::set<std::string> a, b, c;
	for (size_t i = 0; i < logics.size(); i++)
	{
		if (!pivot)
		{
			ic.clear();
			icArray.clear();
			if (rContainer.results != NULL)
			{
				free(rContainer.results);
				rContainer.results=NULL;
			}
			return;
		}

		if (logics[i] == "AND")
		{
			for (auto& j : ic)
			{
				a.insert(std::get<0>(j)[4]);
			}
			for (auto& j : icArray[i + 1])
			{
				b.insert(std::get<0>(j)[4]);
			}
			std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
			for (auto& each : c)
			{
				auto it1 = std::find_if(ic.begin(), ic.end(),
					[&each](const decltype(*ic.begin())& tmp) {
						return std::get<0>(tmp)[4] == each;
					});
				auto it2 = std::find_if(icArray[i+1].begin(), icArray[i+1].end(),
					[&each](const decltype(*icArray[i+1].begin())& tmp) {
						return std::get<0>(tmp)[4] == each;
					});
				if (it1 != ic.end() && it2!= icArray[i+1].end())
				{
					std::copy(std::get<1>(*it2).begin(), std::get<1>(*it2).end(),
						std::back_inserter(std::get<1>(*it1)));
					std::stable_sort(std::get<1>(*it1).begin(), std::get<1>(*it1).end(),
						[](const std::pair<int64_t, int64_t>& a, const std::pair<int64_t, int64_t>& b) {
							return a.first < b.first; });
					temp_ic.push_back(*it1);
				}
			}
		}
		if (logics[i] == "OR")
		{
			for (auto& j : ic)
			{
				a.insert(std::get<0>(j)[4]);
			}
			for (auto& j : icArray[i + 1])
			{
				b.insert(std::get<0>(j)[4]);
			}
			std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
			for (auto& each : c)
			{
				auto it1 = std::find_if(ic.begin(), ic.end(),
					[&each](const decltype(*ic.begin())& tmp) {
						return std::get<0>(tmp)[4] == each;
					});
				auto it2 = std::find_if(icArray[i + 1].begin(), icArray[i + 1].end(),
					[&each](const decltype(*icArray[i + 1].begin())& tmp) {
						return std::get<0>(tmp)[4] == each;
					});
				if (it1 != ic.end() && it2 != icArray[i + 1].end())
				{
					std::copy(std::get<1>(*it2).begin(), std::get<1>(*it2).end(),
						std::back_inserter(std::get<1>(*it1)));
					std::stable_sort(std::get<1>(*it1).begin(), std::get<1>(*it1).end(),
						[](const std::pair<int64_t, int64_t>& a, const std::pair<int64_t, int64_t>& b) {
							return a.first < b.first; });
					temp_ic.push_back(*it1);
				}
				if (it1 == ic.end() && it2 != icArray[i + 1].end())
				{
					temp_ic.push_back(*it2);
				}
				if (it1 != ic.end() && it2 == icArray[i + 1].end())
				{
					temp_ic.push_back(*it1);
				}
			}
		}
		if (logics[i] == "NOT")
		{
			for (auto& j : ic)
			{
				a.insert(std::get<0>(j)[4]);
			}
			for (auto& j : icArray[i + 1])
			{
				b.insert(std::get<0>(j)[4]);
			}
			std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
			for (auto& each : c)
			{
				auto it1 = std::find_if(ic.begin(), ic.end(),
					[&each](const decltype(*ic.begin())& tmp) {
						return std::get<0>(tmp)[4] == each;
					});
				if (it1 != ic.end())
				{
					temp_ic.push_back(*it1);
				}
			}
		}
		ic.clear();
		a.clear();
		b.clear();
		c.clear();
		std::copy(temp_ic.begin(), temp_ic.end(), std::back_inserter(ic));
		temp_ic.clear();
	}
}

void DataBase::setCase(bool value)
{
	caseSensitive = value;
}
