/* # Group 01

    Aviral Sethi 	: 2016B3A70532P
    SANJEET MALHOTRA 	: 2016B4A70601P
    PRIYANKA VERMA 	: 2016B3A70492P
    ANIRUDH VIJAY 	: 2016B3A70525P
    
*/
#include "lexer.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int lno=1;
char buff[buff_size];
FILE *source = NULL;
int traceback=0;
int buffer_control=-1;
int buildhash=0;
hashnode* hashtable[hash_table_size] = {NULL}; 

/* For Implementer's Help
- If size of tr and ntr is changed do appropriate changes in insertinHash function's 2 for loops at the end.
- buffer_size and hash table size are defined in lexerDef.h
*/

char* tr[] = {/*0*/"INTEGER","REAL","BOOLEAN","OF","ARRAY","START","END","DECLARE","MODULE","DRIVER",
/*10*/"PROGRAM","GET_VALUE","PRINT","USE","WITH","PARAMETERS","TRUE","FALSE","TAKES","INPUT",
/*20*/"RETURNS","AND","OR","FOR","IN","SWITCH","CASE","BREAK","DEFAULT","WHILE",
/*30*/"ID","NUM","RNUM","PLUS","MINUS","MUL","DIV","LT","LE","GE",
/*40*/"GT","NE","EQ","DEF","ENDDEF","DRIVERDEF","DRIVERENDDEF","COLON","RANGEOP","SEMICOL",
/*50*/"COMMA","ASSIGNOP","SQBO","SQBC","BO","BC","COMMENTMARK","EPS","DOLLAR" /*58*/ };

/**** Array of Non Terminals****/
char* ntr[] = {"program", "moduleDeclarations","otherModules","moduleDeclaration","module",
    "driverModule", "moduleDef","input_plist","ret","output_list","dataType","type",
    "input_plist1","output_list1","statements","statement","ioStmt", "var","whichId",
    "simpleStmt","assignmentStmt", "whichStmt","lvalueIDStmt","lvalueARRStmt","index",
    "moduleReuseStmt","optional","idList","idList1","expression","expression1",
    "logicalOp","expterm","expterm1","relationalOp","expfactor","expfactor1","op1","term",
    "term1","op2","factor","declareStmt","condionalStmt","caseStmt",
    "caseStmt1","value","default","iterativeStmt","range1","range2","aoBexpr","U","U1","unary_op",
    "boolConstt","var_id_num"};


FILE *getStream(FILE *fp){
    // load data equivalent to min(buff_size-traceback,EOF-fp_offset);
    for(int i=traceback;i<buff_size;i++)
        buff[i]=0;
    fread(buff+traceback,1,buff_size-traceback,fp);
    return fp;
}

void load(){
    // to check if loading of new data is required in buffer from source file
    if(buffer_control==buff_size || buffer_control==-1){
        source = getStream(source);
        buffer_control=0;
    }
}

// Only for demonstration
void removeComments(char *testcaseFile){
    int line=1;
    char * cleanFile = "cleanFile_afterCommentRemoval.txt";
    FILE *fin = fopen(testcaseFile,"r");
    if(fin==NULL){
        return;
    }
    else{
        FILE *fout = fopen(cleanFile,"w");
        if(fout==NULL)
           return;
        char c=0;
        int flag=0;
        c = fgetc(fin);
        if(c!=EOF)
            printf("%d ",line);
        while(c != EOF){
            if(c!='*' && c!=EOF){
                fputc(c,fout);
                if(c=='\n'){
                    line++;
                    printf("\n");
                    printf("%d ",line);
                }
                else
                    printf("%c",c);
            }
            else{
                c = fgetc(fin);
                if(c=='*'){
                    // comment starts
                    while(1){
                        c = fgetc(fin);
                        if(c==EOF){
                            flag=1;
                            break;
                        }
                        else if(c=='\n'){
                            line++;
                            printf("\n");
                            printf("%d ",line);
                            fputc(c,fout);
                        }
                        else if(c=='*'){
                            c = fgetc(fin);
                            if(c=='*'){
                                break;
                            }
                            else if(c==EOF){
                                flag=1;
                                printf("Warning: Comment Not Closed Properly Found only 1 * at the end\n");
                                break;
                            }
                        }
                    }
                }
                else{
                    printf("*");
                    fflush(stdout); // For the first * encountered
                    fputc('*',fout);
                    if(c!=EOF){
                        printf("%c",c);
                        fflush(stdout);
                        fputc(c,fout);
                    }
                    else{
                        flag=1;
                        break;
                    }
                }
            }
            if(flag)
                break;
            c = fgetc(fin);
        }
        fflush(fout);
        fflush(fin);
    }
}

int hashvalue(char *value){
    int hashvalue=0;
    int length = strlen(value);
	for(int i=0;i<length;i++) {
		hashvalue=hashvalue*31 + value[i];
		hashvalue=hashvalue%hash_table_size;
	}
	return hashvalue;
}

void insertinHash(char *value,int id,int tag){
    // tag is 0 for lexer dependent strings else 1 while inserting
    int a = hashvalue(value);
    if(hashtable[a]==NULL){
        //printf("%d",sizeof(hashnode));
        hashtable[a] = (hashnode*)malloc(sizeof(hashnode));
        hashtable[a]->id = id;
        hashtable[a]->tag= tag;
        hashtable[a]->next = NULL;
        strcpy(hashtable[a]->value,value);
    }
    else{
        hashnode* temp = hashtable[a];
        while(temp->next != NULL)
            temp = temp->next;
        temp->next = (hashnode*)malloc(sizeof(hashnode));
        (temp->next)->id = id;
        strcpy((temp->next)->value,value);
        (temp->next)->tag=tag;
        (temp->next)->next= NULL;
    }
}

int search(char *value,int tag){
    int k = hashvalue(value);
    hashnode * temp = hashtable[k];
    while(temp!=NULL){
        int a = strcmp(value,temp->value);
        if(a==0 && tag==temp->tag)
            return temp->id;
        temp = temp->next;
    }
    return -1;
}

void fillhashTable(){
    if(buildhash){
        buffer_control=-1;
        lno=1;
        return;
    }
    insertinHash("integer",0,0);
    insertinHash("real",1,0);
    insertinHash("boolean",2,0);
    insertinHash("of",3,0);
    insertinHash("array",4,0); 
    insertinHash("start",5,0);
    insertinHash("end",6,0);
    insertinHash("declare",7,0);
    insertinHash("module",8,0);
    insertinHash("driver",9,0);
    insertinHash("program",10,0);
    insertinHash("get_value",11,0);
    insertinHash("print",12,0);
    insertinHash("use",13,0);
    insertinHash("with",14,0); 
    insertinHash("parameters",15,0);
    insertinHash("true",16,0);
    insertinHash("false",17,0);
    insertinHash("takes",18,0);
    insertinHash("input",19,0);
    insertinHash("returns",20,0);
    insertinHash("AND",21,0);
    insertinHash("OR",22,0);
    insertinHash("for",23,0);
    insertinHash("in",24,0); 
    insertinHash("switch",25,0);
    insertinHash("case",26,0);
    insertinHash("break",27,0);
    insertinHash("default",28,0);
    insertinHash("while",29,0);
    /*    
    insertinHash("+",30);
    insertinHash("-",31);
    insertinHash("*",32);
    insertinHash("/",33);
    insertinHash("<",34); 
    insertinHash("<=",35);
    insertinHash(">=",36);
    insertinHash(">",37);
    insertinHash("==",38);
    insertinHash("!=",39);
    insertinHash("<<",40);
    insertinHash(">>",41);
    insertinHash("<<<",42);
    insertinHash(">>>",43);
    insertinHash(":",44); 
    insertinHash("..",45);
    insertinHash(";",46);
    insertinHash(",",47);
    insertinHash(":=",48);
    insertinHash("[",49);
    insertinHash("]",50);
    insertinHash("(",51);
    insertinHash(")",52);
    */
    //printf("Here");
    buildhash=1;
    for(int i=0;i<57;i++)
        insertinHash(ntr[i],i,1);
    for(int i=0;i<59;i++)
        insertinHash(tr[i],i,1);
}

int lexerDependencies(char *input){
    // source is a global file pointer
    if(source!=NULL){
        fclose(source);
    }
    source = fopen(input,"r");
    if(source==NULL){
        printf("Error: Missing Source File or Error in Opening File\n");
        fflush(stdout);
        return -1;
    }
    fillhashTable();
    return 0;
}

void printError(char lex[],int code,int len,int lno){
    /*
        lex : Is the problem string.
        Code: Error Code
        len : Length of string
        lno : Line of actual code
    */
    switch(code){
        case 0:
            printf("line: %d Lexical Error: Length of Identifier exceeded ---> Expected: Identifier of length <= 20, Got: '%s' of length %d\n", lno,lex,len);
            break;
        case 1:
            printf("line: %d Lexical Error: Invalid Floating Point Numeral '%s' ---> Missing valid character after '.'\n",lno,lex);
            break;
        case 2:
            printf("line: %d Lexical Error: Invalid Floating Point Numeral '%s' ---> Missing valid character after '%c', Expected a number between 0-9\n",lno,lex,lex[len-1]);
            break;
        case 10:
            if(strlen(lex)>=2 && lex[0]=='.' && lex[1]>='0' && lex[1]<='9'){
                printf("line: %d Lexical Error: Invalid Operator Encountered ---> '.' before %c, Add a '0' before '.' if a real number was desired\n",lno,lex[1]);
            }
            else{
		//printf("Here");
                printf("line: %d Lexical Error: Invalid Operator Encountered ---> '%c'\n",lno,lex[0]);
            }
            break;
        case 11:
            printf("line: %d Lexical Error: Invalid Character Encountered ---> '%s', No matching Tokenization Rule\n",lno,lex);
            break;
	    case 12:
	        printf("line: %d Lexical Error: Invalid Character Encountered after '!' ---> '%c', Expected: '='\n",lno,lex[0]);
            break;
    }
}

token* getNextToken(){
    // Gateway between lexer and parser
    // Returns a new token from source code or Dollar token if EOF
    // always call load() when doing buffer_control++ and reading a new character in switch case;
    token* tk = (token*)malloc(sizeof(token));
    tk->tag=-1;
    load();
    int start=0;
    char c=' ';
    int len=0; // Length of lexeme starts from 0 and not 1
    char lexeme[300] = {0};
    while(1){
        if(c==0)
            break;
        load();
        //for(int i=0;i<50;i++)
            //printf("%c",buff[i]);
        c = buff[buffer_control];
        if(c==0)
            break;
        switch(c){
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
                // The lower if is not currently of major use
                if(!start){
                    len = 0;
                    start=1;
                }
                else{
                    printf("Error : Identifier not completed earlier");
                }
                while((c>='a' && c<='z') || (c<='Z' && c>='A') || c=='_' || (c>='0' && c<='9')){
                    buffer_control++;
                    load();
                    lexeme[len]=c;
                    c = buff[buffer_control];
                    len++;
                }
                lexeme[len] = '\0';
                if(len>20)
                    printError(lexeme,0,len,lno);
                else{
                    //buffer_control--;
                    tk->tag=0; // for valid lexeme
                    strcpy(tk->lexeme,lexeme);
                    int a = search(lexeme,0);
                    if(a==-1)
                        strcpy(tk->token_t,"ID");
                    else
                        strcpy(tk->token_t,tr[a]);
                    tk->lno = lno;
                }
                lexeme[len] = 0;
                start=0;
                break;
            case '\n':
                lno++;
                buffer_control++;
                break;
            case ' ':
                //printf("In here");
                buffer_control++;
                break;
            case '\t':
                buffer_control++;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                while(c>='0' && c<='9'){
                    buffer_control++;
                    load();
                    lexeme[len]=c;
                    c = buff[buffer_control];
                    len++; // holds the actual length till now
                }
                if(c=='.'){
                    buffer_control++;
                    load();
                    lexeme[len]=c;
                    c = buff[buffer_control];
                    if((c>='0' && c<='9')){
                        len++;
                        while(c>='0' && c<='9'){
                            buffer_control++;
                            load();
                            lexeme[len]=c;
                            len++; // holds the actual length till now
                            c = buff[buffer_control];
                        }
                        if(c=='e' || c=='E'){
                            int validflag=0;
                            buffer_control++;
                            load();
                            lexeme[len] = c; // for e or E
                            len++;
                            c = buff[buffer_control];
                            if(c=='+' || c=='-' || (c>='0' && c<='9')){
                                validflag=1;
                            }
                            else{
                                lexeme[len]='\0';
                                printError(lexeme,2,len,lno);
                            }
                            int validflag2=len;
                            int plusflag=0;
                            while(c=='+' || c=='-' || (c>='0' && c<='9')){
                                if((c>='0' && c<='9')){
                                    plusflag=1;
                                }
                                buffer_control++;
                                load();
                                lexeme[len]=c;
                                len++;
                                c = buff[buffer_control];
                                if(c=='+' || c=='-'){
                                    break;
                                }
                            }
                            if(validflag2==len-1 && !plusflag){
                                validflag=0;
                                lexeme[len]='\0';
                                printError(lexeme,2,len,lno);
                            }
                            //buffer_control--;
                            if(validflag){
                                lexeme[len] = '\0';
                                tk->tag = 2;
                                strcpy(tk->lexeme,lexeme);
                                tk->lno = lno;
                                strcpy(tk->token_t,"RNUM");
                            }
                        }
                        else{
                            lexeme[len] = '\0';
                            tk->tag = 1; // For numeral
                            (tk->value).b = strtof(lexeme, (char **)NULL);
                            strcpy(tk->lexeme,lexeme);
                            tk->lno = lno;
                            strcpy(tk->token_t,"RNUM");
                        }
                    }
                    else if(c=='.'){
                        if(buffer_control==0){
                            traceback=1;
                            buff[0]='.';
                            fseek(source,-1*buff_size,SEEK_CUR);
                            buffer_control=-1;
                            load();
                        }
                        else 
                            buffer_control--;
                        
                        lexeme[len] = '\0';
                        tk->tag = 1; // For numeral
                        (tk->value).a = (int) strtol(lexeme, (char **)NULL, 10);
                        strcpy(tk->lexeme,lexeme);
                        tk->lno = lno;
                        strcpy(tk->token_t,"NUM");
                        traceback=0;
                    }
                    else{
                        len++;
                        lexeme[len] = '\0';
                        printError(lexeme,1,len,lno);
                    }
                }
                else{
                    //buffer_control--;
                    lexeme[len] = '\0';
                    tk->tag = 1; // For numeral
                    (tk->value).a = (int) strtol(lexeme, (char **)NULL, 10);
                    strcpy(tk->lexeme,lexeme);
                    tk->lno = lno;
                    strcpy(tk->token_t,"NUM");
                }
                lexeme[len] = 0;
                break;

                case '+':
                    tk->tag=0;
                    strcpy(tk->lexeme,"+");
                    strcpy(tk->token_t,"PLUS");
                    tk->lno = lno;
                    buffer_control++;
                    break;

                case '-':
                    tk->tag=0;
                    strcpy(tk->lexeme,"-");
                    strcpy(tk->token_t,"MINUS");
                    tk->lno = lno;
                    buffer_control++;
                    break;

                case '*':
                    buffer_control++;
                    load();
                    c = buff[buffer_control];
                    int cflag=0;
                    if(c=='*'){
                        while(1){
                            //printf("Debugging");
                            buffer_control++;
                            load();
                            c = buff[buffer_control];
                            if(c==EOF){
                                break;
                            }
                            if(c=='\n')
                                lno++;
                            else if(cflag && c=='*' || c==0){
                                buffer_control++;
                                break;
                            }
                            else if(c=='*')
                                cflag=1;
                            else if(cflag)
                                cflag=0;
                        }
                    }
                    else{
                        tk->tag=0;
                        strcpy(tk->lexeme,"*");
                        strcpy(tk->token_t,"MUL");
                        tk->lno = lno;
                    }
                    break;

                case '/':
                    tk->tag=0;
                    strcpy(tk->lexeme,"/");
                    strcpy(tk->token_t,"DIV");
                    tk->lno = lno;
                    buffer_control++;
                    break;

                case '<':
                    buffer_control++;
                    load();
                    c = buff[buffer_control];
                    if(c=='<'){
                        buffer_control++;
                        load();
                        c = buff[buffer_control];
                        if(c=='<'){
                            tk->tag=0;
                            strcpy(tk->lexeme,"<<<");
                            strcpy(tk->token_t,"DRIVERDEF");
                            tk->lno = lno;
                            buffer_control++; 
                        }
                        else{
                            tk->tag=0;
                            strcpy(tk->lexeme,"<<");
                            strcpy(tk->token_t,"DEF");
                            tk->lno = lno;
                        }
                    }
                    else if(c=='='){
                        tk->tag=0;
                        strcpy(tk->lexeme,"<=");
                        strcpy(tk->token_t,"LE");
                        tk->lno = lno;
                        buffer_control++;
                    }
                    else{
                        tk->tag=0;
                        strcpy(tk->lexeme,"<");
                        strcpy(tk->token_t,"LT");
                        tk->lno = lno;
                    }
                    //buffer_control--;
                    break;

                case '>':
                    buffer_control++;
                    load();
                    c = buff[buffer_control];
                    if(c=='>'){
                        buffer_control++;
                        load();
                        c = buff[buffer_control];
                        //printf("%c %d",c,c);
                        if(c=='>'){
                            tk->tag=0;
                            strcpy(tk->lexeme,">>>");
                            strcpy(tk->token_t,"DRIVERENDDEF");
                            tk->lno = lno;
                            buffer_control++; 
                        }
                        else{
                            tk->tag=0;
                            strcpy(tk->lexeme,">>");
                            strcpy(tk->token_t,"ENDDEF");
                            tk->lno = lno;
                        }
                    }
                    else if(c=='='){
                        tk->tag=0;
                        strcpy(tk->lexeme,">=");
                        strcpy(tk->token_t,"GE");
                        tk->lno = lno;
                        buffer_control++;
                    }
                    else{
                        tk->tag=0;
                        strcpy(tk->lexeme,">");
                        strcpy(tk->token_t,"GT");
                        tk->lno = lno;
                    }
                    //buffer_control--;
                    break;

                case '=':
                    buffer_control++;
                    load();
                    c = buff[buffer_control];
                    if(c=='='){
                        tk->tag=0;
                        strcpy(tk->lexeme,"==");
                        strcpy(tk->token_t,"EQ");
                        tk->lno = lno;
                        buffer_control++;
                    }
                    else{
                        char arr[2]={'=','\0'};
                        printError(arr,10,1,lno);
                    }
                    break;

                case ':':
                    buffer_control++;
                    load();
                    c = buff[buffer_control];
                    if(c=='='){
                        tk->tag=0;
                        strcpy(tk->lexeme,":=");
                        strcpy(tk->token_t,"ASSIGNOP");
                        tk->lno = lno;
                        buffer_control++;
                    }
                    else{
                        tk->tag=0;
                        strcpy(tk->lexeme,":");
                        strcpy(tk->token_t,"COLON");
                        tk->lno = lno;
                        
                    }
                    break;

                case ',':
                    tk->tag=0;
                    strcpy(tk->lexeme,",");
                    strcpy(tk->token_t,"COMMA");
                    tk->lno = lno;
                    buffer_control++;
                    break;
                case ';':
                    tk->tag=0;
                    strcpy(tk->lexeme,";");
                    strcpy(tk->token_t,"SEMICOL");
                    tk->lno = lno;
                    buffer_control++;
                    break;
                case '[':
                    tk->tag=0;
                    strcpy(tk->lexeme,"[");
                    strcpy(tk->token_t,"SQBO");
                    tk->lno = lno;
                    buffer_control++;
                    break;
                case ']':
                    tk->tag=0;
                    strcpy(tk->lexeme,"]");
                    strcpy(tk->token_t,"SQBC");
                    tk->lno = lno;
                    buffer_control++;
                    break;
                case '(':
                    tk->tag=0;
                    strcpy(tk->lexeme,"(");
                    strcpy(tk->token_t,"BO");
                    tk->lno = lno;
                    buffer_control++;
                    break;
                case ')':
                    tk->tag=0;
                    strcpy(tk->lexeme,")");
                    strcpy(tk->token_t,"BC");
                    tk->lno = lno;
                    buffer_control++;
                    break;

                case '!':
                    buffer_control++;
                    load();
                    c = buff[buffer_control];
                    if(c=='='){
                        tk->tag=0;
                        strcpy(tk->lexeme,"!=");
                        strcpy(tk->token_t,"NE");
                        tk->lno = lno;
                        buffer_control++;
                    }
                    else{
                        char arr[2]={c,'\0'};
                        printError(arr,12,1,lno);
                    }
                    break;

                case '.':
                    buffer_control++;
                    load();
                    c = buff[buffer_control];
                    if(c=='.'){
                        tk->tag=0;
                        strcpy(tk->lexeme,"..");
                        strcpy(tk->token_t,"RANGEOP");
                        tk->lno = lno;
                        buffer_control++;
                    }
                    else{
                        char arr[3]={'.',c,'\0'};     
                        printError(arr,10,1,lno);
                    }
                    break;

                default:
                    buffer_control++;
                    char arr[2]={c,'\0'};
                    printError(arr,11,1,lno);
        }
        if(tk->tag!=-1)
            break;
    }
    if(tk->tag==-1){
        tk->lno = lno;
        strcpy(tk->lexeme,"$");
        strcpy(tk->token_t,"DOLLAR");
    }
    return tk;
}

void printToken(token *tk){
    // To print tokens as required in Driver Option 2.
    printf("%-30d\t %-30s \t%-30s",tk->lno,tk->lexeme,tk->token_t);
}
