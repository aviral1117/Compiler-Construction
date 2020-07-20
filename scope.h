/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#ifndef scope_func
#define scope_func

#include "ast.h"
#include <string.h>

//astNode* scopeDriver(node* root);
table* scopeDriver(astNode* astRoot);
void handle_for_stmt(astNode* begin,table* stable,int isOutput);
void handle_while(astNode* begin,table* stable,int isOutput);
void printSymbolTable(table* stable,char* value);
void printActivation(table* stable);
char* itoa(int value, char* buffer, int base);
char* reverse(char *buffer, int i, int j);
extern int errFlag;
extern int warFlag;
extern int parrays;

#endif