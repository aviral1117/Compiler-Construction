/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#ifndef SEMANTICS
#define SEMANTICS

#include "semanticsDef.h"
extern int semErr;

int null(astNode* root);
void passUpIsDynamic(astNode* root, astNode* child);
int checkArrayDeclaration(astNode* root);
int handleConstId(astNode* var);
void assignmentCheck(astNode* root, int stmtType);
void checkIoGet(astNode* root);
void checkIoPrint(astNode* root);
int checkExpression(astNode* root);
void checkAssignment(astNode* root);
void checkFunctionCall(astNode* root);
void checkDeclaration(astNode* root);
void checkSwitch(astNode* root);
void checkFor(astNode* root);
queue* createQueue();
void pushQueue(queue* q, astNode* root);
void popQueue(queue* q);
void whileUtil(astNode* root, queue* q, int mode);
void checkWhile(astNode* root);
void handleStatements(astNode* body);
void handleFunctions(astNode* funcDef);
void checkSemantics(astNode* root);
#endif
