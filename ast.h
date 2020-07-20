/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#ifndef ASTH
#define ASTH
#include "parser.h"
astNode* createAstNode(Label label, astNode* parent, astNode* next, astNode* prev, token* tkn);
subTree* createSubTree(astNode* start, astNode* end, int numOfNodes);
subTree* populateSubTree(subTree* sub,astNode* nd);
astNode* insertAtBegin(astNode* nd,astNode* list);
astNode* constructAst(node* pnode);
void removeAst(astNode* root);
void computeParseTreeSize(node* pnode, int *count);
void computeAstSize(astNode* root, int *count);
void printAstTree(astNode* root);

/*
-1: variable identifier
-2: function identifier
-3: NUM
-4: RNUM
-5: True
-6: False
*/

#endif