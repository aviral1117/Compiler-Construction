/* # Group 01

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#ifndef lexer_func
#define lexer_func
#include "lexerDef.h"

int lexerDependencies(char *input);
token* getNextToken();
int hashvalue(char *value);
int search(char *value,int tag);
void removeComments(char *testcaseFile);
void printToken(token *tk);

extern hashnode* hashtable[hash_table_size]; // an array of pointers of type hashtable node

#endif
