/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#include "scope.h"
#include<stdlib.h>

int errFlag=0;
int warFlag=0;

#define INT_SIZE 2
#define REAL_SIZE 4
#define BOOL_SIZE 1

int offset=0;
int fcount=0;
int ftag=0;
symhashnode* curFunc=NULL;

// function to reverse buffer[i..j]
char* reverse(char *buffer, int i, int j){
	while (i < j){
		//swap(&buffer[i++], &buffer[j--]);
        char t = buffer[i];
        buffer[i]=buffer[j];
        buffer[j]=t;
        i++;
        j--;
    }
	return buffer;
}

// Iterative function to implement itoa() function in C
char* itoa(int value, char* buffer, int base){
	if (base < 2 || base > 32)
		return buffer;
	int n = abs(value);
	int i = 0;
	while (n){
		int r = n % base;
		if (r >= 10) 
			buffer[i++] = 65 + (r - 10);
		else
			buffer[i++] = 48 + r;
		n = n / base;
	}
	if (i == 0){
		buffer[i++] = '0';
        i++;
        i--;
    }
	if (value < 0 && base == 10){
		buffer[i++] = '-';
    }
	buffer[i] = '\0';
	return reverse(buffer, 0, i - 1);
}

// For variable cannot be redeclared inside...............
//returns a hashtable of size 'size'. Table is an array of pointers.
int space=8;
table* createTable(int level,table* parent,int start,int end){
    table* a = (table*)malloc(sizeof(table));
    a->size=(level==0)?hash_table_size_scope:internal_hts;
    symhashnode** head = (symhashnode**)malloc(sizeof(symhashnode*)*a->size);
    for(int i=0;i<a->size;i++)
        head[i]=NULL;
    a->head=head;
    a->level=level;
    a->parent = parent;
    a->start=start;
    a->end=end;
    a->children=NULL;
    if(level==0){
        a->children=NULL;
    }
    else if(level==1){
        a->children=NULL;
    }
    else{
        if(!(parent->children)){
            parent->children = (tlist*)malloc(sizeof(tlist));
            parent->children->sib=NULL;
            parent->children->child = a;
        }
        else{
            tlist* ab=parent->children;
            while(ab->sib!=NULL){
                ab=ab->sib;
            }
            ab->sib = (tlist*)malloc(sizeof(tlist));
            ab = ab->sib;
            ab->sib=NULL;
            ab->child=a;
        }
    }
    return a;
}

int hash_value(char *value,int table_size){
    int hash_value=0;
    int length = strlen(value);
	for(int i=0;i<length;i++) {
		hash_value=hash_value*31 + value[i];
		hash_value=hash_value%table_size;
	}
	return hash_value;
}

void fill(symhashnode* array, symhashnode* ga,char* lex,int level){
    ga->drange_begin=NULL;
    ga->dcount=-1;
    ga->drange_end=NULL;
    ga->dtag=-1;
    ga->isDefined=0;
    ga->isFunc=0;
    ga->used=0;
    ga->type=0;
    strcpy(ga->value,lex);
    ga->level=level;
    ga->offset=-1;
    ga->lno=array->lno;
    ga->self=NULL;
    ga->isStatic=1;
    ga->isOutput=3;
    ga->a=-1;
    ga->b=-1;
    ga->input=NULL;
    ga->outal=NULL;
    ga->in=NULL;
    ga->out=NULL;
    ga->next=NULL;
    ga->semFor = -1;
    ga->semFunc = -1;
    ga->semWhile = -1;
    ga->index = NULL;
}
symhashnode* lookup(char *value,table* begin,int option){
    // Option=2 only current table lookup
    // Option=1 in both input and outal tables
    // Option=0 Lookup Upto Head without Input Table
    if(begin==NULL)
        return NULL;
    int key = hash_value(value,begin->size);
    symhashnode* temp = (begin->head)[key];
    if(temp==NULL){
        if(option==2){
            return NULL;
        }
        temp = lookup(value,begin->parent,option);
    }
    else{
        while(temp!=NULL){
            if(strcmp(value,temp->value)==0){
                return temp;
            }
            temp=temp->next;
        }
        if(option!=2)
            temp=lookup(value,begin->parent,option);
    }
    if(temp==NULL && option==1 && begin->level==0){
        temp = lookup(value,curFunc->input,0);
    }
    return temp;
}

symhashnode* insertinhash(char *value,table* stable,int a,int b,int isOutput,astNode* node,int size,int isIdArray){
    /*
        int level; char value[30]; int isFunc; // 1 for function entry else 0
        int offset; // 0 for function entry else as calculated in no. of bytes from level0 entry int lno; table *self; char type[10];
        int isStatic; int isOutput; int a; int b; // if array- end of range
        table *input; //formal parameters table *outal;
        symhashnode *next; // for open chaining // if multiple on same hash value
    */
    /*symhashnode* check= lookup(value,stable,2);
    if(check){
        return check;
    }*/
    fflush(stdout);
    if(strcmp(value,"$")==0){
        symhashnode* check= lookup(value,stable,0);
        if(check!=NULL){
            errFlag=1;
            printf("Driver Module Not Unique '%s'\n",value);
            return check;
        }
    }
    int flag=1;
    int key = hash_value(value,stable->size);
    symhashnode* temp = (symhashnode*)malloc(sizeof(symhashnode));
    strcpy(temp->value,value);
    temp->isFunc = (stable->level==0)?1:0;
    temp->offset = temp->isFunc?0:offset;
    temp->lno = (strcmp("$",value)==0)?-1:(node->tokenInfo)->lno;
    temp->level = stable->level;
    temp->drange_begin=NULL;
    temp->drange_end=NULL;
    temp->next=NULL;
    temp->isDefined=0;
    temp->used=0;
    temp->type=-1;
    temp->self = stable;
    temp->a=a;
    temp->b=b;
    temp->isStatic = 1;
    temp->in=NULL;
    temp->out=NULL;
    temp->dcount=-1;
    temp->dtag=-1;
    if(isIdArray){
        if(a==-1 || b==-1){
            if(isOutput==2)
                offset+=5 + space;
            else{
                offset+=1;
            }
            //temp->offset=-1;
            temp->isStatic=0;
            temp->dtag=ftag;
            ftag++;
            fcount++;
            flag=0;
        }
        else{
            if(isOutput==2){
                offset+=5+space;
            }
            else{
                offset+=(b-a+1)*size + 1;
            }
        }
    }
    else if(temp->level!=0){
        offset+=size;
    }
    temp->isOutput = isOutput;
    if((stable->head)[key] != NULL){
        symhashnode* t=(stable->head)[key];
        while(t->next!=NULL)
            t=t->next;
        t->next = temp;
    }
    else
        (stable->head)[key] = temp;

    if(temp->isFunc){
        temp->input = createTable(stable->level+1,stable,-1,-1);
        temp->input->parent = stable;
        temp->outal = createTable(stable->level+1,stable,-1,-1);
        temp->outal->parent = stable;
    }
    else{
        temp->input=NULL;
        temp->outal=NULL;
    }
    node->symTableEntry=temp;
    if(temp->isFunc)
        offset=0;
    
    temp->semFor = -1;
    temp->semFunc = -1;
    temp->semWhile = -1;
    temp->index = NULL;
    return temp;
}

void tfree(table* stable){
    int size=stable->level?internal_hts:hash_table_size_scope;
    /*for(int i=0;i<size;i++){
        free(stable->head[i]);
    }
    free(stable->head);
    free(stable);*/
}

int raiseError(char*value,int lno,int id){
    switch(id){
        case 1:
            errFlag=1;
            printf("Line %d: Variable '%s' Not Declared in this scope\n",lno,value);
            break;
        case 2:
            errFlag=1;
            printf("Line %d: Function '%s' Not Declared in this scope\n",lno,value);
            break;
        case 3:
            errFlag=1;
            printf("Line %d: Function '%s' Recursive Call Not Allowed\n",lno,value);
            break;
        case 4:
            errFlag=1;
            printf("Line %d: Function '%s' Dynamic array range variable not declared in this scope\n",lno,value);
            break;
        case 5:
            errFlag=1;
            printf("Line %d:  Multiple Definitions found. Overloading not supported for function name: '%s'\n",lno,value);
            break; 
        case 6:
            errFlag=1;
            printf("Line %d:  Multiple Declarations found for '%s'\n",lno,value);
            break; 
        case 7:
            errFlag=1;
            printf("Line %d:  Output variable redeclared in function body '%s'\n",lno,value);
            break; 
        case 8:
            errFlag=1;
            printf("Line %d:  For iterative variable redeclared in outermost loop body scope as '%s'\n",lno,value);
            break; 
    }
}

void checker(astNode* a){
    if(a==NULL){
        //printf("Yes It is Null\n");
        fflush(stdout);
    }
    else{
        //printf("No, with label %d\n",a->label);
        fflush(stdout);
    }
}
inout* handle_declare(astNode* begin,table* stable,int isOutput){
    // if declared afterwards---- check in output table.
    int flag=0;
    if(begin->label==46){
        flag=1;
    }
    inout* insertedlist = (inout*)malloc(sizeof(inout));
    inout* buildlist=NULL;
    astNode* ID = begin->down->start; // list if label of begin is 46
    astNode* datatype = ID->next;
    astNode* fdec = ID;
    int outflag=0;
    do{
        outflag=0;
        if(flag){
            fdec=ID; //free declaration
            ID=ID->down->start;
        }
        symhashnode* assist;
        int option=0;
        if(isOutput==1){ // 1 denotes output
            option=0; // 1 if we need to be sure that it is not in input as well.
        }
        else{
            option=2;
        }
        inout* scanner = curFunc->out;
        while(scanner!=NULL){
            if(!strcmp(scanner->para->value,ID->tokenInfo->lexeme)){
                raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,7);
                ID->donotproceed=3;
                if(!flag)
                    return NULL;
                else{
                    outflag=1;
                    break;
                }
            }
            scanner=scanner->next;
        }
        if(!outflag){
            int jump=0;
            assist = lookup(ID->tokenInfo->lexeme,stable,option);
            if(assist && !(assist->isOutput==2 && isOutput<=1) && assist->level!=0){ // as of now output name can be same as func name
                raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,6);
                ID->donotproceed=3;
                if(!flag)
                    return NULL;
                else{
                    jump=1;
                }
            }
            assist=NULL;
            if(!jump){
                if(datatype->label==3 || datatype->label==59){
                    assist = insertinhash(ID->tokenInfo->lexeme,stable,-1,-1,isOutput,ID,INT_SIZE,0);
                    assist->type=0;
                    if(!flag){
                        insertedlist->para=assist;
                        insertedlist->next=NULL;
                        return insertedlist;
                    }
                    else{
                        if(!buildlist){
                            insertedlist->para=assist;
                            insertedlist->next=NULL;
                            buildlist = insertedlist;
                        }
                        else{
                            buildlist->next=(inout*)malloc(sizeof(inout));
                            buildlist=buildlist->next;
                            buildlist->para = assist;
                            buildlist->next=NULL;
                        }
                    }
                }
                else if(datatype->label==4 || datatype->label==60){
                    assist = insertinhash(ID->tokenInfo->lexeme,stable,-1,-1,isOutput,ID,REAL_SIZE,0);
                    assist->type=1;
                    if(!flag){
                        insertedlist->para=assist;
                        insertedlist->next=NULL;
                        return insertedlist;
                    }
                    else{
                        if(!buildlist){
                            insertedlist->para=assist;
                            insertedlist->next=NULL;
                            buildlist = insertedlist;
                        }
                        else{
                            buildlist->next=(inout*)malloc(sizeof(inout));
                            buildlist=buildlist->next;
                            buildlist->para = assist;
                            buildlist->next=NULL;
                        }
                    }
                }
                else if(datatype->label==5 || datatype->label==61){
                    assist = insertinhash(ID->tokenInfo->lexeme,stable,-1,-1,isOutput,ID,BOOL_SIZE,0);
                    assist->type=2;
                    if(!flag){
                        insertedlist->para=assist;
                        insertedlist->next=NULL;
                        return insertedlist;
                    }
                    else{
                        if(!buildlist){
                            insertedlist->para=assist;
                            insertedlist->next=NULL;
                            buildlist = insertedlist;
                        }
                        else{
                            buildlist->next=(inout*)malloc(sizeof(inout));
                            buildlist=buildlist->next;
                            buildlist->para = assist;
                            buildlist->next=NULL;
                        }
                    }
                }
                else{
                    astNode* range2node = datatype->down->start;
                    astNode* addon = range2node->down->start;
                    astNode* a = addon->down->start;
                    astNode* b = addon->next->down->start;
                    astNode* type = range2node->next;
                    int size=0;
                    int local_type=0;
                    if(type->label==59){
                        size=INT_SIZE;
                        local_type=3;
                    }
                    else if(type->label==60){
                        size=REAL_SIZE;
                        local_type=4;
                    }
                    else if(type->label==61){
                        size=BOOL_SIZE;
                        local_type=5;
                    }
                    if(a->label<-3 || b->label<-3){
                        // Not valid array range holders... Should Not be a case
                        if(!flag)
                            return NULL;
                    }
                    else{
                        if(a->label==-1){
                            symhashnode* a1;
                            if(isOutput!=2){
                                a1 = lookup(a->tokenInfo->lexeme,stable,1);
                                if(a1==NULL || a1->level==0){
                                    raiseError(a->tokenInfo->lexeme,a->tokenInfo->lno,4);
                                    symhashnode* dyn;
                                    if(b->label==-1){
                                        dyn = insertinhash(ID->tokenInfo->lexeme,stable,-1,-1,isOutput,ID,size,1);
                                        symhashnode* ghosta = (symhashnode*)malloc(sizeof(symhashnode));
                                        symhashnode* ghostb = (symhashnode*)malloc(sizeof(symhashnode));
                                        fill(dyn,ghosta,a->tokenInfo->lexeme,stable->level);
                                        symhashnode* b1;
                                        b1=lookup(b->tokenInfo->lexeme,stable,1);
                                        if(!b1){
                                            fill(dyn,ghostb,b->tokenInfo->lexeme,stable->level);
                                            dyn->drange_end = ghostb;
                                            b->symTableEntry=ghostb;
                                        }
                                        else
                                        {
                                            dyn->drange_end=b1;
                                            b->symTableEntry=b1;
                                        }
                                        
                                        dyn->drange_begin = ghosta;
                                        a->symTableEntry=ghosta;
                                        
                                    }
                                    else{
                                        int end = (b->tokenInfo->value).a;
                                        dyn = insertinhash(ID->tokenInfo->lexeme,stable,-1,end,isOutput,ID,size,1);
                                        symhashnode* ghosta = (symhashnode*)malloc(sizeof(symhashnode));
                                        fill(dyn,ghosta,a->tokenInfo->lexeme,stable->level);
                                        dyn->drange_begin = ghosta;
                                        a->symTableEntry=ghosta;
                                        b->symTableEntry=NULL;
                                    }
                                    dyn->type=local_type;
                                    if(!flag){
                                        insertedlist->para=dyn;
                                        insertedlist->next=NULL;
                                        return insertedlist;
                                    }
                                    else{
                                        if(!buildlist){
                                            insertedlist->para=dyn;
                                            insertedlist->next=NULL;
                                            buildlist = insertedlist;
                                        }
                                        else{
                                            buildlist->next=(inout*)malloc(sizeof(inout));
                                            buildlist=buildlist->next;
                                            buildlist->para = dyn;
                                            buildlist->next=NULL;
                                        }
                                    }
                                }
                                else{
                                    a->symTableEntry = a1;
                                }
                            }
                            if(b->label==-1){
                                symhashnode* b1;
                                b1=lookup(b->tokenInfo->lexeme,stable,1);
                                if((b1==NULL || b1->level==0) && isOutput!=2){
                                    raiseError(b->tokenInfo->lexeme,b->tokenInfo->lno,4);
                                    if(!flag){
                                        return NULL;
                                    }
                                }
                                else{
                                    symhashnode* dyn;
                                    dyn = insertinhash(ID->tokenInfo->lexeme,stable,-1,-1,isOutput,ID,size,1);
                                    dyn->type = local_type;
                                    if(isOutput==2){
                                        symhashnode* ghosta = (symhashnode*)malloc(sizeof(symhashnode));
                                        symhashnode* ghostb = (symhashnode*)malloc(sizeof(symhashnode));
                                        fill(dyn,ghosta,a->tokenInfo->lexeme,stable->level);
                                        fill(dyn,ghostb,b->tokenInfo->lexeme,stable->level);
                                        dyn->drange_begin = ghosta;
                                        dyn->drange_end = ghostb;
                                        a->symTableEntry=ghosta;
                                        b->symTableEntry=ghostb;
                                    }
                                    else{
                                        dyn->drange_begin = a1;
                                        dyn->drange_end = b1;
                                        b->symTableEntry = b1;
                                    }
                                    if(!flag){
                                        insertedlist->para=dyn;
                                        insertedlist->next=NULL;
                                        return insertedlist;
                                    }
                                    else{
                                        if(!buildlist){
                                            insertedlist->para=dyn;
                                            insertedlist->next=NULL;
                                            buildlist = insertedlist;
                                        }
                                        else{
                                            buildlist->next=(inout*)malloc(sizeof(inout));
                                            buildlist=buildlist->next;
                                            buildlist->para = dyn;
                                            buildlist->next=NULL;
                                        }
                                    }
                                }
                            }
                            else{
                                symhashnode *dyn;
                                int end=(b->tokenInfo->value).a;
                                dyn = insertinhash(ID->tokenInfo->lexeme,stable,-1,end,isOutput,ID,size,1);
                                dyn->type=local_type;
                                if(isOutput==2){
                                    symhashnode* ghosta = (symhashnode*)malloc(sizeof(symhashnode));
                                    fill(dyn,ghosta,a->tokenInfo->lexeme,stable->level);
                                    dyn->drange_begin = ghosta;
                                    a->symTableEntry=ghosta;
                                }
                                else    
                                    dyn->drange_begin = a1;
                                if(!flag){
                                    insertedlist->para=dyn;
                                    insertedlist->next=NULL;
                                    return insertedlist;
                                }
                                else{
                                    if(!buildlist){
                                        insertedlist->para=dyn;
                                        insertedlist->next=NULL;
                                        buildlist = insertedlist;
                                    }
                                    else{
                                        buildlist->next=(inout*)malloc(sizeof(inout));
                                        buildlist=buildlist->next;
                                        buildlist->para = dyn;
                                        buildlist->next=NULL;
                                    }
                                }
                            }
                        }
                        else{
                            if(b->label==-1){
                                symhashnode* b1 = lookup(b->tokenInfo->lexeme,stable,1);
                                if((b1==NULL||b1->level==0) && isOutput!=2){
                                    raiseError(b->tokenInfo->lexeme,b->tokenInfo->lno,4);
                                    symhashnode* dyn;
                                    int start = (a->tokenInfo->value).a;
                                    dyn = insertinhash(ID->tokenInfo->lexeme,stable,start,-1,isOutput,ID,size,1);
                                    symhashnode* ghostb = (symhashnode*)malloc(sizeof(symhashnode));
                                    fill(dyn,ghostb,b->tokenInfo->lexeme,stable->level);
                                    dyn->drange_end = ghostb;
                                    b->symTableEntry=ghostb;
                                    dyn->type=local_type;
                                    if(!flag){
                                        insertedlist->para=dyn;
                                        insertedlist->next=NULL;
                                        return insertedlist;
                                    }
                                    else{
                                        if(!buildlist){
                                            insertedlist->para=dyn;
                                            insertedlist->next=NULL;
                                            buildlist = insertedlist;
                                        }
                                        else{
                                            buildlist->next=(inout*)malloc(sizeof(inout));
                                            buildlist=buildlist->next;
                                            buildlist->para = dyn;
                                            buildlist->next=NULL;
                                        }
                                    }
                                }
                                else{
                                    int begin=(a->tokenInfo->value).a;
                                    symhashnode* dyn;
                                    dyn = insertinhash(ID->tokenInfo->lexeme,stable,begin,-1,isOutput,ID,size,1);
                                    dyn->type=local_type;
                                    if(isOutput==2){
                                        symhashnode* ghostb = (symhashnode*)malloc(sizeof(symhashnode));
                                        fill(dyn,ghostb,b->tokenInfo->lexeme,stable->level);
                                        dyn->drange_end = ghostb;
                                        b->symTableEntry=ghostb;
                                    }
                                    else{
                                        b->symTableEntry = b1;
                                        dyn->drange_end=b1;
                                    }
                                    if(!flag){
                                        insertedlist->para=dyn;
                                        insertedlist->next=NULL;
                                        return insertedlist;
                                    }
                                    else{
                                        if(!buildlist){
                                            insertedlist->para=dyn;
                                            insertedlist->next=NULL;
                                            buildlist = insertedlist;
                                        }
                                        else{
                                            buildlist->next=(inout*)malloc(sizeof(inout));
                                            buildlist=buildlist->next;
                                            buildlist->para = dyn;
                                            buildlist->next=NULL;
                                        }
                                    }
                                }
                            }
                            else{
                                int begin=(a->tokenInfo->value).a;
                                int end= (b->tokenInfo->value).a;
                                assist = insertinhash(ID->tokenInfo->lexeme,stable,begin,end,isOutput,ID,size,1);
                                assist->type = local_type;
                                if(!flag){
                                    insertedlist->para=assist;
                                    insertedlist->next=NULL;
                                    return insertedlist;
                                }
                                else{
                                    if(!buildlist){
                                        insertedlist->para=assist;
                                        insertedlist->next=NULL;
                                        buildlist = insertedlist;
                                    }
                                    else{
                                        buildlist->next=(inout*)malloc(sizeof(inout));
                                        buildlist=buildlist->next;
                                        buildlist->para = assist;
                                        buildlist->next=NULL;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(flag)
            ID=fdec; //if not part of input/output list
        ID=ID->list;
        if(!flag)
            break;
    }while(ID->label!=-7);

    return insertedlist;
}

void handle_ioStmt_get(astNode* begin,table* stable,int isOutput){
    //printf("Entering ioStmt_get\n");
    astNode* ID = begin->down->start;
    symhashnode* check = lookup(ID->tokenInfo->lexeme,stable,1);
    if(check==NULL || check->level==0){
        raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,1);
    }
    else{
        ID->symTableEntry = check;
    }
    //printf("Exiting ioStmt_get\n");
}
void handle_ioStmt_print(astNode* begin,table* stable,int isOutput){
    //printf("Entering print\n");
    astNode* ID = begin->down->start;
    int pflag=0;
    if(ID->label==13 || ID->label == 63){
        if(ID->label==63)
            pflag=1;
        ID=ID->down->start;
        symhashnode* check = lookup(ID->tokenInfo->lexeme,stable,1);
        if(check==NULL || check->level==0){
            raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,1);
        }
        else{
            ID->symTableEntry=check;
        }
        if(!pflag){
            ID=ID->next;
            if(ID->label==17){
                ID=ID->down->start;
                symhashnode* check = lookup(ID->tokenInfo->lexeme,stable,1);
                if(check==NULL || check->level==0){
                    raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,1);
                }
                else{
                    ID->symTableEntry=check;
                }
            }
        }
    }
    //printf("Exiting print\n");
}

void handle_downnext(astNode* begin,table* stable){
    //printf("Entering DownNext\n");
    while(begin!=NULL){
        astNode* snext = begin;
        while(snext!=NULL){
            if(snext->label==-1){
                symhashnode* check = lookup(snext->tokenInfo->lexeme,stable,1);
                if(check==NULL || check->level==0){
                    raiseError(snext->tokenInfo->lexeme,snext->tokenInfo->lno,1);
                }
                else{
                    snext->symTableEntry=check;
                }
            }
            snext = snext->next;
            handle_downnext(snext,stable);
        }
        begin=begin->down->start;
    }
}

int fromwhile=0;
void handle_assignmentStmt(astNode* begin,table* stable,int isOutput){
    //printf("Entering Assignment\n");
    astNode* ID = begin->down->start;
    if(!fromwhile){
        symhashnode* check = lookup(ID->tokenInfo->lexeme,stable,1);
        if(check==NULL || check->level==0){
            raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,1);
        }
        else{
            ID->symTableEntry=check;
        }
        ID = ID->next;
        checker(ID);
        if(ID->label==15){
            ID=ID->down->start;
        }
        else{
            ID=ID->down->start;
            if(ID->label==17){
                // IS there a DOWN here;
                symhashnode* check = lookup((ID->down->start)->tokenInfo->lexeme,stable,1);
                if(check==NULL || check->level==0){
                    raiseError((ID->down->start)->tokenInfo->lexeme,(ID->down->start)->tokenInfo->lno,1);
                }
                else{
                    (ID->down->start)->symTableEntry=check;
                }
            }
            ID=ID->next;
        }
    }
    // This is that set
    fromwhile=0;
    while(ID!=NULL){
        astNode* specificExplorer = ID->down->start; // go down and next of this.
        handle_downnext(specificExplorer,stable);
        ID=ID->next;
    }
    //printf("Exiting Assignment\n");
}


void handle_moduleReuse(astNode* begin,table* stable,int isOutput){
    astNode* ID = begin->down->start;
    if(ID->label!=-7){
        astNode* temp1 = ID->down->start;
        while(temp1->label!=-7){
            symhashnode* check = lookup((temp1->down->start)->tokenInfo->lexeme,stable,1);
            if(check==NULL){
                raiseError((temp1->down->start)->tokenInfo->lexeme,ID->tokenInfo->lno,1);
            }
            else{
                (temp1->down->start)->symTableEntry=check;
            }
            temp1=temp1->list;
        }
    }
    ID=ID->next; // this is module name;
    checker(ID);
    symhashnode* check = lookup(ID->tokenInfo->lexeme,stable,0);
    if(check==NULL){
        //raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,2);
        return;
    }
    while(check->self->level!=0){
        check = lookup(ID->tokenInfo->lexeme,check->self->parent,0);
        if(check==NULL)
            break;
    }
    if(check==NULL){
        //raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,2); // error for missing declaration/definition earlier
        return;
    }
    else if(check==curFunc){
        ID->symTableEntry=check;
        ID->donotproceed=2;
        //raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,3); // error for recursion
        return;
    }
    else{
        check->used=1;
        ID->symTableEntry=check;
    }
    ID=ID->next;
    if(ID->label!=-7){
        astNode* temp1 = ID;
        while(temp1->label!=-7){
            symhashnode* check = lookup((temp1->down->start)->tokenInfo->lexeme,stable,1);
            if(check==NULL || check->level==0){
                raiseError((temp1->down->start)->tokenInfo->lexeme,(temp1->down->start)->tokenInfo->lno,1);
            }
            else{
                (temp1->down->start)->symTableEntry=check;
            }
            temp1=temp1->list;
            checker(temp1);
        }
    }
    //printf("Exiting Module Reuse");
    return;
}
void handle_switch(astNode* begin,table* stable,int isOutput){
    //printf("Entering Switch\n");
    int start = begin->startLno;
    int end = begin->endLno;
    astNode* ID = begin->down->start;
    symhashnode* check = lookup(ID->tokenInfo->lexeme,stable,1);
    if(check==NULL || check->level==0){
        raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,1);
    }
    else{
        if(!(check->type==0 || check->type==2)){
            ID->donotproceed=4;
            return;
        }
        ID->symTableEntry=check;
    }
    ID=ID->next;//case statement list
    astNode* temp=ID;
    int tuseflag=0;
    table* switch_new = createTable(stable->level+1,stable,start,end);
    while(temp->label!=-7){
        checker(temp);
        astNode* temp1 = temp->down->start->next;
        checker(temp1);
        while(temp1->label!=-7){
            switch(temp1->label){
                case 7:
                    handle_ioStmt_get(temp1,switch_new,0);
                    break;
                case 8:
                    handle_ioStmt_print(temp1,switch_new,0);
                    break;
                case 14:
                    handle_assignmentStmt(temp1,switch_new,0);
                    break;
                case 19:
                    handle_moduleReuse(temp1,switch_new,0);
                    break;
                case 46:
                    tuseflag=1;
                    handle_declare(temp1,switch_new,0);
                    break;
                case 47:
                    handle_switch(temp1,switch_new,0);
                    break;
                case 53:
                    handle_for_stmt(temp1,switch_new,0);
                    break;
                case 54:
                    handle_while(temp1,switch_new,0);
                    break;
            }
            temp1=temp1->list;
        }
        temp=temp->list;
    }
    ID=ID->next;//Default node
    checker(ID);
    astNode* temp1 = ID->down->start; // IS THIS DOWN REQUIRED
    checker(temp1);
    while(temp1!=NULL && temp1->label!=-7){
        switch(temp1->label){
            case 7:
                handle_ioStmt_get(temp1,switch_new,0);
                break;
            case 8:
                handle_ioStmt_print(temp1,switch_new,0);
                break;
            case 14:
                handle_assignmentStmt(temp1,switch_new,0);
                break;
            case 19:
                handle_moduleReuse(temp1,switch_new,0);
                break;
            case 46:
                tuseflag=1;
                handle_declare(temp1,switch_new,0);
                break;
            case 47:
                handle_switch(temp1,switch_new,0);
                break;
            case 53:
                handle_for_stmt(temp1,switch_new,0);
                break;
            case 54:
                handle_while(temp1,switch_new,0);
                break;
        }
        temp1=temp1->list;
    }
    if(!tuseflag){
        tfree(switch_new);
    }
    //printf("Exiting Switch\n");
}
void handle_for_stmt(astNode* begin,table* stable,int isOutput){
    int start = begin->startLno;
    int end = begin->endLno;
    astNode* ID = begin->down->start;
    symhashnode* check = lookup(ID->tokenInfo->lexeme,stable,1);
    if(check==NULL || check->level==0){
        raiseError(ID->tokenInfo->lexeme,ID->tokenInfo->lno,1);
    }
    else{
        ID->symTableEntry=check;
    }
    char * forcheck = ID->tokenInfo->lexeme;
    ID=ID->next;
    ID=ID->next;
    //astNode* temp1=ID->next->down->start;
    astNode* temp1=ID;
    int tuseflag=0;
    table* for_new = createTable(stable->level+1,stable,start,end);
    while(temp1->label!=-7){
        switch(temp1->label){
            case 7:
                handle_ioStmt_get(temp1,for_new,0);
                break;
            case 8:
                handle_ioStmt_print(temp1,for_new,0);
                break;
            case 14:
                handle_assignmentStmt(temp1,for_new,0);
                break;
            case 19:
                handle_moduleReuse(temp1,for_new,0);
                break;
            case 46:
                tuseflag=1;
                inout* inserted =  handle_declare(temp1,for_new,0);
                if(!inserted){
                    while(inserted){
                        if(!strcmp(inserted->para->value,forcheck)){
                            raiseError(forcheck,inserted->para->lno,8);
                            break;
                        }
                        inserted = inserted->next;
                    }
                }
                break;
            case 47:
                handle_switch(temp1,for_new,0);
                break;
            case 53:
                handle_for_stmt(temp1,for_new,0);
                break;
            case 54:
                handle_while(temp1,for_new,0);
                break;
        }
        temp1=temp1->list;
    }
    if(!tuseflag){
        tfree(for_new);
    }
}
void handle_while(astNode* begin,table* stable,int isOutput){
    //printf("Entering While\n");
    int start = begin->startLno;
    int end = begin->endLno;
    astNode* ID = begin;
    int tuseflag=0;
    table* while_new = createTable(stable->level+1,stable,start,end);
    fromwhile=1;
    handle_assignmentStmt(ID,stable,0);
    fromwhile=0;
    ID=ID->down->start;
    ID=ID->next;
    astNode* temp1=ID;
    while(temp1->label!=-7){
        switch(temp1->label){
            case 7:
                handle_ioStmt_get(temp1,while_new,0);
                break;
            case 8:
                handle_ioStmt_print(temp1,while_new,0);
                break;
            case 14:
                handle_assignmentStmt(temp1,while_new,0);
                break;
            case 19:
                handle_moduleReuse(temp1,while_new,0);
                break;
            case 46:
                tuseflag=1;
                handle_declare(temp1,while_new,0);
                break;
            case 47:
                handle_switch(temp1,while_new,0);
                break;
            case 53:
                handle_for_stmt(temp1,while_new,0);
                break;
            case 54:
                handle_while(temp1,while_new,0);
                break;
        }
        temp1=temp1->list;
    }
    if(!tuseflag){
        tfree(while_new);
    }
}

int populate_table(table* head,astNode* root){
    astNode* control;
    if(root==NULL)
        return -2;
    control = root->down->start; //module declarations
    if(control->label!=-7){
        astNode* local=control;
        while(local->label!=-7){
            astNode* temp1=(local->down)->start;
            char* value=(temp1->tokenInfo)->lexeme;
            symhashnode* assist = lookup(value,head,0);
            if(assist!=NULL){
                raiseError(value,(temp1->tokenInfo)->lno,1);
                local = local->list;
                continue;
            }
            symhashnode* dcl= insertinhash(value,head,-1,-1,0,temp1,0,0);
            dcl->isDefined = 0;
            temp1->symTableEntry = dcl;
            local=local->list;
        }
    }

    control=control->next; //now on module definition
    if(control->label!=-7){
        astNode* local=control;
        while(local->label!=-7){
            local->donotproceed=0;
            astNode* temp1=(local->down)->start; // ID of module
            int count = local->down->numOfNodes;
            char* value=(temp1->tokenInfo)->lexeme;
            symhashnode* exists = lookup(value,head,0);
            curFunc = exists;
            if(exists && exists->used==0 && !exists->isDefined){
                // ISSUE WARNING OF REDUNDANCY
                warFlag=1;//NOW AN ERROR
                errFlag=1;
                exists->isDefined=1;
                printf("Line %d: Redundant declaration of module '%s' found earlier\n",temp1->tokenInfo->lno,value);
                temp1->symTableEntry=exists;
            }
            else if(exists && exists->isDefined){
                local->donotproceed=1;
                local=local->list;
                //raiseError(value,temp1->tokenInfo->lno,5);
                continue;
            }
            else if(!exists){
                exists = insertinhash(value,head,-1,-1,0,temp1,0,0);
                exists->isDefined=1;
                curFunc = exists;
                temp1->symTableEntry=exists;
            }
            else{
                temp1->symTableEntry=exists;
                exists->isDefined=1;
            }
            int start= local->startLno;
            int end=local->endLno;
            if(exists){
                exists->input->start=start;
                exists->input->end=end;
                exists->outal->start=start;
                exists->outal->end=end;
            }
            temp1=temp1->next; //input list astNode now.
            astNode* input_explorer = temp1;
            fflush(stdout);
            if(temp1->label!=-7){
                inout* build_list;
                while(input_explorer->label!=-7){
                    inout* ilist = handle_declare(input_explorer,exists->input,2);
                    symhashnode* inserted = NULL;
                    if(ilist){
                        inserted = ilist->para;
                    }
                    else{
                        // ERROR OCCURED IN INSERTING;
                        input_explorer = input_explorer->list;
                        continue;
                    } 
                    if(curFunc->in==NULL){
                        curFunc->in = (inout*)malloc(sizeof(inout));
                        curFunc->in->next=NULL;
                        curFunc->in->para=inserted;
                        build_list=curFunc->in;
                    }
                    else{
                        build_list->next = (inout*)malloc(sizeof(inout));
                        build_list = build_list->next;
                        build_list->para = inserted;
                        build_list->next=NULL;
                    }
                    input_explorer = input_explorer->list;
                }
            }
            fflush(stdout);
            temp1=temp1->next; // now on output list
            astNode* output_explorer = temp1;
            if(temp1->label!=-7){
                inout* build_list;
                while(output_explorer->label!=-7){
                    inout* olist = handle_declare(output_explorer,exists->outal,1);
                    symhashnode* inserted = NULL;
                    if(olist){
                        inserted = olist->para;
                    }
                    else{
                        // ERROR OCCURED IN INSERTING;
                        output_explorer = output_explorer->list;
                        continue;
                    } 
                    if(curFunc->out==NULL){
                        curFunc->out = (inout*)malloc(sizeof(inout));
                        curFunc->out->next=NULL;
                        curFunc->out->para=inserted;
                        build_list=curFunc->out;
                    }
                    else{
                        build_list->next = (inout*)malloc(sizeof(inout));
                        build_list = build_list->next;
                        build_list->para = inserted;
                        build_list->next=NULL;
                    }
                    output_explorer = output_explorer->list;
                }
            }
            temp1=temp1->next; // now in func body
            while(temp1->label!=-7){
                switch(temp1->label){
                    case 7:
                        handle_ioStmt_get(temp1,exists->outal,0);
                        break;
                    case 8:
                        handle_ioStmt_print(temp1,exists->outal,0);
                        break;
                    case 14:
                        handle_assignmentStmt(temp1,exists->outal,0);
                        break;
                    case 19:
                        handle_moduleReuse(temp1,exists->outal,0);
                        break;
                    case 46:
                        handle_declare(temp1,exists->outal,0);
                        break;
                    case 47:
                        handle_switch(temp1,exists->outal,0);
                        break;
                    case 53:
                        handle_for_stmt(temp1,exists->outal,0);
                        break;
                    case 54:
                        handle_while(temp1,exists->outal,0);
                        break;
                }
                temp1=temp1->list;
            }
            // Complete here
            if(exists){
                exists->offset = offset;
                exists->dcount = fcount;
            }
            fcount=0;
            ftag=0;
            offset=0;
            local=local->list; //next module definition
        }
    }
    
    control=control->next; //now driver module
    astNode *temp1 = control->down->start;
    symhashnode* exists = insertinhash("$",head,-1,-1,0,control,0,0);
    control->symTableEntry = exists;
    curFunc=exists;
    if(exists==NULL){
        errFlag=1;
        printf("Cannot Insert Driver Module in Symbol Table\n");
    }
    while(temp1->label!=-7){
        switch(temp1->label){
            case 7:
                handle_ioStmt_get(temp1,exists->outal,0);
                break;
            case 8:
                handle_ioStmt_print(temp1,exists->outal,0);
                break;
            case 14:
                handle_assignmentStmt(temp1,exists->outal,0);
                break;
            case 19:
                handle_moduleReuse(temp1,exists->outal,0);
                break;
            case 46:
                handle_declare(temp1,exists->outal,0);
                break;
            case 47:
                handle_switch(temp1,exists->outal,0);
                break;
            case 53:
                handle_for_stmt(temp1,exists->outal,0);
                break;
            case 54:
                handle_while(temp1,exists->outal,0);
                break;
        }
        temp1=temp1->list;
    }
    if(exists){
        tfree(exists->input);
        exists->offset=offset;
        exists->dcount = fcount;
        exists->outal->start = control->startLno;
        exists->outal->end = control->endLno;
    }
    fcount=0;
    ftag=0;
    offset=0;

    control=control->next; // Definitions after driver module
    if(control->label!=-7){
        astNode* local=control;
        while(local->label!=-7){
            astNode* temp1=(local->down)->start; // ID of module
            int count = local->down->numOfNodes;
            char* value=(temp1->tokenInfo)->lexeme;
            symhashnode* exists = lookup(value,head,0);
            curFunc=exists;
            local->donotproceed=0;
            if(exists && exists->used==0 && !exists->isDefined){
                // ISSUE WARNING OF REDUNDANCY
                warFlag=1;//NOW AN ERROR
                errFlag=1;
                exists->isDefined=1;
                printf("Line %d: Redundant declaration of module '%s' found earlier.\n",temp1->tokenInfo->lno,value);
                temp1->symTableEntry=exists;
            }
            else if(exists && exists->isDefined){
                local->donotproceed=1;
                local=local->list;
                //raiseError(value,temp1->tokenInfo->lno,5); // Overloading Error
                continue;
            }
            else if(!exists){
                exists = insertinhash(value,head,-1,-1,0,temp1,0,0);
                exists->isDefined=1;
                curFunc = exists;
            }
            else{
                temp1->symTableEntry=exists;
                exists->isDefined=1;
            }
            int start= local->startLno;
            int end=local->endLno;
            if(exists){
                exists->input->start=start;
                exists->input->end=end;
                exists->outal->start=start;
                exists->outal->end=end;
            }
            temp1=temp1->next; //input list astNode now.
            astNode* input_explorer = temp1;
            if(temp1->label!=-7){
                inout* build_list;
                while(input_explorer->label!=-7){
                    inout* ilist = handle_declare(input_explorer,exists->input,2);
                    symhashnode* inserted = NULL;
                    if(ilist){
                        inserted = ilist->para;
                    }
                    else{
                        // ERROR OCCURED IN INSERTING;
                        input_explorer = input_explorer->list;
                        continue;
                    } 
                    if(curFunc->in==NULL){
                        curFunc->in = (inout*)malloc(sizeof(inout));
                        curFunc->in->next=NULL;
                        curFunc->in->para=inserted;
                        build_list=curFunc->in;
                    }
                    else{
                        build_list->next = (inout*)malloc(sizeof(inout));
                        build_list = build_list->next;
                        build_list->para = inserted;
                        build_list->next=NULL;
                    }
                    input_explorer = input_explorer->list;
                }
            }
            temp1=temp1->next; // now on output list
            astNode* output_explorer = temp1;
            if(temp1->label!=-7){
                inout* build_list;
                while(output_explorer->label!=-7){
                    inout* olist =  handle_declare(output_explorer,exists->outal,1);
                    symhashnode* inserted = NULL;
                    if(olist){
                        inserted = olist->para;
                    }
                    else{
                        // ERROR OCCURED IN INSERTING;
                        output_explorer = output_explorer->list;
                        continue;
                    } 
                    if(curFunc->out==NULL){
                        curFunc->out = (inout*)malloc(sizeof(inout));
                        curFunc->out->next=NULL;
                        curFunc->out->para=inserted;
                        build_list=curFunc->out;
                    }
                    else{
                        build_list->next = (inout*)malloc(sizeof(inout));
                        build_list = build_list->next;
                        build_list->para = inserted;
                        build_list->next=NULL;
                    }
                    output_explorer = output_explorer->list;
                }
            }
            temp1=temp1->next; // now in func body
            while(temp1->label!=-7){
                switch(temp1->label){
                    case 7:
                        handle_ioStmt_get(temp1,exists->outal,0);
                        break;
                    case 8:
                        handle_ioStmt_print(temp1,exists->outal,0);
                        break;
                    case 14:
                        handle_assignmentStmt(temp1,exists->outal,0);
                        break;
                    case 19:
                        handle_moduleReuse(temp1,exists->outal,0);
                        break;
                    case 46:
                        handle_declare(temp1,exists->outal,0);
                        break;
                    case 47:
                        handle_switch(temp1,exists->outal,0);
                        break;
                    case 53:
                        handle_for_stmt(temp1,exists->outal,0);
                        break;
                    case 54:
                        handle_while(temp1,exists->outal,0);
                        break;
                }
                temp1=temp1->list;
            }
            // Complete here
            if(exists){
                exists->offset = offset;
                exists->dcount = fcount;
            }
            offset=0;
            fcount = 0;
            ftag=0;
            local=local->list; // next module definition
        }
    }
    control=control->next;
    return errFlag;
}

char* get_type(int type){
    if(type==0 || type==3){
        return "integer";
    }
    else if(type==1||type==4)
        return "real";
    else{
        return "bool";
    }  
}
int parrays=0;
int poffset=0;
int tosub=0;
void printnode(symhashnode* temp,table* stable,char* value){
    int width=-1;
    int isArray=0;
    int isStatic=temp->isStatic;
    int level=temp->level;
    if(temp->isOutput!=0){   
        level=0;
    }
    if(temp->type>=3){
        isArray=1;
        if(temp->isOutput==2){
            width=5;
            //isStatic=0;
        }
        else{
            if(temp->isStatic && temp->type==3){
                width=(temp->b-temp->a+1)*2+1;
            }
            else if(temp->isStatic && temp->type==4){
                width=(temp->b-temp->a+1)*4+1;
            }
            else if(temp->isStatic && temp->type==5){
                width=(temp->b-temp->a+1)+1;
            }
            else{
                //isStatic=0;
                width=1;
            }
        }
    }
    else{
        if(temp->type==0){
            width=2;
        }
        else if(temp->type==1)
            width=4;
        else{
            width=1;
        }   
    }
    char offbuf[15];
    char linebuf[20];
    itoa(temp->offset-poffset-tosub,offbuf,10);
    char abuf[20],bbuf[20];
    abuf[0]=abuf[1]=bbuf[1]=bbuf[2]='-';
    abuf[2]=bbuf[0]=' ';
    abuf[3]=bbuf[3]='\0';
    if(isStatic && isArray){
        itoa(temp->a,abuf,10);
        itoa(temp->b,bbuf,10);
    }
    else if(isArray){
        if(temp->a==-1){
            strcpy(abuf,temp->drange_begin->value);
        }
        else{
            itoa(temp->a,abuf,10);
        }
        if(temp->b==-1){
            strcpy(bbuf,temp->drange_end->value);
        }
        else{
            itoa(temp->b,bbuf,10);
        }              
    }
    strcat(abuf,"-");
    strcat(abuf,bbuf);
    strcat(abuf,"]");
    itoa(stable->start,linebuf,10);
    strcat(linebuf,"-");
    itoa(stable->end,&linebuf[strlen(linebuf)],10);
    if(parrays){
        if(!(strcmp(value,"$"))){

        }
        if(isArray)
            printf("%-20s\t %-20s \t%-20s \t%-20s\t [%-20s \t%-20s \n",
            strcmp(value,"$")?value:"driver",linebuf,temp->value,(isStatic && isArray)?"static:array":(isArray?"dynamic:array":"---"),
            abuf,get_type(temp->type));
        return;
    }
    if(!(stable->parent && stable->parent->level==0 && temp->isOutput==1)){
        printf("%-20s\t %-20s \t%-20s \t%-10d\t %-10s \t%-20s \t[%-10s\t %-20s \t%-10s\t %-10d\n",
        temp->value,strcmp(value,"$")?value:"driver",linebuf,width,isArray?"yes":"no",(isStatic && isArray)?"static:array":(isArray?"dynamic:array":"---"),
        abuf,get_type(temp->type),offbuf,level);
    }
}

void print_table(table* stable,char* value){
    for(int i=0;i<stable->size;i++){
        symhashnode* temp=stable->head[i];
        while(temp!=NULL){
            printnode(temp,stable,value);
            temp=temp->next;
        }
    }
}
int printflag=0;
void printSymbolTable(table* stable,char* value){
    if(!stable)
        return;
    if(!printflag && !parrays){
        printflag=1;
        printf("%-20s\t %-20s \t%-20s \t%-10s\t %-10s \t%-20s \t%-10s\t %-20s \t%-10s\t %-10s\n",
        "variable_Name","scope(module_name)","scope(line_numbers)","width","isArray","Static/Dynamic","range_lexemes","type_of_element","offset","nesting_level");
    }
    if(stable->level>1){
        print_table(stable,value);
        tlist* children = stable->children;
        while(children!=NULL){
            printSymbolTable(children->child,value);
            children=children->sib;
        }
    }
    else if(stable->level==0){
        for(int i=0;i<stable->size;i++){
            symhashnode* temp=stable->head[i];
            while(temp!=NULL){
                //Print input table of temp.
                poffset=0;
                tosub=0;
                if(stable->level==0){
                    inout* inp = temp->in;
                    while(inp!=NULL){
                        symhashnode *para = inp->para;
                        printnode(para,temp->input,temp->value);
                        if(para->type>=3){
                            tosub+=space;
                        }
                        inp = inp->next;
                        if(!temp->out && !inp){
                            if(para->type==0){
                                poffset+=para->offset+2;
                            }
                            else if(para->type==1){
                                poffset+=para->offset+4;
                            }
                            else if(para->type==2){
                                poffset+=para->offset+1;
                            }
                            else
                            {
                                poffset+=para->offset+5+space;
                            }
                            
                        }
                    }
                    // Print output
                    inout* out = temp->out;
                    while(out!=NULL){
                        symhashnode *para = out->para;
                        //poffset+=para->offset;
                        para->isOutput=2;
                        printnode(para,temp->outal,temp->value);
                        para->isOutput=1;
                        out = out->next;
                        if(!out){
                            if(para->type==0){
                                poffset+=para->offset+2;
                            }
                            else if(para->type==1){
                                poffset+=para->offset+4;
                            }
                            else if(para->type==2){
                                poffset+=para->offset+1;
                            }
                        }
                    }
                }
                // Start Printing Local
                tosub=0;
                print_table(temp->outal,temp->value);
                tlist* children = temp->outal->children;
                while(children!=NULL){
                    printSymbolTable(children->child,temp->value);
                    children=children->sib;
                }
                poffset=0;
                tosub=0;
                temp=temp->next;
            }
        }
    }
}

void printActivation(table* stable){
    for(int i=0;i<stable->size;i++){
        symhashnode* temp=stable->head[i];
        while(temp!=NULL){
            int size=temp->offset;
            inout* inp = temp->in;
            tosub=0;
            while(inp!=NULL){
                symhashnode *para = inp->para;
                /*if(para->isStatic && para->type>=3){
                    if(para->type==3)
                        size -= (para->b-para->a)*2+5;
                    else if(para->type==4)
                        size -= (para->b-para->a)*4+5;
                    else if(para->type==5)
                        size -= (para->b-para->a)*1+5;
                }*/
                if(para->type>=3)
                    tosub+=space;
                inp = inp->next;
            }
            size-=tosub;
            printf("%-20s\t%-10d\n",strcmp(temp->value,"$")?temp->value:"driver",size);
            tosub=0;
            temp=temp->next;
        }
    }
}

table*root_scope;
table* scopeDriver(astNode* astRoot){
    table* head = createTable(0,NULL,-1,-1);
    root_scope=head;
    int correctness = populate_table(head,astRoot);
    return root_scope;
}
