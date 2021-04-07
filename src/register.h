#pragma once
#include "sqlite3.h"
#include "tokenizer.h"

int fts5SimpleCreate(void*, const char** azArg, int nArg, Fts5Tokenizer** ppOut);
int fts5SimpleDelete(Fts5Tokenizer*);
int fts5SimpleTokenize(Fts5Tokenizer*,
                        void* pCtx,
                        int flags,            /* Mask of FTS5_TOKENIZE_* flags */
                        const char* pText, int nText,
                        int (*xToken)(
                            void* pCtx,         /* Copy of 2nd argument to xTokenize() */
                            int tflags,         /* Mask of FTS5_TOKEN_* flags */
                            const char* pToken, /* Pointer to buffer containing token */
                            int nToken,         /* Size of token in bytes */
                            int iStart,         /* Byte offset of token within input text */
                            int iEnd            /* Byte offset of end of token within input text */
                            ));