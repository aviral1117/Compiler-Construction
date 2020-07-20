/* # Group 01
    Aviral Sethi 	: 2016B3A70532P
    SANJEET MALHOTRA 	: 2016B4A70601P
    PRIYANKA VERMA 	: 2016B3A70492P
    ANIRUDH VIJAY 	: 2016B3A70525P  
*/
#ifndef cg
#define cg

#include "semantics.h"

void codeGen_driver(astNode *root,FILE *fp);
int sizeOfID(astNode *ID);
void processNode(astNode *stmts, FILE *fp, astNode* callingModuleID);

#endif
