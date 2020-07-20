/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/

#ifndef PARSERDEF
#define PARSERDEF
#include "lexerDef.h"
typedef enum{
    program, moduleDeclarations,otherModules,moduleDeclaration,module,
    driverModule, moduleDef,input_plist,ret,output_list,dataType,type,
    input_plist1,output_list1,statements,statement,ioStmt,var,whichId,
    simpleStmt,assignmentStmt, whichStmt,lvalueIDStmt,lvalueARRStmt,index_,
    moduleReuseStmt,optional,idList,idList1,expression,expression1,
    logicalOp,expterm,expterm1,relationalOp,expfactor,expfactor1,op1,term,
    term1,op2,factor,declareStmt,condionalStmt,caseStmt,
    caseStmt1,value,default_,iterativeStmt,range1,range2,aoBexpr,U,U1,unary_op,
    boolConstt,var_id_num
}nonTerminals;

typedef enum{
    INTEGER,REAL,BOOLEAN,OF,ARRAY,START,END,DECLARE,MODULE,DRIVER,PROGRAM,
    GET_VALUE,PRINT,USE,WITH,PARAMETERS,TRUE,FALSE,TAKES,INPUT,RETURNS,AND,
    OR,FOR,IN,SWITCH,CASE,BREAK,DEFAULT,WHILE,ID,NUM,RNUM,PLUS,MINUS,MUL,
    DIV,LT,LE,GE,GT,NE,EQ,DEF,ENDDEF,DRIVERDEF,DRIVERENDDEF,COLON,RANGEOP,
    SEMICOL,COMMA,ASSIGNOP,SQBO,SQBC,BO,BC,COMMENTMARK,EPS,DOLLAR
}terminals;

typedef enum{
    terminal,nonTerminal
}symType;

typedef enum{
program_node,moduleDeclaration_node,module_node,dataType_int,dataType_real,dataType_bool,dataType_array,ioStmt_get,ioStmt_print,bool_true,
bool_false,num_var,rnum_var,array_var,assignmentStmt_node,lvalueIDStmt_node,lvalueARRStmt_node,index_id,index_num,moduleReuseStmt_node,
optional_node,input_list,output_list_,id_list,expression_node,logicalOp_and,logicalOp_or,expterm_node,expterm1_node,relationalOp_NT,
relationalOp_LT,relationalOp_LE,relationalOp_GT,relationalOp_GE,relationalOp_EQ,relationalOp_NE,expfactor_node,op1_plus,op1_minus,
term_node,op2_mul,op2_div,u_node,unary_plus,unary_minus,u1_node,declareStmt_node,switchStmt_node,value_num,value_true,value_false,
value_plus_num,value_minus_num,forStmt_node,whileStmt_node,range1_node,range2_node,default_node,driverModule_node,type_int,type_real,
type_bool,caseStmt_node,id_var
}Label;

typedef struct sym{
    int value;
    symType tag;
    struct sym *next;
    struct sym *prev;
}symbol;

typedef struct{
    int value;
    symbol *head;
    symbol *tail;
}grammar;

typedef struct{
    int value;
    unsigned long long first;
    unsigned long long follow;
}FirstAndFollow;

typedef struct astNode astNode;
typedef struct subTree subTree;

typedef struct NODE_sym symhashnode;
typedef struct table_ table;
typedef struct inout_ inout;
typedef struct tlist_ tlist;

struct subTree{
    astNode* start;
    astNode* end;
    int numOfNodes;
};

struct astNode{
    astNode* parent;
    astNode* next;
    astNode* prev;
    token* tokenInfo;
    subTree* down;
    astNode* list;
    int startLno;
    int endLno;
    int label;

    astNode* left;
    astNode* right;

    symhashnode* symTableEntry;
    int donotproceed; // 1 for overloading, 2 for ,3 for no declarations
    int type;
    int isDynamic;
    int dynamicAction;
};

#define hash_table_size_scope 10 // for number of function declarations
#define internal_hts 20

struct NODE_sym{
    int level;
    char value[30]; // name of function or driver
    int isFunc; // 1 for function entry else 0
    int offset; 
    int lno;
    table *self;
    int type; // 0 for int, 1 for real, 2 for bool, 3 for int_array, 4 for real_array, 5 for bool_array
    int isStatic;
    int isOutput; // 0 for normal, 1 for output, 2 for input
    int a; // if array- start of range
    int b; // if array- end of range
    int dtag; // if -1 then makes no sense else it is the index of the dynamic array being used
    table *input; //formal parameters
    table *outal;
    symhashnode* drange_begin; // array dynamic range begin
    symhashnode* drange_end;   // array dynamic range end

    inout * in; // function input list in order
    inout * out; // function output list in order
    int dcount; //if -1 then makes no sense else it is the number of darrays in func, basically it is valid only for func table entry
    int isDefined;
    int used;
    symhashnode *next; // for open chaining // if multiple on same hash value

    int semFor;
    int semWhile;
    int semFunc;
    int *index;
};

struct tlist_{
    tlist* sib;
    table* child;
};

struct inout_{
    symhashnode* para;
    inout* next;
};

struct table_{
    int size;
    int level;
    table *parent;
    int start;
    int end;
    symhashnode** head;
    tlist* children; //childlist children; // Just Next Level Children whose parent is this table.
};

typedef struct TreeNode{
    int value;
    symType tag;
    token* lex;
    struct TreeNode* down; // Next Level Left Most node
    struct TreeNode* right; // Right Siblings
    struct TreeNode* left; // Left Siblings
    struct TreeNode* parent;

    int ruleNo;
    subTree* children;
    astNode* inh;
}node;

typedef struct Element{
    node *ele;
    struct Element *next;
    struct Element *prev;
}stack;

/*typedef struct QueueItem{
    node *ele;
    int value;
    struct QueueItem *next;
    struct QueueItem *prev;
}queueItem;

typedef struct Queue{
    int size;
    queueItem *head;
    queueItem *tail;
}queue;*/



#endif