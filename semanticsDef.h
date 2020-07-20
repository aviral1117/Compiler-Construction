/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#ifndef SEMANTICSDEF
#define SEMANTICSDEF

#include "scope.h"

typedef enum Type Type;
typedef enum Dynamic dynamic;

enum Type{
    integer, real, bool, int_array, real_array, bool_array, function
};

enum Dynamic{
    dynamicDeclaration, dynamicTypeCheck, dynamicBoundCheck, dynamicTypeCompute
};

typedef struct Queue queue;
typedef struct Entry entry;

struct Entry{
    symhashnode* symNode;
    int oldWhile;
    int* index;
    struct Entry* next;
};

struct Queue{
    entry* head;
    entry* tail;
    entry* cur;
    int size;
};

#endif