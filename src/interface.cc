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
			return Nan::ThrowTypeError(Nan::New("To use step,make sure the second\
			 parameter must be array of array").ToLocalChecked());
		}
	} else {
		int rc = self->execute(*Nan::Utf8String(info[0]));
		info.GetReturnValue().Set(rc);
	}
}

NAN_METHOD(DataBase::Commit) {
	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	if (!info[0]->IsUndefined()) {
		return Nan::ThrowSyntaxError(
			Nan::New("commit method has no parameter").ToLocalChecked()
		);
	}
	self->commit();
}

NAN_METHOD(DataBase::SortByScoreAscent) {
	All_Sort_Routing(DataBase)
	self->sortByScoreAscent();
}

NAN_METHOD(DataBase::SortByScoreDescent) {
	All_Sort_Routing(DataBase)
	self->sortByScoreDescent();
}

NAN_METHOD(DataBase::SortBySizeAscent) {
	All_Sort_Routing(DataBase)
	self->sortBySizeAscent();
}

NAN_METHOD(DataBase::SortBySizeDescent) {
	All_Sort_Routing(DataBase)
	self->sortBySizeDescent();
}

NAN_METHOD(DataBase::SortByCTimeAscent) {
	All_Sort_Routing(DataBase)
	self->sortByCTimeAscent();
}

NAN_METHOD(DataBase::SortByCTimeDescent) {
	All_Sort_Routing(DataBase)
	self->sortByCTimeDescent();
}

NAN_METHOD(DataBase::SortByMTimeAscent) {
	All_Sort_Routing(DataBase)
	self->sortByMTimeAscent();
}

NAN_METHOD(DataBase::SortByMTimeDescent) {
	All_Sort_Routing(DataBase)
	self->sortByMTimeDescent();
}

NAN_METHOD(DataBase::SortByTitleAscent) {
	All_Sort_Routing(DataBase)
	self->sortByTitleAscent();
}

NAN_METHOD(DataBase::SortByTitleDescent) {
	All_Sort_Routing(DataBase)
	self->sortByTitleDescent();
}

NAN_METHOD(DataBase::SortByFormatAscent) {
	All_Sort_Routing(DataBase)
	self->sortByFormatAscent();
}

NAN_METHOD(DataBase::SortByFormatDescent) {
	All_Sort_Routing(DataBase)
	self->sortByFormatDescent();
}

NAN_METHOD(DataBase::SortByPathAscent) {
	All_Sort_Routing(DataBase)
	self->sortByPathAscent();
}

NAN_METHOD(DataBase::SortByPathDescent) {
	All_Sort_Routing(DataBase)
	self->sortByPathDescent();
}

NAN_METHOD(DataBase::SetPivot) {
	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	if (!info[0]->IsBoolean()) {
		return Nan::ThrowSyntaxError(Nan::New("setPivot has no parameter").ToLocalChecked());
	}
	self->setPivot(info[0]->BooleanValue(info.GetIsolate()));
}

NAN_METHOD(DataBase::SetCase) {
	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	if (!info[0]->IsBoolean()) {
		return Nan::ThrowSyntaxError(Nan::New("setPivot has no parameter").ToLocalChecked());
	}
	self->setCase(info[0]->BooleanValue(info.GetIsolate()));
}

NAN_METHOD(DataBase::SetTop) {
	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
	if (!info[0]->IsInt32() || !info[0]->IsUint32() || !info[0]->IsBigInt()) {
		return Nan::ThrowTypeError(Nan::New("Top index number must be an integer").ToLocalChecked());
	}

	if (info[0]->NumberValue(context).FromJust() < 0) {
		return Nan::ThrowRangeError(Nan::New("Top range must greater than zero").ToLocalChecked());
	}

	self->setTop(static_cast<int>(info[0]->NumberValue(context).FromJust()));
}

NAN_METHOD(DataBase::Search) {
	if (!info[0]->IsArray() || !info[1]->IsArray()) {
		return Nan::ThrowTypeError(Nan::New("both queries and logics must be array of string(s)").ToLocalChecked());
	}
	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
	std::vector<std::string> queries,logics;
	v8::Local<v8::Object> queryObj = info[0]->ToObject(context).ToLocalChecked();
	v8::Local<v8::Array> queryProps = queryObj->GetPropertyNames(context).ToLocalChecked();
	for (uint32_t i = 0;i<queryProps->Length();++i) {
		queries.push_back(*Nan::Utf8String(queryProps->Get(context,i).ToLocalChecked()));
	}
	v8::Local<v8::Object> logicObj = info[1]->ToObject(context).ToLocalChecked();
	v8::Local<v8::Array> logicProps = logicObj->GetPropertyNames(context).ToLocalChecked();
	for (uint32_t i = 0;i<logicProps->Length();++i) {
		logics.push_back(*Nan::Utf8String(logicProps->Get(context,i).ToLocalChecked()));
	}
	self->queryArraySearch(std::move(queries),std::move(logics));
}

NAN_METHOD(DataBase::GetResult) {
	if (!info[0].IsEmpty()) {
		return Nan::ThrowTypeError(Nan::New("GetResult method has no parameter").ToLocalChecked());
	}

	DataBase* self = Nan::ObjectWrap::Unwrap<DataBase>(info.This());
	v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
	v8::Local<v8::Array> contents = Nan::New<v8::Array>();
	v8::Local<v8::Array> content = Nan::New<v8::Array>();
	v8::Local<v8::Array> locs = Nan::New<v8::Array>();
	v8::Local<v8::Array> loc = Nan::New<v8::Array>();
	v8::Local<v8::Object> points = Nan::New<v8::Object>();
	v8::Local<v8::Array> scores = Nan::New<v8::Array>();

	const InfoContainer& container = self->getInfo();
	uint32_t i=0;
	uint32_t j;
	for (const InfoTuple& tuple : container) {
		j = 0;
		for (const std::string& each : std::get<0>(tuple)) {
			content->Set(context,j++,Nan::New(each).ToLocalChecked());
		}
		contents->Set(context,i,content);
		content.Clear();
		j=0;
		for (const std::pair<int64_t,int64_t>& each : std::get<1>(tuple)) {
			points->Set(context,Nan::New("start").ToLocalChecked(),
			Nan::New(static_cast<int32_t>(each.first)));
			points->Set(context,Nan::New("end").ToLocalChecked(),
			Nan::New(static_cast<int32_t>(each.second)));
			loc->Set(context,j++,points);
			points.Clear();
		}
		locs->Set(context,i,loc);
		loc.Clear();
		scores->Set(context,i,Nan::New(static_cast<int32_t>(*(std::get<2>(tuple).end()-1))));
		i=i+1;
	}
	v8::Local<v8::Object> result = Nan::New<v8::Object>();
	result->Set(context,Nan::New("contents").ToLocalChecked(),
	contents);
	result->Set(context,Nan::New("locs").ToLocalChecked(),
	locs);
	result->Set(context,Nan::New("scores").ToLocalChecked(),
	scores);

	info.GetReturnValue().Set(result);
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

int DataBase::execute(const char* sql, StringContainer& sc)
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
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple&b) {
		return *(std::get<2>(a).end() - 1) > * (std::get<2>(b).end() - 1); });
}

void DataBase::sortByScoreAscent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return *(std::get<2>(a).end() - 1) < * (std::get<2>(b).end() - 1); });
}

void DataBase::sortBySizeDescent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::stoll(std::get<0>(a)[5]) > std::stoll(std::get<0>(b)[5]); });
}

void DataBase::sortBySizeAscent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::stoll(std::get<0>(a)[5]) < std::stoll(std::get<0>(b)[5]); });
}

void DataBase::sortByCTimeDescent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::stoll(std::get<0>(a)[2]) > std::stoll(std::get<0>(b)[2]); });
}

void DataBase::sortByCTimeAscent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::stoll(std::get<0>(a)[2]) < std::stoll(std::get<0>(b)[2]); });
}

void DataBase::sortByMTimeDescent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::stoll(std::get<0>(a)[3]) > std::stoll(std::get<0>(b)[3]); });
}

void DataBase::sortByMTimeAscent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::stoll(std::get<0>(a)[3]) < std::stoll(std::get<0>(b)[3]); });
}

void DataBase::sortByTitleDescent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::get<0>(a)[0] < std::get<0>(b)[0]; });
}

void DataBase::sortByTitleAscent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::get<0>(a)[0] > std::get<0>(b)[0]; });
}
void DataBase::sortByFormatDescent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::get<0>(a)[1] < std::get<0>(b)[1]; });
}

void DataBase::sortByFormatAscent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::get<0>(a)[1] > std::get<0>(b)[1]; });
}

void DataBase::sortByPathDescent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::get<0>(a)[4] < std::get<0>(b)[4]; });
}

void DataBase::sortByPathAscent()
{
	std::stable_sort(ic.begin(), ic.end(), [](const InfoTuple& a, const InfoTuple& b) {
		return std::get<0>(a)[4] > std::get<0>(b)[4]; });
}

void DataBase::setPivot(bool value)
{
	pivot = value;
}

const InfoContainer& DataBase::getInfo()
{
	sortByScoreDescent();
	return ic;
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
//							return std::get<0>(a) < std::get<0>(b);
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
//							return std::get<0>(a) < std::get<0>(b);
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

void DataBase::queryArraySearch(std::vector<std::string>&& queries, std::vector<std::string>&& logics)
{
	int rc;
	std::string sqls = "select func(test) from test where content match ";
	icArray.clear();
	ic.clear();
	for (std::string& query : queries)
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
							return std::get<0>(a) < std::get<0>(b); });
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
							return std::get<0>(a) < std::get<0>(b); });
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
