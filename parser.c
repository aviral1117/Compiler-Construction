/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#include<stdio.h>
#include<stdlib.h>
#include "parser.h"
#include<string.h>

int ntf = program; /* first non terminal*/
int ntl = var_id_num;  /* last non terminal*/
int tf = INTEGER; /* first terminal*/
int tl = DOLLAR; /* last terminal*/
int numOfNT = var_id_num+1; /* total number of non terminals*/
int numOfT = DOLLAR+1; /* total number of terminals*/
int grammarRules = 107; /* total number of grammar rules starting with 1*/
int startSymbol = program; /* Start Symbol*/
int bufferSize = 200; /* buffer size for reading grammar rules*/
int correct = 1; /*do not edit*/

int searchNonTerminal(char *data){
    int a = search(data,1); /*In lexer.c*/
    return a;
}

int searchTerminal(char *data){
    int a = search(data,1); /*In lexer.c*/
    return a;
}

grammar* addRule(char *buf,int lno, grammar* g){
    char data[30];
    int i,j;
    for(i=0,j=0;buf[i]!=':';i++){
        if(buf[i]==' '||buf[i]=='\t') continue;
        else if(buf[i]=='<'||buf[i]=='>') data[j] = '\0';
        else{
            data[j] = buf[i];
            data[j+1] = '\0';
            j++;
        }
    }
    g[lno].value = searchNonTerminal(data);
    g[lno].head = NULL;
    g[lno].tail = NULL;
    i++;
    data[0] = '\0';
    for(j=0;buf[i]!='\0';i++){
        if(buf[i]==' '||buf[i]=='\t'||buf[i]=='<'||buf[i]=='>') continue;
        /* To have this function read any other characters as part of non-terminals or terminals edit in the below line*/
        else if((buf[i]>='A'&&buf[i]<='Z')||(buf[i]>='a'&&buf[i]<='z')||buf[i]=='_'||(buf[i]>='0'&&buf[i]<='9')){
            data[j] = buf[i];
            data[j+1] = '\0';
            j++;
            if(buf[i+1]==' '||buf[i+1]=='\t'||buf[i+1]=='\0'){
                symbol *s = (symbol*)malloc(sizeof(symbol));
                s->value = searchTerminal(data);
                s->tag = terminal;
                s->next = NULL;
                s->prev = NULL;
                if(g[lno].head==NULL){
                    g[lno].head = s;
                    g[lno].tail = s;
                }
                else{
                    g[lno].tail->next = s;
                    s->prev = g[lno].tail;
                    g[lno].tail = s;
                }
                data[0] = '\0';
                j = 0;
            }
            else if(buf[i+1]=='>'){
                symbol *s = (symbol*)malloc(sizeof(symbol));
                s->value = searchNonTerminal(data);
                s->tag = nonTerminal;
                s->next = NULL;
                s->prev = NULL;
                if(g[lno].head==NULL){
                    g[lno].head = s;
                    g[lno].tail = s;
                }
                else{
                    g[lno].tail->next = s;
                    s->prev = g[lno].tail;
                    g[lno].tail = s;
                }
                data[0] = '\0';
                j = 0;
            }
        }
    }
    return g;
}

grammar* readGrammar(char *infile, grammar* g){
    FILE *fp;
    fp = fopen(infile,"r");
    char buf[bufferSize];
    int i = 0;
    while(fscanf(fp," %[^\n]",buf)!=EOF){
        g = addRule(buf,i,g);
        i++;
    }
    fclose(fp);
    return g;
}

/* if number of terminals exceed above 64 then bit masking fails, may need to change implementation*/
FirstAndFollow* ComputeFirst(grammar* g, FirstAndFollow* f, int s, int *visit){
    if(f[s].first) return f;
    visit[s] = 1;
    for(int i=0;i<grammarRules;i++){
        if(g[i].value!=s) continue;
        symbol *cur = g[i].head;
        unsigned long long mask = 0;
        while(cur!=NULL){
            if(cur->tag==terminal){
                f[s].first|=(1LL<<cur->value);
                mask = 0;
                break;
            }
            if(visit[cur->value]){
                cur = cur->next;
                continue;
            }
            f = ComputeFirst(g,f,cur->value,visit);
            mask = f[cur->value].first&~(1LL<<EPS);
            f[s].first|=mask;
            mask = f[cur->value].first&(1LL<<EPS);
            if(!mask) break;
            cur = cur->next;
        }
        if(mask) f[s].first|=(1LL<<EPS);
    }
    visit[s] = 0;
    return f;
}

/* if number of terminals exceed above 64 then bit masking fails, may need to change implementation*/
FirstAndFollow* ComputeFollow(grammar* g, FirstAndFollow* f, int s, int *visit){
    if(f[s].follow) return f;
    visit[s] = 1;
    if(s==startSymbol) f[s].follow|=(1LL<<DOLLAR);
    for(int i=0;i<grammarRules;i++){
        symbol *cur = g[i].head;
        while(cur){
            if(cur->value!=s||cur->tag!=nonTerminal) cur = cur->next;
            else if(cur->next==NULL){
                if(visit[g[i].value]){
                    f[s].follow|=f[g[i].value].follow;
                    cur = cur->next;
                    continue;
                }
                f = ComputeFollow(g,f,g[i].value,visit);
                f[s].follow|=f[g[i].value].follow;
                cur = cur->next;
            }
            else{
                symbol *tmpcur = cur->next;
                unsigned long long mask = 0;
                while(tmpcur!=NULL){
                    if(tmpcur->tag==terminal){
                       f[s].follow|=(1LL<<tmpcur->value); 
                       mask = 0;
                       break;
                    }
                    mask = f[tmpcur->value].first&~(1LL<<EPS);
                    f[s].follow|=mask;
                    mask = f[tmpcur->value].first&(1LL<<EPS);
                    if(!mask) break;
                    tmpcur = tmpcur->next;
                }
                if(mask){
                    if(visit[g[i].value]){
                        f[s].follow|=f[g[i].value].follow;
                    }
                    else{
                        f = ComputeFollow(g,f,g[i].value,visit);
                        f[s].follow|=f[g[i].value].follow;
                    }
                }
                cur = cur->next;
            }
        }
    }
    visit[s] = 0;
    return f;
}

/* if number of terminals exceed above 64 then bit masking fails, may need to change implementation*/
FirstAndFollow* ComputeFirstAndFollowSets (grammar* g, FirstAndFollow* f){
    int visit[ntl+1];
    for(int i=ntf;i<=ntl;i++) visit[i] = 0;
    for(int i=ntf;i<=ntl;i++){
        f[i].value = i;
        f[i].first = 0;
        f[i].follow = 0;
    }
    for(int i=ntf;i<=ntl;i++){
        f = ComputeFirst(g,f,i,visit);
    }
    for(int i=ntf;i<=ntl;i++) visit[i] = 0;
    for(int i=ntf;i<=ntl;i++){
        f = ComputeFollow(g,f,i,visit);
    }
    return f;
}

/* if number of terminals exceed above 64 then bit masking fails, may need to change implementation*/
int** fillTableFirst(unsigned long long set, int **t, int nt, int rule){
    unsigned long long mask = set&~(1LL<<EPS);
    for(int i=tf;i<=tl;i++){
        if(mask&(1LL<<i)){
            if(t[nt][i]!=-1){
                printf("LL(1) Incompatible:First set\n");
                printf("Non-Terminal:%s Terminal:%s Clash b/w %d & %d\n",ntr[nt],tr[i],t[nt][i],rule);
            }
            else t[nt][i] = rule;
        }
    }
    return t;
}

/* if number of terminals exceed above 64 then bit masking fails, may need to change implementation*/
int** fillTableFollow(unsigned long long set, int **t, int nt, int rule){
    for(int i=tf;i<=tl;i++){
        if(set&(1LL<<i)){
            if(t[nt][i]!=-1){
                printf("LL(1) Incompatible:Follow set\n");
                printf("Non-Terminal:%s Terminal:%s Clash b/w %d & %d\n",ntr[nt],tr[i],t[nt][i],rule);
            }
            else t[nt][i] = rule;
        }
    }
    return t;
}

/* if number of terminals exceed above 64 then bit masking fails, may need to change implementation*/
int** createParseTable(grammar* g, FirstAndFollow* f, int **t){
    for(int i=0;i<numOfNT;i++){
        for(int j=0;j<numOfT;j++){
            t[i][j] = -1;
        }
    }
    for(int i=0;i<grammarRules;i++){
        symbol *cur = g[i].head;
        int isEps = 0;
        while(cur){
            if(cur->tag==nonTerminal&&((f[cur->value].first)&(1LL<<EPS))){
                t = fillTableFirst(f[cur->value].first,t,g[i].value,i);
                isEps = 1;
                cur = cur->next;
            }
            else if(cur->tag==nonTerminal){
                t = fillTableFirst(f[cur->value].first,t,g[i].value,i);
                isEps = 0;
                break;
            }
            else if(cur->value!=EPS){
                t = fillTableFirst((1LL<<cur->value),t,g[i].value,i);
                isEps = 0;
                break;
            }
            else{
                isEps = 1;
                cur = cur->next;
            }
        }
        if(isEps) t = fillTableFollow(f[g[i].value].follow,t,g[i].value,i);
    }
    /*error recovery:this just add syn in parse table.*/
    for(int i=ntf;i<=ntl;i++){
        for(int j=tf;j<=tl;j++){
            if(t[i][j]==-1&&(f[i].follow&(1LL<<j))){
                t[i][j] = -2;
            }
        }
    }
    return t;
}

/*this function is just to help implementor for observing how correctly the grammar is read*/
void printGrammar(grammar* g){
    printf("Grammar:\n");
    for(int i=0;i<grammarRules;i++){
        printf("%d) <%s>->",i,ntr[g[i].value]);
        symbol *cur = g[i].head;
        while(cur){
            if(cur->tag==nonTerminal){
                printf("<%s> ",ntr[cur->value]);
            }
            else{
                printf("%s ",tr[cur->value]);
            }
            cur = cur->next;
        }
        printf("\n");
    }
    printf("\v");
}

/*this function is just to help implementor for observing how correctly the first and follow sets are made*/
void printFirstAndFollow(FirstAndFollow* f){
    printf("First & Follow Sets:\n");
    for(int i=ntf;i<=ntl;i++){
        printf("First of %s:\n",ntr[i]);
        for(int j=tf;j<=tl;j++){
            if(f[i].first&(1LL<<j)) printf("%s,",tr[j]);
        }
        printf("\n");
        printf("Follow of %s:\n",ntr[i]);
        for(int j=tf;j<=tl;j++){
            if(f[i].follow&(1LL<<j)) printf("%s,",tr[j]);
        }
        printf("\n\n");
    }
    printf("\v");
}

/*this function outputs in file named-"ParseTable.txt"-for observing the correctness of parse table*/
void printParseTable(int **t){
    FILE *fp;
    fp = fopen("ParseTable.txt","w");
    fprintf(fp,"Parse Table:\n");
    for(int i=0;i<numOfNT;i++){
        for(int j=0;j<numOfT;j++){
            fprintf(fp,"%d\t",t[i][j]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    printf("\v");
}

/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*helper function for parseInputSourceCode()*/
/*stack implmentation and stack initialized with a null symbol*/
stack* initStack(){
    stack* top = (stack*)malloc(sizeof(stack));
    top->ele = NULL;
    top->next = NULL;
    top->prev = NULL;
    return top;
}

stack* push(stack *top, node *nd){
    stack* tmp = (stack*)malloc(sizeof(stack));
    tmp->ele = nd;
    tmp->next = top;
    tmp->prev = NULL;
    if(top) 
        top->prev = tmp;
    top = tmp;
    return top;
}

/*function to create tree node*/
node* makeNode(int value, symType tag, token* rec, node* parent){
    node *nd = (node*)malloc(sizeof(node));
    nd->value = value;
    nd->tag = tag;
    nd->lex = rec;
    nd->down = NULL;
    nd->right = NULL;
    nd->left = NULL;
    nd->parent = parent;
    nd->ruleNo = -1;
    nd->children = (subTree*)malloc(sizeof(subTree));
    nd->children->start = NULL;
    nd->children->end = NULL;
    nd->children->numOfNodes = 0;
    nd->inh = NULL;
    return nd;
}

stack* pop(stack *top){
    stack* tmp = top;
    top = top->next;
    if(top) top->prev = NULL;
    free(tmp);
    return top;
}

/*for error handling, this hybrid of first and follow or one can say same as what sir taught in class in Error Recovery lecture*/
token* errorHandler(int nt, int tkn, int **t){
    token* rec = NULL;
    while(tkn!=DOLLAR&&t[nt][tkn]==-1){
        rec = getNextToken();
        //printf("Token printing: %s\n",tr[search(rec->token_t,1)]);
        tkn = search(rec->token_t,1);
    }
    if(tkn==DOLLAR&&t[nt][tkn]==-1){
        return NULL;
    }
    return rec;
}

/*just doing child then parent to free memory allocated to parse tree*/
node* freeMemory(node *root){ 
    /*To free memory*/
    while(root){
        node* tmp = root;
        root = root->right;
        if(root) root->left = NULL;
        freeMemory(tmp->down);
        free(tmp);
    }
    return root;
}

/*actual parser logic, don't change err initialization ever
first if(inside while)- handles case when stack is empty but still some tokens are there like some gibberish after int main(){}
else if runs if top of stack is a terminal and handle error like token on top and input token doesn't match
else runs when top of stack is non-terminal and handle error when no rule found for given token and non-terminal
In each error handling case only one error is reported per line*/
node* parseInputSourceCode(char *testFile, int **t, grammar *g){
    int err = -1000;
    /*lexerDependencies(testFile);*/
    stack* top = initStack();
    node* nd = makeNode(startSymbol,nonTerminal,NULL,NULL);
    top = push(top,nd);
    token* rec = getNextToken();
    //printf("Token printing: %s\n",tr[search(rec->token_t,1)]);
    node* root = nd;
    while(search(rec->token_t,1)!=DOLLAR||top->ele!=NULL){
        if(top->ele==NULL){
            while(search(rec->token_t,1)!=DOLLAR){
            	if(err!=rec->lno) printf("line: %d Syntax Error: Unexpected token_t: %s\n",rec->lno, tr[search(rec->token_t,1)]);  
                err = rec->lno;
                rec = getNextToken();
                //printf("Token printing: %s\n",tr[search(rec->token_t,1)]);
            }
        }
        /*else if(search(rec->token_t,1)==DOLLAR){
            printf("line %d Syntax Error: Expected a token_t\n",rec->lno);
            top = pop(top);
            err = 1;
        }*/
        else if(top->ele->tag==terminal){
            if(top->ele->value!=search(rec->token_t,1)){
                if(err!=rec->lno) printf("line: %d Syntax Error: Expected token_t: %s ---> Got: %s\n",rec->lno,tr[top->ele->value],tr[search(rec->token_t,1)]);
                err = rec->lno;
                while(top->ele!=NULL&&top->ele->tag==terminal&&top->ele->value!=search(rec->token_t,1)){
                	top = pop(top);
                }
            }
            else{
                top->ele->lex = rec;
                top = pop(top);
                rec = getNextToken();
                //printf("Token printing: %s\n",tr[search(rec->token_t,1)]);
            }
        }
        else{
            int tkn = search(rec->token_t,1);
            int nt = top->ele->value;
            int rule = t[nt][tkn];
            if(rule==-1 || rule==-2){
                if(err!=rec->lno) printf("line: %d Syntax Error: No matching rule for token: %s\n",rec->lno,tr[search(rec->token_t,1)]);
                err = rec->lno;
                if(rule==-2){
                    top = pop(top);
                    continue;
                }
                rec = errorHandler(nt,tkn,t);
                if(!rec) break;
                tkn = search(rec->token_t,1);
                rule = t[nt][tkn];
                if(rule==-2){
                    top = pop(top);
                    continue;
                }
            }
            symbol* cur = g[rule].head;
            node *lft=NULL,*rt=NULL;
            top->ele->ruleNo = rule+1;
            while(cur){
                /*if(cur->tag==nonTerminal) nd = makeNode(cur->value,cur->tag,NULL);
                else if(cur->value==EPS) nd = makeNode(cur->value,cur->tag,NULL);
                else nd = makeNode(cur->value,cur->tag,rec);*/
                nd = makeNode(cur->value,cur->tag,NULL,top->ele); /*tree node is created*/
                if(lft==NULL){
                    lft = nd;
                    rt = nd;
                }
                else{
                    nd->left = rt;
                    rt->right = nd;
                    rt = nd;
                }/*tree nodes were linked to each other*/
                cur = cur->next;
            }
            top->ele->down = lft; /*new rule entered in the tree*/
            top = pop(top); 
            if(g[rule].head->value==EPS&&g[rule].head->tag==terminal) continue; /*entering epsilon skipped*/
            node *tmpcur = rt;
            while(tmpcur){
                top = push(top,tmpcur);
                tmpcur = tmpcur->left;
            }/*rhs of rule pushed in stack in reverse order, this why iteration is done using left pointer rather than right*/
        }
    }
    /*to free stack if not empty in case of syntax error*/
    if(err!=-1000){
        correct = 0;
        while(top->ele!=NULL){
            top = pop(top);
        }
        //root = freeMemory(root);
    }
    return root;
}
/*--------------------------------Print Tree-----------------------------------------------------*/
/*for printing parse tree, 25 characters have been allocated per column but can be changed by replacing the needed with 25 
in for loop*/
FILE* printSpaces(FILE* fp, int data){
    for(int i=1;i<=20-data;i++){
        fprintf(fp," ");
    }
    return fp;
}

/*to calculate digits in an integer number NOT FLOAT*/
int lenCheck(int data){
    int base = 10;
    int cnt = 1;
    while(data/base!=0){
        base*=10;
        cnt++;
    }
    return cnt;
}

/*Utility function to print parse Tree, 'l' means onlt leftmost child to be examined and
'r' means all children except leftmost child to be examined. In both the cases same processing
is being done for current node. In processing current node, "if" deals with thoes nodes which don't
contain any information from lexer and else deals with those nodes which have info from lexer included 
in them in lex field. lex field points to structure returned by lexer*/
FILE* printTree(FILE* fp, node* root, char side, int nt, int tag){
    if(!root) return fp;
    else if(side=='l'){
        fp = printTree(fp,root->down,'l',root->value, root->tag);
        if(root->lex==NULL){
            fprintf(fp,"----");
            fp = printSpaces(fp,4);
            fprintf(fp,"----");
            fp = printSpaces(fp,4);
            fprintf(fp,"----");
            fp = printSpaces(fp,4);
            fprintf(fp,"----");
            fp = printSpaces(fp,4);
            if(root->tag==nonTerminal && root->value==startSymbol){
                fprintf(fp,"ROOT");
                fp = printSpaces(fp,4);
            }
            else{
                fprintf(fp,"%s",ntr[nt]);
                fp = printSpaces(fp,strlen(ntr[nt]));
            }
            if(root->down){
                fprintf(fp,"no");
                fp = printSpaces(fp,2);
            }
            else{
                fprintf(fp,"yes");
                fp = printSpaces(fp,3);
            }
            if(root->tag==nonTerminal){
                fprintf(fp,"%s\n",ntr[root->value]);
            }
            else{
                fprintf(fp,"%s\n",tr[root->value]);
            }        
        }
        else{
            token* rec = root->lex;
            fprintf(fp,"%s",rec->lexeme);
            fp = printSpaces(fp,strlen(rec->lexeme));
            fprintf(fp,"%d",rec->lno);
            fp = printSpaces(fp,lenCheck(rec->lno));
            fprintf(fp,"%s",rec->token_t);
            fp = printSpaces(fp,strlen(rec->token_t));
            if(rec->tag==1 || rec->tag==2){
                fprintf(fp,"%s",rec->lexeme);
                fp = printSpaces(fp,strlen(rec->lexeme));
            }
            else{
                fprintf(fp,"----");
                fp = printSpaces(fp,4);
            }
            fprintf(fp,"%s",ntr[nt]);
            fp = printSpaces(fp,strlen(ntr[nt]));
            fprintf(fp,"yes");
            fp = printSpaces(fp,3);
            fprintf(fp,"----\n");
        }
        if(root->down) fp = printTree(fp,root->down->right,'r',root->value, root->tag); 
    }
    else{
        node* cur = root;
        while(cur){
            fp = printTree(fp, cur->down, 'l', cur->value, cur->tag);
            if(cur->lex==NULL){
                fprintf(fp,"----");
                fp = printSpaces(fp,4);
                fprintf(fp,"----");
                fp = printSpaces(fp,4);
                fprintf(fp,"----");
                fp = printSpaces(fp,4);
                fprintf(fp,"----");
                fp = printSpaces(fp,4);
                if(cur->tag==nonTerminal && cur->value==startSymbol){
                    fprintf(fp,"ROOT");
                    fp = printSpaces(fp,4);
                }
                else{
                    fprintf(fp,"%s",ntr[nt]);
                    fp = printSpaces(fp,strlen(ntr[nt]));
                }
                if(cur->down){
                    fprintf(fp,"no");
                    fp = printSpaces(fp,2);
                }
                else{
                    fprintf(fp,"yes");
                    fp = printSpaces(fp,3);
                }
                if(cur->tag==nonTerminal){
                    fprintf(fp,"%s\n",ntr[cur->value]);
                } 
                else{
                    fprintf(fp,"%s\n",tr[cur->value]);
                }
            }
            else{
                token* rec = cur->lex;
                fprintf(fp,"%s",rec->lexeme);
                fp = printSpaces(fp,strlen(rec->lexeme));
                fprintf(fp,"%d",rec->lno);
                fp = printSpaces(fp,lenCheck(rec->lno));
                fprintf(fp,"%s",rec->token_t);
                fp = printSpaces(fp,strlen(rec->token_t));
                if(rec->tag==1 || rec->tag==2){
                    fprintf(fp,"%s",rec->lexeme);
                    fp = printSpaces(fp,strlen(rec->lexeme));
                }
                else{
                    fprintf(fp,"----");
                    fp = printSpaces(fp,4);
                }
                fprintf(fp,"%s",ntr[nt]);
                fp = printSpaces(fp,strlen(ntr[nt]));
                fprintf(fp,"yes");
                fp = printSpaces(fp,3);
                fprintf(fp,"----\n");
            }
            if(cur->down) fp = printTree(fp,cur->down->right,'r',cur->value, cur->tag);
            cur = cur->right;
        }
    }
    return fp;
}

/*for printing parse tree and it calls printTree() after just entering column names in outfile*/
void printParseTree(node *root/*, char *outfile*/){
    FILE *fp = stdout;
    //fp = fopen(outfile,"w");
    fprintf(fp,"lexeme of CurrentNode");
    fp = printSpaces(fp,21);
    fprintf(fp,"lineno");
    fp = printSpaces(fp,6);
    fprintf(fp,"tokenName");
    fp = printSpaces(fp,9);
    fprintf(fp,"valueIfNumber");
    fp = printSpaces(fp,13);
    fprintf(fp,"parentNodeSymbol");
    fp = printSpaces(fp,16);
    fprintf(fp,"isLeafNode(yes/no)");
    fp = printSpaces(fp,18);
    fprintf(fp,"NodeSymbol");
    fprintf(fp,"\n\n");

    fp = printTree(fp,root,'l',startSymbol,nonTerminal);
    //printf("%s",ntr[root->value]);
    //fclose(fp);
}
/*--------------------------------------------------------------*/
/*
Implementation of queue is provided, can be used if needed and it is also in parserDef but is commented out for now
queue initQueue(){
    queue qu;
    qu.size = 0;
    qu.head = NULL;
    qu.tail = NULL;
    return qu;
}

queueItem* makeItem(node *nd, int value){
    queueItem *item = (queueItem*)malloc(sizeof(queueItem));
    item->ele = nd;
    item->value = value;
    item->next = NULL;
    item->prev = NULL;
    return item;
}

queue pushQueue(queue qu, node *nd, int value){
    queueItem *item = makeItem(nd,value);
    if(qu.head==NULL){
        qu.head = item;
        qu.tail = item;
    }
    else{
        (qu.tail)->next = item;
        item->prev = qu.tail;
        qu.tail = item;
    }
    qu.size++;
    return qu;
}

queue popQueue(queue qu){
    queueItem *item = qu.head;
    qu.head = qu.head->next;
    if(qu.head){
        qu.head->prev = NULL;
    }
    else{
        qu.tail = NULL;
    }
    free(item);
    qu.size--;
    return qu;
}

void printParseTree(node *root, char *outfile){
    FILE *fp;
    fp = fopen(outfile,"w");
    queue qu = initQueue();
    qu = pushQueue(qu,root,0);
    while(qu.size){
        int curSize = qu.size;
        while(curSize){
            node *nd = qu.head->ele;
            if(nd->tag==terminal){
                fprintf(fp,"(<%s>,%s) ",ntr[qu.head->value],tr[nd->value]);
            }
            else{
                fprintf(fp,"(<%s>,<%s>) ",ntr[qu.head->value],ntr[nd->value]);
            }
            node* child = nd->down;
            while(child){
                qu = pushQueue(qu,child,nd->value);
                child = child->right;
            }
            qu = popQueue(qu);
            curSize--;
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}
*/
/*------------------------------------------------------------------------------------------------*/

node* parserDriver(int argc, char* argv[]){
    lexerDependencies(argv[1]);
    grammar* g = (grammar*)malloc(grammarRules*sizeof(grammar));
    g = readGrammar("grammar.txt",g);
    //printGrammar(g);
    FirstAndFollow* f = (FirstAndFollow*)malloc(numOfNT*sizeof(FirstAndFollow));
    f = ComputeFirstAndFollowSets(g,f);
    //printFirstAndFollow(f);
    int **t = (int**)malloc(numOfNT*sizeof(int*));
    for(int i=0;i<numOfNT;i++){
        t[i] = (int*)malloc(numOfT*sizeof(int));
    }
    t = createParseTable(g,f,t);
    //printParseTable(t);
    node* root = parseInputSourceCode(argv[1],t,g);
    if(correct){
        printf("\n...........Passed: Input source code is syntactically correct...........\n\n");
    }
    else{
        printf("\n...........Failed: Input source code is not syntactically correct...........\n\n"); 
    }
    return root;
}

/*int main(int argc, char* argv[]){
    parserDriver(argc,argv);
    return 0;
}*/

