/* # Group 01

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#include <stdio.h>

#ifndef lexer
#define lexer

#define buff_size 64
#define hash_table_size 96

extern char buff[buff_size];
extern int lno;
extern int buildhash;
extern char* tr[];
extern char* ntr[];

typedef struct value numeral;
extern FILE * source;
struct value{
    int a; // may be needed to convert to long long 
    float b;
};

typedef struct tk token;
struct tk{
    numeral value; // to store num or rnum
    char lexeme[30]; // actual token in source code
    char token_t[30]; // 
    int lno;
    int tag;   // 0 : Valid Lexeme
               // 1 : Valid Int or floating point number
               // 2 : Valid Floating Point no. with E stored in lexeme
};

// Hashtable points to such a node.
typedef struct NODE hashnode;
struct NODE{
    int id;
    char value[30];
    int tag;
    hashnode * next;
};

#endif
