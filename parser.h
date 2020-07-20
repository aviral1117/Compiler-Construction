/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#ifndef PARSER
#define PARSER
#include "parserDef.h"
#include "lexer.h"

FirstAndFollow* ComputeFirstAndFollowSets (grammar* G, FirstAndFollow*  F);
int** createParseTable(grammar*, FirstAndFollow*, int**);
node* parseInputSourceCode(char *testcaseFile, int**, grammar* );
void printParseTree(node* root/*, char *outfile*/);
//int parserDriver(int argc, char* argv[]);
node* parserDriver(int argc, char* argv[]);
node* freeMemory(node *root);

extern int numOfNT;
extern int numOfT;
extern int grammarRules;
extern int ntf;
extern int ntl;
extern int tf;
extern int tl;
extern int startSymbol;
extern int bufferSize;
extern int correct;

#endif
