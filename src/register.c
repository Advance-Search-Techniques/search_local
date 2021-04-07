#include "register.h"

int fts5SimpleCreate(void* pCtx, const char** azArg, int nArg, Fts5Tokenizer** ppOut)
{
	int rc = SQLITE_OK;
	SimpleTokenizer* tok = malloc(sizeof(SimpleTokenizer));
	*ppOut = (Fts5Tokenizer*)tok;
	return rc;
}

int fts5SimpleDelete(Fts5Tokenizer* tok)
{
	SimpleTokenizer* simTok = (SimpleTokenizer*)tok;
	free(simTok);
	simTok = NULL;
	return SQLITE_OK;
}

int fts5SimpleTokenize(Fts5Tokenizer* pTokenizer,
                        void* pCtx,
                        int flags,
                        const char* pText,
                        int nText,
                        int (*xToken)(
                            void* pCtx,
                            int tflags,
                            const char* pToken,
                            int nToken,
                            int iStart,
                            int iEnd))
{
    SimpleTokenizer* tok = (SimpleTokenizer*)pTokenizer;
    tok->simpleTokenizer = tokenize;
    return tok->simpleTokenizer(pCtx, flags, pText, nText, xToken);
}