#pragma once

#include "sqlite3.h"
#include "tool.h"
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

/* 
                    DataType Offset
components:
start:record token start offset from text's first token
end:record record token end offset from text's first token
len:record length of specific token as unit of utf8 charactor
*/
typedef struct Offset Offset;
struct Offset
{
    int start;
    int end;
    int len;
};

/*
                    DataType Position
components:
offsets:An array of structure Offset which size is initialized 1024 and can be automatically
doubled if need more.The reason why space is dynamically allocated is the number if qualified
token is unknow. 
length:current position of offsets,starting at 0.
capability:The total volumn of offsets,starting at 1024.
method pushBack:like c++ std::vector push_back method which add one structure Offset at 
rear of offset.
method popBack:pop the last element of offsets.if empty,return exit code -1 and exits.
method isEmpty:to judge whether offsets is an empty array.
*/
typedef struct Position Position;
struct Position
{
    Offset* offsets;
    size_t length;
    size_t capability;
    void (*pushBack)(Position*, Offset);
    Offset (*popBack)(Position*);
    bool (*isEmpty)(Position*);
};

static bool posIsEmpty(Position* pos)
{
    return pos->length == 0 ? true : false;
}

static void posPushBack(Position* pos, Offset offset)
{
    if (pos->length >= pos->capability)
    {
        Offset* newOffset = (Offset*)malloc(sizeof(Offset) * 2 * pos->capability);
        size_t i;
        for (i = 0; i < pos->length; i++)
        {
            newOffset[i] = pos->offsets[i];
        }
        pos->capability *= 2;
        free(pos->offsets);
        pos->offsets = newOffset;
    }
    pos->offsets[pos->length++] = offset;
}

static Offset posPopBack(Position* pos)
{
    if (pos->isEmpty(pos))
        exit(-1);
    Offset result = pos->offsets[pos->length--];
    return result;
}


static Position initPosition()
{
    Position pos;
    pos.capability = 1024;
    pos.length = 0;
    pos.offsets = (Offset*)malloc(sizeof(Offset) * (pos.capability));
    pos.isEmpty = posIsEmpty;
    pos.pushBack = posPushBack;
    pos.popBack = posPopBack;
    return pos;
}

/*
                    DataType Results
component:
pos:structure of Position to record all positions of qualified tokens.
matchs:record number of appearance of qualified tokens is used by scoring.
rowId:rowid of chosen row
title:column 1,storing title of file.
extension:column2,storing file format,like .pdf,.docx eta.
cTime:column3,storing created time of file.
mTime:column4,storing last modified time of file.
path:column5,storing the absolute path to file.
size:column6,storing size of file in mb.
content:column7,self explained.
*/
typedef struct Results Results;
struct Results
{
    Position pos;
    int64_t rowId;
    char* title;
    char* extension;
    char* cTime;
    char* mTime;
    char* path;
    char* size;
    char* content;
};

typedef struct ResultsContainer ResultsContainer;
struct ResultsContainer
{
    Results* results;
    size_t len;
    size_t capability;
    size_t initSize;
    bool (*isEmpty)(ResultsContainer*);
    void (*pushBack)(ResultsContainer*, Results);
    void (*clear)(ResultsContainer*);
};

static bool resultsIsEmpty(ResultsContainer* rc)
{
    return rc->len == 0 ? true : false;
}

static void resultsPushBack(ResultsContainer* rc, Results res)
{
    if (rc->len >= rc->capability)
    {
        Results* newResults = (Results*)malloc(sizeof(Results) * 2 * rc->capability);
        size_t i;
        for (i = 0; i < rc->len; i++)
        {
            newResults[i] = rc->results[i];
        }
        rc->capability *= 2;
        free(rc->results);
        rc->results = newResults;
    }
    rc->results[rc->len++] = res;
}

static void resultsClear(ResultsContainer* rc)
{
    if (rc->results != NULL)
    {
        free(rc->results);
    }
    rc->results = (Results*)malloc(sizeof(Results) * rc->initSize);
    rc->len = 0;
    rc->capability = rc->initSize;
}

static ResultsContainer initResultsContainer(size_t initSize)
{
    ResultsContainer rc;
    rc.initSize = initSize;
    rc.results = (Results*)malloc(sizeof(Results) * rc.initSize);
    rc.len = 0;
    rc.capability = rc.initSize;
    rc.isEmpty = resultsIsEmpty;
    rc.pushBack = resultsPushBack;
    rc.clear = resultsClear;
    return rc;
}

ResultsContainer rContainer;
bool pivot;
int numPhrase;

static void fts5AuxFunction(const Fts5ExtensionApi* pApi,
                        Fts5Context* pFts,
                        sqlite3_context* pCtx,
                        int nVal,
                        sqlite3_value** apVal)
{
    int rc = SQLITE_OK;
    if (nVal != 0)
    {
        sqlite3_result_text(pCtx, "wrong number of parameters", -1, SQLITE_TRANSIENT);
    }
    sqlite3_int64 rowId;
    Position pos = initPosition();
    int nTokens;
    int phrase = pApi->xPhraseCount(pFts);
    int phraseId, colPrev, colNext, offsetPrev, offsetNext, initOffset, counter;
    colNext = 0; offsetNext = 0; offsetPrev = 0; colPrev = 0; initOffset = 0; counter = 0;
    char* text = NULL;
    int pn;
    rc = pApi->xInstCount(pFts, &nTokens);
    if (rc != SQLITE_OK)
        sqlite3_result_error_code(pCtx, rc);
    int i = 0;
    int j = 0;
    bool consecutive = false;
    while (i < nTokens)
    {
        if (!pivot)
            break;
        rc = pApi->xInst(pFts, i, &phraseId, &colPrev, &offsetPrev);
        if (phrase == 1)
        {
            initOffset = offsetPrev;
            Offset offset = { initOffset,initOffset + phrase,phrase };
            pos.pushBack(&pos, offset);
            counter = 0;
            consecutive = true;
            i += 1;
            continue;
        }
        if (rc != SQLITE_OK)
            sqlite3_result_error_code(pCtx, rc);
        if (i >= nTokens - 1)
            break;
        rc = pApi->xInst(pFts, ++i, &phraseId, &colNext, &offsetNext);
        if (rc != SQLITE_OK)
            sqlite3_result_error_code(pCtx, rc);
     
        if (colNext == colPrev && (offsetNext == offsetPrev + 1 || offsetNext == offsetPrev))
        {
            if (counter == 0)
            {
                initOffset = offsetPrev;
                j = i;
            }
            if (offsetNext == offsetPrev + 1)
                counter += 1;
        }
        else
        {
            if (counter != 0)
            {
                counter = 0;
                j = 0;
            }
        }
        if (counter+1 == phrase)
        {
            Offset offset = { initOffset,initOffset + phrase,phrase };
            pos.pushBack(&pos, offset);
            i = j;
            counter = 0;
            consecutive = true;
        }
    }

    if (consecutive)
    {
        Results res;
        char* temp = NULL;
        rc = pApi->xColumnText(pFts, 0, (const char**)&temp, &pn);
        res.title = (char*)calloc((int64_t)pn + 1, sizeof(char));
        strcpy_s(res.title, (int64_t)pn + 1, temp);
        rc = pApi->xColumnText(pFts, 1, (const char**)&temp, &pn);
        res.extension = (char*)calloc((int64_t)pn + 1, sizeof(char));
        strcpy_s(res.extension, (int64_t)pn + 1, temp);
        rc = pApi->xColumnText(pFts, 2, (const char**)&temp, &pn);
        res.cTime = (char*)calloc((int64_t)pn + 1, sizeof(char));
        strcpy_s(res.cTime, (int64_t)pn + 1, temp);
        rc = pApi->xColumnText(pFts, 3, (const char**)&temp, &pn);
        res.mTime = (char*)calloc((int64_t)pn + 1, sizeof(char));
        strcpy_s(res.mTime, (int64_t)pn + 1, temp);
        rc = pApi->xColumnText(pFts, 4, (const char**)&temp, &pn);
        res.path = (char*)calloc((int64_t)pn + 1, sizeof(char));
        strcpy_s(res.path, (int64_t)pn + 1, temp);
        rc = pApi->xColumnText(pFts, 5, (const char**)&temp, &pn);
        res.size = (char*)calloc((int64_t)pn + 1, sizeof(char));
        strcpy_s(res.size, (int64_t)pn + 1, temp);
        rc = pApi->xColumnText(pFts, 6, (const char**)&temp, &pn);
        res.content = (char*)calloc((int64_t)pn + 1, sizeof(char));
        strcpy_s(res.content, (int64_t)pn + 1, temp);
        rowId = pApi->xRowid(pFts);
        res.rowId = rowId;
        res.pos = pos;
        rContainer.pushBack(&rContainer, res);
    }
}