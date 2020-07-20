/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#include "codeGen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define INT_SIZE 2
#define REAL_SIZE 4
#define BOOL_SIZE 1
#define DYNAMIC "_dynamic"
#define DYNAMIC_SIZE 1000
int switchCount = 0;
int whileCount = 0;
int forCount = 0;
int arrayCopying = 0;
int getArray = 0;
int num_relop = 0;
int printArray=0;

char output[100]; //example dword[vuffeeer+1212]l

char reg[10];//eax,ax etc
char bwd[10];//byte word dword qword

// uninitialised and garbage values are 0 
// 

//--------------Helper Functions-----------------

char *getbwd(int size){
    memset(bwd,0,10);
    if(size==1){
        strncpy(bwd,"byte",10);
        
    }else if(size==2){
        strncpy(bwd,"word",10);
    }else if(size==4){
        strncpy(bwd,"dword",10);
    }else{
        strncpy(bwd,"qword",10);
    }
    return bwd;

}
char * getReg(char *a,int size){
    memset(reg,0,10);
    if(size==1){
        strncpy(reg,a,10);
        strncat(reg,"l",10);
    }else if(size==2){
        strncpy(reg,a,10);
        strncat(reg,"x",10);
    }else if(size==4){
        strncpy(reg,"e",10);
        strncat(reg,a,10);
        strncat(reg,"x",10);
    }else if(size==8){
        strncpy(reg,"r",10);
        strncat(reg,a,10);
        strncat(reg,"x",10);
    }
    return reg;
}

int sizeOfID(astNode *ID){
    int size = 0;
    if (ID->symTableEntry->type == 0 || ID->symTableEntry->type == 3)
    {
        size = INT_SIZE;
    }
    else if (ID->symTableEntry->type == 1 || ID->symTableEntry->type == 4)
    {
        size = REAL_SIZE;
    }
    else
    {
        size = BOOL_SIZE;
    }
    return size;
}

char *outputIDwithREG(int isStatic, char reg[10], int size, char *moduleName)
{
    // 0 for int, 1 for real, 2 for bool, 3 for int_array, 4 for real_array, 5 for bool_array
    char buffer_name[100];
    strncpy(buffer_name, moduleName, 100);
    if (isStatic == 0)
    {
        strncat(buffer_name, "_dynamic", 100);
    }
    memset(output, 0, 100);
    char buff[100];
    if (size == 4)
    {
        strncpy(output, "dword[", 100);
        strncat(output, buffer_name, 100);
        strncat(output, "+", 100);
        strncat(output, reg, 10);
        strncat(output, "]", 10);
    }
    else if (size == 2)
    {
        strncpy(output, "word[", 100);
        strncat(output, buffer_name, 100);
        strncat(output, "+", 100);
        strncat(output, reg, 10);
        strncat(output, "]", 10);
    }
    else if (size == 1)
    {
        strncpy(output, "byte[", 100);
        strncat(output, buffer_name, 100);
        strncat(output, "+", 100);
        strncat(output, reg, 10);
        strncat(output, "]", 10);
    }else if(size==8){
        strncpy(output, "qword[", 100);
        strncat(output, buffer_name, 100);
        strncat(output, "+", 100);
        strncat(output, reg, 10);
        strncat(output, "]", 10);
    }
    return output;
}

char *outputID(int isStatic, int offset, int size, char *moduleName)
{
    // 0 for int, 1 for real, 2 for bool, 3 for int_array, 4 for real_array, 5 for bool_array
    char buffer_name[100];
    strncpy(buffer_name, moduleName, 100);
    if (isStatic == 0)
    {
        strncat(buffer_name, DYNAMIC, 100);
    }
    memset(output, 0, 100);
    char buff[100];
    if (size == 4)
    {
        strncpy(output, "dword[", 100);
        strncat(output, buffer_name, 100);
        strncat(output, "+", 100);
        strncat(output, itoa(offset, buff, 10), 10);
        strncat(output, "]", 10);
    }
    else if (size == 2)
    {
        strncpy(output, "word[", 100);
        strncat(output, buffer_name, 100);
        strncat(output, "+", 100);
        strncat(output, itoa(offset, buff, 10), 10);
        strncat(output, "]", 10);
    }
    else if (size == 1)
    {
        strncpy(output, "byte[", 100);
        strncat(output, buffer_name, 100);
        strncat(output, "+", 100);
        strncat(output, itoa(offset, buff, 10), 10);
        strncat(output, "]", 10);
    }else if(size==8){
        strncpy(output, "qword[", 100);
        strncat(output, buffer_name, 100);
        strncat(output, "+", 100);
        strncat(output, itoa(offset, buff, 10), 10);
        strncat(output, "]", 10); 
    }
    return output;
}

void copy(char from[30], char to[30], int from_offset, int to_offset, int size, FILE *fp)
{ //from and to are modulename
    fprintf(fp, "mov %s, %s\n",getReg("a",size), outputID(1, from_offset, size, from));
    fprintf(fp, "mov %s,%s\n", outputID(1, to_offset, size, to),getReg("a",size));
}

void arraySize(symhashnode *array,FILE *fp,char a[10],char callingModuleName[30],int moduleReuse){//will store array size in reg
    int isOutput=array->isOutput;
    int isStatic=array->isStatic;
    int offset=array->offset;
    if(isOutput==2&&moduleReuse==0){
        fprintf(fp,"mov %s,%s\n",getReg(a,INT_SIZE),outputID(1,offset+11,INT_SIZE,callingModuleName));
        fprintf(fp,"sub %s,%s\n",getReg(a,INT_SIZE),outputID(1,offset+9,INT_SIZE,callingModuleName));
        fprintf(fp,"add %s,1\n",getReg(a,INT_SIZE));
    }else{
        if(isStatic==1){
            int size=array->b-array->a+1;
            fprintf(fp,"mov %s,%d\n",getReg(a,INT_SIZE),size);
        }else{
            int dtag=array->dtag;
            fprintf(fp,"mov %s,word[%s_offset_%d+4]\n",getReg(a,INT_SIZE),callingModuleName,dtag);
            fprintf(fp,"sub %s,word[%s_offset_%d+2]\n",getReg(a,INT_SIZE),callingModuleName,dtag);
            fprintf(fp,"add %s,1\n",getReg(a,INT_SIZE));
        }
    }
}

void compSize(symhashnode *lhsArray,symhashnode *rhsArray,FILE *fp,char callingModuleName[30]){
    arraySize(lhsArray,fp,"a",callingModuleName,0);
    arraySize(rhsArray,fp,"b",callingModuleName,0);
    fprintf(fp,"cmp ax,bx\n");
    fprintf(fp,"jne size_mismatch_error\n");
}


void arrayBounds(symhashnode *array,FILE *fp,char lower[10],char upper[10],char callingModuleName[30],int moduleReuse){//reg1 s
    int isOutput=array->isOutput;
    int isStatic=array->isStatic;
    int offset=array->offset;
    if(isOutput==2&&moduleReuse==0){
        fprintf(fp,"mov %s,%s\n",getReg(upper,INT_SIZE),outputID(1,offset+11,INT_SIZE,callingModuleName));
        fprintf(fp,"mov %s,%s\n",getReg(lower,INT_SIZE),outputID(1,offset+9,INT_SIZE,callingModuleName));
    }else{
        if(isStatic==1){
            int a=array->a;
            int b=array->b;
            fprintf(fp,"mov %s,%d\n",getReg(lower,INT_SIZE),a);
            fprintf(fp,"mov %s,%d\n",getReg(upper,INT_SIZE),b);

        }else{
            int dtag=array->dtag;
            fprintf(fp,"mov %s,word[%s_offset_%d+4]\n",getReg(upper,INT_SIZE),callingModuleName,dtag);
            fprintf(fp,"mov %s,word[%s_offset_%d+2]\n",getReg(lower,INT_SIZE),callingModuleName,dtag);
        }
    }
}


void compBounds(symhashnode *lhsArray,symhashnode *rhsArray,FILE *fp,char callingModuleName[30]){
    arrayBounds(lhsArray,fp,"a","b",callingModuleName,0);
    arrayBounds(rhsArray,fp,"c","d",callingModuleName,0);
    fprintf(fp,"cmp ax,cx\n");
    fprintf(fp,"jne bound_mismatch_error\n");
    fprintf(fp,"cmp bx,dx\n");
    fprintf(fp,"jne bound_mismatch_error\n");
}

void arrayOffset(symhashnode *array,FILE *fp,char a[10],char callingModuleName[30]){
    int isOutput=array->isOutput;
    int isStatic=array->isStatic;
    int offset=array->offset;
    if(isOutput==2){
        fprintf(fp,"movsx %s,%s\n",getReg(a,8),outputID(1,offset+8,1,callingModuleName));
        fprintf(fp,"add %s,%s\n",getReg(a,8),outputID(1,offset,8,callingModuleName));
        
    }else{
        if(isStatic==1){
            int size=array->b-array->a+1;
            fprintf(fp,"mov %s,%d\n",getReg(a,8),offset);
            fprintf(fp,"add %s,%s\n",getReg(a,8),callingModuleName);
            
        }else{
            int tag=array->dtag;
            fprintf(fp,"movsx %s,word[%s_offset_%d]\n",getReg(a,8),callingModuleName,tag);
            fprintf(fp,"add %s,%s_dynamic\n",getReg(a,8),callingModuleName);
            
        }
    }

}

void copyArray(astNode *lhsArrayNode,astNode *rhsArrayNode,FILE *fp,char callingModuleName[30]){
    int size=sizeOfID(lhsArrayNode);

    symhashnode *lhsArray=lhsArrayNode->symTableEntry;
    symhashnode *rhsArray=rhsArrayNode->symTableEntry;
    //check for size match
    compSize(lhsArray,rhsArray,fp,callingModuleName);
    //check for bounds
    compBounds(lhsArray,rhsArray,fp,callingModuleName);
    arrayOffset(lhsArray,fp,"a",callingModuleName);
    arrayOffset(rhsArray,fp,"b",callingModuleName);
    arraySize(lhsArray,fp,"c",callingModuleName,0);
    fprintf(fp,"mov r8w,0\n");
    fprintf(fp,"copy_array_%d:\n", arrayCopying);
    fprintf(fp,"mov %s,%s[rbx]\n",getReg("d",size),getbwd(size)); 
    fprintf(fp,"mov %s[rax],%s\n",getbwd(size),getReg("d",size));   
    fprintf(fp, "add rax,%d\n", size);
    fprintf(fp, "add rbx,%d\n", size);
    fprintf(fp, "inc r8w\n");
    fprintf(fp, "cmp r8w,cx\n");
    fprintf(fp, "jne copy_array_%d\n", arrayCopying);

    arrayCopying++;
}
//copy array from input to function parameters

void user_input(FILE* fp)
{
	fprintf(fp,"user_input:\n\
push rax\n\
push rbx\n\
push rcx\n\
push rdx\n\
push rsi\n\
push rdi\n\
push r10\n\
push r9\n\
push r8\n\
\t\tmov\tedi, buffer\n\
\t\tmov\tesi, 0\t\n\
input:\t\n\
\t\tmov\teax, 03\t\n\
\t\tmov\tebx, 0\t\n\
\t\tmov\tecx, edi\n\
\t\tmov\tedx, 1\t\n\
\t\tint\t0x80\t\n\
\t\tcmp\tbyte[edi], 0\n\
\t\tje\t convert\n\
\t\tcmp\tbyte[edi], 10\n\
\t\tje\t convert\n\
\t\tinc\tedi\n\
\t\tinc\tesi\t\n\
\t\tjmp\tinput\n\
convert:\n\
\t\tmov\tedi, buffer\t\n\
\t\tmov\tecx, esi\t\n\
\t\tmov\tdword[integer], 0\n\
\t\tmov\tbyte[isnegative], 0\n\
.negative:\t\n \
\t\tcmp\tbyte[edi], '-'\n\
\t\tjne\titerate\n\
\t\tinc\tedi\n\
\t\tinc\tbyte[isnegative]\n\
iterate:\t  mov\tebx, 0\n\
\t\tmov\tbl, byte[edi]\t\n \
\t\tcmp\tbl, 10\t\n  \
\t\tje\t done\t\n\
\t\tcmp\tbl, '0'\n\
\t\tjb\t done\n\
\t\tcmp\tbl, '9'\n\
\t\tja\t done\t\n\
\t\tmov\tedx, 10\n\
\t\tmov\teax, dword[integer]\n\
\t\tmul\tedx\t\n \
\t\tsub\tbl, '0'\n\
\t\tadd\teax, ebx\n\
\t\tmov\tdword[integer], eax\n\
\t\tinc\tedi\n\
\t\tloop    iterate\n\
done:\n  \
\t\tcmp\tbyte[isnegative], 0\n\
\t\tje\t return\n\
\t\tneg\teax\n\
\t\tmov\tdword [integer], eax\n\
return:\n\
pop r8\n\
pop r9\n\
pop r10\n\
pop rdi\n\
pop rsi\n\
pop rdx\n\
pop rcx\n\
pop rbx\n\
pop rax\n\
mov	r11w, [integer]\n\
ret\n");
}


void simplePush(FILE *fp){
    fprintf(fp,"push rax\n\
push rbx\n\
push rcx\n\
push rdx\n\
push r11\n");

}

void simplePop(FILE *fp){
    fprintf(fp,"pop r11\n\
pop rdx\n\
pop rcx\n\
pop rbx\n\
pop rax\n\
");
}

void iprint(FILE *fp){
fprintf(fp, "%s\n","iprint:"); // put in r11d the value to be printed  
fprintf(fp,"push rax\n\
push rbx\n\
push rcx\n\
push rdx\n\
push rsi\n\
push rdi\n\
push r10\n\
push r9\n\
push r8\n\
");

fprintf(fp,"cmp r11d, 0000h\n\
jge positive\n\
neg r11d\n\
push r11\n\
mov ecx, minuss\n\
mov eax,4\n\
mov edx, 1\n\
mov ebx, 1\n\
int 80h\n\
pop r11\n\
positive:\n\
mov eax, r11d\n\
mov dword[count],0\n\
mov dword[extra] , r11d\n\
mov r8d, extra\n\
add r8d,7\n\
.charnex:\n\
mov edx,0\n\
mov ebx,10\n\
div ebx  \n\
add edx,'0'\n\
mov byte[r8d],dl\n\
dec r8d\n\
inc dword[count]\n\
cmp eax,0\n\
jne .charnex\n\
add r8d,1\n\
mov ecx, r8d\n\
mov edx, [count]\n\
mov eax, 4\n\
mov ebx, 1\n\
int 80h\n");

fprintf(fp,"pop r8\n\
pop r9\n\
pop r10\n\
pop rdi\n\
pop rsi\n\
pop rdx\n\
pop rcx\n\
pop rbx\n\
pop rax\n\
");

fprintf(fp,"ret\n\n");


 // for boolean 
fprintf(fp,"bprint:\n");   
fprintf(fp,"cmp r11d, 1 \n");        
fprintf(fp,"jne printfalse \n");
fprintf(fp,"printtrue:\n");  
simplePush(fp);
fprintf(fp,"mov rdx, 4\n");
fprintf(fp,"mov rcx, bool_true \n");
fprintf(fp,"mov rbx, 1\n");
fprintf(fp,"mov rax, 4\n");
fprintf(fp,"int 80h\n");
simplePop(fp);
fprintf(fp,"ret\n");

fprintf(fp,"printfalse:\n");
simplePush(fp);
fprintf(fp,"mov rdx, 5\n"); 
fprintf(fp,"mov rcx, bool_false \n");
fprintf(fp,"mov rbx, 1\n");
fprintf(fp,"mov rax, 4\n");
fprintf(fp,"int 80h\n");
simplePop(fp);
fprintf(fp,"ret\n\n");

 fprintf(fp, "enter_input_print:\n");
 simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,normal_input\n");
fprintf(fp, "mov edx,normal_inputLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");


fprintf(fp, "output_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,output\n");
fprintf(fp, "mov edx,outputLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "space_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,space\n");
fprintf(fp, "mov edx,spaceLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "line_feed_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,line_feed\n");
fprintf(fp, "mov edx,line_feedLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "integer_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,int_val\n");
fprintf(fp, "mov edx,int_valLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "real_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,real_val\n");
fprintf(fp, "mov edx,real_valLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "bool_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,bool_val\n");
fprintf(fp, "mov edx,bool_valLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "val_print:\n"); //print value string
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,val\n");
fprintf(fp, "mov edx,valLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "a_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,a_val\n");
fprintf(fp, "mov edx,a_valLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "an_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,an_val\n");
fprintf(fp, "mov edx,an_valLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "array_ele_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,array_ele\n");
fprintf(fp, "mov edx,array_eleLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "to_val_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,to_val\n");
fprintf(fp, "mov edx,to_valLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "type_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,type\n");
fprintf(fp, "mov edx,typeLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");


fprintf(fp, "complete_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,complete\n");
fprintf(fp, "mov edx,completeLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "error_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,error\n");
fprintf(fp, "mov edx,errorLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "array_error_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,indexOOB\n");
fprintf(fp, "mov edx,indexOOBLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "bound_mismatch_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,bound_mismatch\n");
fprintf(fp, "mov edx,bound_mismatchLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");

fprintf(fp, "size_mismatch_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,size_mismatch\n");
fprintf(fp, "mov edx,size_mismatchLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");



fprintf(fp, "incomp_bounds_print:\n");
simplePush(fp);
fprintf(fp, "mov eax,4\n");
fprintf(fp, "mov ebx,1\n");
fprintf(fp, "mov ecx,incomp_bounds\n");
fprintf(fp, "mov edx,incomp_boundsLen\n");
fprintf(fp, "int 80h\n");
simplePop(fp);
fprintf(fp, "ret\n\n");




//iprint is for real and num
//bprint is for bool

}





//-------------------writer functions------------------

void codeGen_dataInit(FILE *fp)
{

    fprintf(fp, "section .data\n");
    fprintf(fp, "normal_input: db \"Input: Enter\"\n");
    fprintf(fp, "normal_inputLen equ $-normal_input\n");
    fprintf(fp, "val: db \"value\"\n");
    fprintf(fp, "valLen equ $-val\n");
    fprintf(fp, "int_val: db \"Integer\"\n");
    fprintf(fp, "int_valLen equ $-int_val\n");
    fprintf(fp, "real_val: db \"Real\"\n");
    fprintf(fp, "real_valLen equ $-real_val\n");
    fprintf(fp, "bool_val: db \"Boolean\"\n");
    fprintf(fp, "bool_valLen equ $-bool_val\n");
    fprintf(fp, "a_val: db \"a\"\n");
    fprintf(fp, "a_valLen equ $-a_val\n");
    fprintf(fp, "space: db \" \"\n");
    fprintf(fp, "spaceLen equ $-space\n");
    fprintf(fp, "line_feed: db 10\n");
    fprintf(fp, "line_feedLen equ $-line_feed\n");
    fprintf(fp, "an_val: db \"an\"\n");
    fprintf(fp, "an_valLen equ $-an_val\n");
    //for array printing
    fprintf(fp, "array_ele: db \"array elements of\"\n");
    fprintf(fp, "array_eleLen equ $-array_ele\n");
    fprintf(fp, "type: db \"type for range\"\n");
    fprintf(fp, "typeLen equ $-type\n");
    fprintf(fp, "to_val: db \"to\"\n");
    fprintf(fp, "to_valLen equ $-to_val\n");
    fprintf(fp, "error: db \"error in the code\"\n");
    fprintf(fp, "errorLen equ $-error\n");
    fprintf(fp, "complete: db \"execution complete\"\n");
    fprintf(fp, "completeLen equ $-complete\n");
    fprintf(fp,"output: db \"Output:\"\n");
    fprintf(fp,"outputLen equ $-output\n");
    fprintf(fp,"minuss: db \"-\"\n");
    fprintf(fp,"indexOOB: db \"RUN TIME ERROR:  Array index out of bound\"\n");
    fprintf(fp,"indexOOBLen equ $-indexOOB\n");
    fprintf(fp,"size_mismatch:db \"Incompatible array size\"\n");
    fprintf(fp,"size_mismatchLen equ $-size_mismatch\n");
    fprintf(fp,"bound_mismatch: db \"Incompitable Array bounds\"\n");
    fprintf(fp,"bound_mismatchLen equ $-bound_mismatch\n");
    fprintf(fp,"incomp_bounds: db \"Lower bound greater than or equal to Upper Bound of Array\"\n");
    fprintf(fp,"incomp_boundsLen equ $-incomp_bounds\n");
    fprintf(fp, "\n\n\n");


    //labels true and false
    fprintf(fp,"bool_true: db \"true\"\n");
    
    fprintf(fp,"bool_false: db \"false\"\n");
    //print enter input pro
   

    iprint(fp);
    user_input(fp);
   // printf("CodeGen data section done\n");
}

void codeGen_bss(astNode *root, FILE *fp)
{
    fprintf(fp, "section .bss\n");
    astNode *control;
    if (root == NULL)
        return;
    control = root->down->start; //module declarations
    control = control->next;     //now on module definition
    if (control->label != -7)
    {
        astNode *local = control;
        while (local->label != -7)
        {
            astNode *module = (local->down)->start; // ID of module
            char moduleName[30];
            strncpy(moduleName, module->symTableEntry->value, 30);
           // printf("%s\n",moduleName);
            int functionSize = module->symTableEntry->offset;
            //TODO
            char dynamic_buffer[100];
            strncpy(dynamic_buffer, moduleName, 100);
            strncat(dynamic_buffer, DYNAMIC, 100);
            fprintf(fp, "%s: resb %d\n", moduleName, functionSize);
            fprintf(fp, "%s: resb %d\n", dynamic_buffer, DYNAMIC_SIZE);

            int i = 0;
         for (i = 0; i < module->symTableEntry->dcount; i++)
            {
                fprintf(fp, "%s_offset_%d: resb %d\n", moduleName, i, INT_SIZE*3);
            }
            local = local->list; //next module definition
        }
    }

   // printf("bss Module1 Done\n");
    control = control->next; //now driver module
    astNode *driverModule = control;
    char driverModuleName[30];
    strncpy(driverModuleName,"driver",30);
    strncpy(driverModule->symTableEntry->value,driverModuleName, 30);
   
    //printf("%s\n",driverModuleName);

    int functionSize = driverModule->symTableEntry->offset;
    //TODO
    char dynamic_buffer[100];
    strncpy(dynamic_buffer, driverModuleName, 100);
    strncat(dynamic_buffer, DYNAMIC, 100);
    fprintf(fp, "%s: resb %d\n", driverModuleName, functionSize);
    fprintf(fp, "%s: resb %d\n", dynamic_buffer, DYNAMIC_SIZE);
    int i = 0;
    for (i = 0; i < driverModule->symTableEntry->dcount; i++)
    {
        fprintf(fp, "%s_offset_%d: resb %d\n", driverModuleName, i, INT_SIZE*3);
    }
    //printf("%d\n",control->label);
   // printf("bss Driver Module Done\n");
    control = control->next; // Definitions after driver module
   // printf("%d\n",control->label);
    if (control->label != -7)
    {
        astNode *local = control;
        while (local->label != -7)
        {
            astNode *module = (local->down)->start; // ID of module
            char moduleName[30];
            strncpy(moduleName, module->symTableEntry->value, 30);
            int functionSize = module->symTableEntry->offset;
            //TODO
            char dynamic_buffer[100];
            strncpy(dynamic_buffer, moduleName, 100);
            strncat(dynamic_buffer, DYNAMIC, 100);
            fprintf(fp, "%s: resb %d\n", moduleName, functionSize);
            fprintf(fp, "%s: resb %d\n", dynamic_buffer, DYNAMIC_SIZE);
            for (i = 0; i < module->symTableEntry->dcount; i++)
            {
                fprintf(fp, "%s_offset_%d: resb %d\n", moduleName, i, INT_SIZE*3);
            }

            local = local->list; // next module definition
        }
        // Complete here
    }
	
    fprintf(fp, "buffer: resb 120\n"); //changed
    fprintf(fp,"count: resd 1\n");
    fprintf(fp,"extra: resb 8\n");
    fprintf(fp,"integer: resd 1\n"); //changed
    fprintf(fp,"isnegative: resb 1\n");	 //changed
    fprintf(fp, "section .text\n");
   // printf("bss Module2 Done\n");
    control = control->next;
}

void boundcheck(astNode *arrayID,astNode *index,FILE *fp, char callingModuleName[30])
{ //ID is the ID of array

    int offset = arrayID->symTableEntry->offset;//if static
    int lowerbound = arrayID->symTableEntry->a; //-1 if dynamic
    int upperbound = arrayID->symTableEntry->b; //-1 if dynamic
    int isStatic = arrayID->symTableEntry->isStatic;
    int isOutput=arrayID->symTableEntry->isOutput;
    if (index->label == index_id)
    { //array index is ID
        astNode *indexID = index->down->start;
        int indexoffset = indexID->symTableEntry->offset; //where the id resides in memory(relative)
        fprintf(fp, ";---------arraybound check--------------\n");
        simplePush(fp);
        fprintf(fp, "mov ax, %s\n", outputID(1, indexoffset, sizeOfID(indexID), callingModuleName)); //id is static
        
        if (isStatic == 1)
        { //static array

            //index is id
            fprintf(fp, "mov bx, %d\n", lowerbound);
            fprintf(fp, "mov cx, %d\n", upperbound);
            
        }
        else
        { //dynamic

            if(isOutput==2){  // check for this case !!
                    fprintf(fp, "mov bx,%s\n", outputID(1, offset+9, INT_SIZE, callingModuleName));
                    fprintf(fp, "mov cx,%s\n", outputID(1, offset+11, INT_SIZE, callingModuleName));
            }else{

                fprintf(fp, "mov bx , word[%s_offset_%d +2 ]\n",callingModuleName, arrayID->symTableEntry->dtag  );
                fprintf(fp, "mov cx , word[%s_offset_%d +4 ]\n",callingModuleName, arrayID->symTableEntry->dtag  );
                       
            }
        }
        fprintf(fp, "cmp ax,bx\n");
        fprintf(fp, "jl error_code\n"); // if index is less than equal to lowerbound
        fprintf(fp, "cmp ax,cx\n");
        fprintf(fp, "jg error_code\n"); // if index is greater than equal to upperbound
        simplePop(fp);
        fprintf(fp, ";--------array bound check---------------\n");
            
    }
    else if (index->label == 18)
    { //num
        astNode *indexNum = index->down->start;
        char indexnum[30];
        strncpy(indexnum, indexNum->tokenInfo->lexeme, 30);
        int indexValue = atoi(indexnum);

        if (arrayID->symTableEntry->isStatic == 0)
        {

            fprintf(fp, ";--------array bound check---------------\n");
            simplePush(fp);
            fprintf(fp, "mov ax,%d\n", indexValue);

            if(isOutput==2){ // check !!
                 fprintf(fp, "mov bx,%s\n", outputID(1, offset+9, INT_SIZE, callingModuleName));
                 fprintf(fp, "mov cx,%s\n", outputID(1, offset+11, INT_SIZE, callingModuleName));

            }else{

               fprintf(fp, "mov bx , word[%s_offset_%d +2 ]\n",callingModuleName, arrayID->symTableEntry->dtag  );
                fprintf(fp, "mov cx , word[%s_offset_%d +4 ]\n",callingModuleName, arrayID->symTableEntry->dtag  );
                    
            }
        fprintf(fp, "cmp ax,bx\n");
        fprintf(fp, "jl error_code\n"); // if index is less than equal to lowerbound
        fprintf(fp, "cmp ax,cx\n");
        fprintf(fp, "jg error_code\n"); // if index is greater than equal to upperbound
        simplePop(fp);
        fprintf(fp, ";--------array bound check---------------\n");

           
        }
    }
    
}




void codeGen_print(FILE *fp, astNode *begin, astNode *callingModuleID)
{

    char reg[10];
    char callingModuleName[30];
    strncpy(callingModuleName,callingModuleID->symTableEntry->value,30);

    astNode *node = begin->down->start;


    fprintf(fp,"call output_print\n");
    fprintf(fp,"call space_print\n");
    if(node->label==11){//num
        strncpy(reg,"r11w",10);
    }else if(node->label==12){//rnum
        strncpy(reg,"r11d",10);
    }

    if (node->label == 9)
    { //true
        fprintf(fp,"call printtrue\n");    
    }
    else if (node->label == 10)
    { //false
        fprintf(fp,"call printfalse\n");
    }
    else if (node->label == 11)
    { //num
        astNode *temp = node->down->start;
        char num[30];
        strncpy(num, temp->tokenInfo->lexeme, 30);
        fprintf(fp,"mov r11d,%s\n",num);
        fprintf(fp,"call iprint\n");
    }
    else if (node->label == 12)
    { //rnum
        astNode *temp = node->down->start;
        char rnum[30];
        strncpy(rnum, temp->tokenInfo->lexeme, 30);
        //fprintf(fp,"xor r11d,r11d\n",reg,reg);//making r11d zero
        fprintf(fp,"mov r11d,%s\n",rnum);
        fprintf(fp,"call iprint\n");    
    
    }
    else if (node->label == 13)
    { //array
        fprintf(fp,";--------array single element print----------\n");
        fprintf(fp,"xor rax,rax\n");
        astNode *ID = node->down->start;
        int size=sizeOfID(ID);
        int isStatic = ID->symTableEntry->isStatic;
        int offset = ID->symTableEntry->offset;
        int isOutput=ID->symTableEntry->isOutput;
        int messageLength = sizeOfID(ID); //will check if array is of type int real or bool and return the size
        
        if(messageLength==1){//bool
            strncpy(reg,"r11b",10);
        }else if(messageLength==2){//integer
            strncpy(reg,"r11w",10);

        }else{//real
            strncpy(reg,"r11d",10);

        }

        
        astNode *index = ID->next; //index
        boundcheck(ID,index,fp, callingModuleName);
        //insert code to bound check
        //----------------------------------------------------------
        

        if (index->label == 17)
        { //index is id
            astNode *indexID = index->down->start;
            int indexoffset = indexID->symTableEntry->offset;//where the id resides in memory(relative)
            
            fprintf(fp, "mov ax,%s\n", outputID(1, indexoffset, INT_SIZE, callingModuleName)); //retreving value of index from memory
            if(isStatic==1){
            int lower=ID->symTableEntry->a;
                fprintf(fp,"mov cx,%d\n",lower);
                fprintf(fp,"sub ax,cx\n");
                fprintf(fp,"mov cx,%d\n",size);
                fprintf(fp, "imul cx\n");
                fprintf(fp,"mov bx,%d\n",offset);      
                fprintf(fp, "add ax,bx\n");
            }else{
                int tag=ID->symTableEntry->dtag;
                if(isOutput==2){
                    fprintf(fp,"mov cx,%s\n",outputID(1,offset+9,INT_SIZE,callingModuleName));
                }else{
                    int m_offset=ID->symTableEntry->drange_begin->offset;
                    fprintf(fp,"mov cx,%s\n",outputID(1,m_offset,INT_SIZE,callingModuleName));
                }
                fprintf(fp,"sub ax,cx\n");
                fprintf(fp,"mov cx,%d\n",size);
                fprintf(fp, "imul cx\n");
                if(isOutput==2){
                    fprintf(fp,"movsx bx,%s\n",outputID(1,offset+8,1,callingModuleName));
                }else{
                    fprintf(fp,"mov bx,word[%s_offset_%d]\n",callingModuleName,tag);
                    
                }
                fprintf(fp,"add ax,bx\n");
            }                                                              // adding index offset to array offset
            fprintf(fp,"movsx rax,ax\n");
            if(isOutput==2){
                fprintf(fp,"add rax,qword[%s+%d]\n",callingModuleName,offset);
                fprintf(fp,"movsx %s[rax]\n",getbwd(messageLength));
            }else{
                fprintf(fp,"movsx r11d,%s\n",outputIDwithREG(isStatic,"rax",messageLength,callingModuleName));
            }
            if (messageLength == 1)
            {
                fprintf(fp, "call bprint\n");
            }
            else{ 
                fprintf(fp, "call iprint\n");
            }
            
        }
        else if (index->label == 18)
        { //index is num
            char indexnum[30];
            strncpy(indexnum, index->down->start->tokenInfo->lexeme, 30);
            int indexoffset=atoi(indexnum);
            fprintf(fp,"mov ax, %d\n",indexoffset);
            if(isStatic==1){
            int lower=ID->symTableEntry->a;
            fprintf(fp,"mov cx,%d\n",lower);
            fprintf(fp,"sub ax,cx\n");
            fprintf(fp,"mov cx,%d\n",size);
            fprintf(fp,"imul cx\n");
            fprintf(fp,"mov bx,%d\n",offset);
            fprintf(fp,"add ax,bx\n");
            
            }else{
                int tag=ID->symTableEntry->dtag;
                if(isOutput==2){
                    fprintf(fp,"mov cx,%s\n",outputID(1,offset+9,INT_SIZE,callingModuleName));
                }else{
                    int m_offset=ID->symTableEntry->drange_begin->offset;
                    fprintf(fp,"mov cx,%s\n",outputID(1,m_offset,INT_SIZE,callingModuleName));
                }
                fprintf(fp,"sub ax,cx\n");
                fprintf(fp,"mov cx,%d\n",size);
                fprintf(fp,"imul cx\n");

                if(isOutput==2){
                    fprintf(fp,"movsx bx,%s\n",outputID(1,offset+8,1,callingModuleName));
                }else{
                    fprintf(fp,"mov bx,word[%s_offset_%d]\n",callingModuleName,tag);
                    
                }
                fprintf(fp,"add ax,bx\n");
            }
            //fprintf(fp,"xor r11d,r11d\n",reg,reg);//making r11d zero
            fprintf(fp,"movsx rax,ax\n");
            if(isOutput==2){
                fprintf(fp,"add rax,qword[%s+%d]\n",callingModuleName,offset);
                fprintf(fp,"movsx %s[rax]\n",getbwd(messageLength));
            }else{
                fprintf(fp,"movsx r11d,%s\n",outputIDwithREG(isStatic,"rax",messageLength,callingModuleName));
            }
            if(messageLength==1){
            fprintf(fp,"call bprint\n");
            }else{
                fprintf(fp,"call iprint\n");
            }
        }
    }
    else if (node->label == 63)
    { //id
        //printf("ID\n");
        astNode *ID = node->down->start;
        int isStatic=ID->symTableEntry->isStatic;
        int isOutput=ID->symTableEntry->isOutput;
        int offset = ID->symTableEntry->offset;
        int messageLength = sizeOfID(ID);

        if(messageLength==1){//bool
            strncpy(reg,"r11b",10);
        }else if(messageLength==2){//integer
            strncpy(reg,"r11w",10);

        }else{//real
            strncpy(reg,"r11d",10);

        }
        if(ID->symTableEntry->type==0||ID->symTableEntry->type==1||ID->symTableEntry->type==2){
            //single id
           // printf("is not array\n");
            //fprintf(fp,"xor r11d,r11d\n",reg,reg);//making r11d zero
            fprintf(fp,"movsx r11d,%s\n",outputID(1, offset, messageLength, callingModuleName));
            
            
            if(messageLength==1){
                fprintf(fp,"call bprint\n");
            }else{
                fprintf(fp,"call iprint\n");
            }
        }else{
           // printf("is array\n");
            if(isStatic==1){
                
                //printf("printing static array\n");
                int offset=ID->symTableEntry->offset;
                int size=ID->symTableEntry->b-ID->symTableEntry->a+1;
                fprintf(fp,"mov ax,0\n");//loop variable
                fprintf(fp,"mov bx,%d\n",size);//holds the size

                if(isOutput==2){
                    fprintf(fp,"movsx rcx,%s\n",outputID(1,offset+8,1,callingModuleName));
                }else{
                    fprintf(fp,"mov rcx,%d\n",offset);
                }
            }else{
                int tag=ID->symTableEntry->dtag;

                if(isOutput==2){
                    fprintf(fp, "mov ax,%s\n", outputID(1, offset+9, INT_SIZE, callingModuleName));
                    fprintf(fp, "mov bx,%s\n", outputID(1, offset+11, INT_SIZE, callingModuleName));
                    
                }else{
                    int a = ID->symTableEntry->a;
                    int b = ID->symTableEntry->b;
                    int dtag=ID->symTableEntry->dtag;
                    fprintf(fp,"mov ax,word[%s_offset_%d+2]\n",callingModuleName,dtag);
                    fprintf(fp,"mov bx,word[%s_offset_%d+4]\n",callingModuleName,dtag);

                }
                fprintf(fp,"sub bx,ax\n");
                fprintf(fp,"add bx,1\n");//bx holds the size of the array
                fprintf(fp,"mov ax,0\n");

                if(isOutput==2){
                    fprintf(fp,"movsx rcx,%s\n",outputID(1,offset+8,1,callingModuleName));
                }else{
                    fprintf(fp,"movsx rcx,word[%s_offset_%d]\n",callingModuleName,tag);
                }
            }
                
                if(isOutput==2){
                    fprintf(fp,"add rcx,qword[%s+%d]\n",callingModuleName,offset);
                }
                fprintf(fp,"printArray_%d:\n",printArray);
                //fprintf(fp,"xor r11d,r11d\n");
                if(isOutput==2){
                    fprintf(fp,"movsx r11d,%s[rcx]\n",getbwd(messageLength));
                }else{
                    fprintf(fp,"movsx r11d,%s\n",outputIDwithREG(isStatic,"rcx",messageLength,callingModuleName));
                }
                if(messageLength==1){
                fprintf(fp,"call bprint\n");
                }else{
                    fprintf(fp,"call iprint\n");
                }
                fprintf(fp,"call space_print\n");
                
                fprintf(fp,"add rcx,%d\n",messageLength);
                fprintf(fp,"inc ax\n");
                fprintf(fp,"cmp ax,bx\n");
                fprintf(fp,"jne printArray_%d\n",printArray);
                printArray++;
            
        }
    }

    fprintf(fp,"call line_feed_print\n");

    
}


void codeGen_getvalue(astNode *begin, FILE *fp, astNode *callingModuleID)
{
    char reg[10];

    char callingModuleName[30];
    strncpy(callingModuleName,callingModuleID->symTableEntry->value,30);
    astNode *ID = begin->down->start;
    int offset = ID->symTableEntry->offset;
    int bytestoread = sizeOfID(ID);

    if(bytestoread==1){
        strncpy(reg,"r11b",10);
    }else if(bytestoread==2){
        strncpy(reg,"r11w",10);
    }else{
        strncpy(reg,"r11d",10);
    }

    int type = ID->symTableEntry->type;
    int isStatic = ID->symTableEntry->isStatic;

    fprintf(fp, "call enter_input_print\n");
    fprintf(fp, "call space_print\n");
    if (type == 0 || type == 1 || type == 2)
    {
        if (type == 0)
        {
            fprintf(fp, "call an_print\n");
            fprintf(fp, "call space_print\n");
            fprintf(fp, "call integer_print\n");
        }
        else if (type == 1)
        {
            fprintf(fp, "call a_print\n");
            fprintf(fp, "call space_print\n");
            fprintf(fp, "call real_print\n");
        }
        else
        {
            fprintf(fp, "call a_print\n");
            fprintf(fp, "call space_print\n");
            fprintf(fp, "call bool_print\n");
        }
        fprintf(fp, "call space_print\n");
        fprintf(fp, "call val_print\n");
        fprintf(fp, "call line_feed_print\n");

        //getting value
        fprintf(fp,"call user_input\n");
        fprintf(fp,"mov %s,%s\n",outputID(1,offset,sizeOfID(ID),callingModuleName),reg);
        
    }
    else
    {
        //input is an array

        if (isStatic == 1)
        {
            int size = ID->symTableEntry->b - ID->symTableEntry->a + 1;
            //fprintf(fp,"xor r11d,r11d\n");
            fprintf(fp, "mov r11d,%d\n", size);
            fprintf(fp, "mov bx,%d\n", size); //bx stores the size of array
            fprintf(fp, "call iprint\n");
            fprintf(fp, "call space_print\n");
            fprintf(fp, "call array_ele_print\n");
            fprintf(fp, "call space_print\n");
            if (type == 3)
            {
                fprintf(fp, "call integer_print\n");
                fprintf(fp, "call space_print\n");
            }
            else if (type == 4)
            {
                fprintf(fp, "call real_print\n");
                fprintf(fp, "call space_print\n");
            }
            else if (type == 5)
            {
                fprintf(fp, "call bool_print\n");
                fprintf(fp, "call space_print\n");
            }
            fprintf(fp, "call type_print\n");
            fprintf(fp, "call space_print\n");

            //a
            int lower = ID->symTableEntry->a;
            int upper = ID->symTableEntry->b;
            fprintf(fp,"xor r11d,r11d\n");
            fprintf(fp, "mov r11d,%d\n", lower);
            fprintf(fp, "call iprint\n");
            fprintf(fp, "call space_print\n");

            fprintf(fp, "call to_val_print\n");
            fprintf(fp, "call space_print\n");
            //b
            fprintf(fp,"xor r11d,r11d\n");
            fprintf(fp, "mov r11d,%d\n", upper);
            fprintf(fp, "call iprint\n");
        }
        else
        {

            int a = ID->symTableEntry->a;
            int b = ID->symTableEntry->b;
            if(ID->symTableEntry->isOutput==2){
                fprintf(fp, "mov ax,%s\n", outputID(1, ID->symTableEntry->offset+9, INT_SIZE, callingModuleName));
                fprintf(fp, "mov bx,%s\n", outputID(1, ID->symTableEntry->offset+11, INT_SIZE, callingModuleName));

            }else{
                int dtag=ID->symTableEntry->dtag;
                fprintf(fp,"mov ax,word[%s_offset_%d+2]\n",callingModuleName,dtag);
                fprintf(fp,"mov bx,word[%s_offset_%d+4]\n",callingModuleName,dtag);
            }
            fprintf(fp, "mov cx,ax\n"); //ecx stores the value of m
            fprintf(fp, "mov dx,bx\n"); //edx stores the value of n
            fprintf(fp, "sub bx,ax\n");
            fprintf(fp, "add bx,1\n");
            //fprintf(fp, "mov r8w,bx\n"); //r10w stores the size
            //fprintf(fp,"xor r11d,r11d\n");
            fprintf(fp, "movsx r11d,bx\n");
            //print size

            fprintf(fp, "call iprint\n");

            fprintf(fp, "call space_print\n");
            fprintf(fp, "call array_ele_print\n");
            fprintf(fp, "call space_print\n");
            if (type == 3)
            {
                fprintf(fp, "call integer_print\n");
                fprintf(fp, "call space_print\n");
            }
            else if (type == 4)
            {
                fprintf(fp, "call real_print\n");
                fprintf(fp, "call space_print\n");
            }
            else if (type == 5)
            {
                fprintf(fp, "call bool_print\n");
                fprintf(fp, "call space_print\n");
            }
            fprintf(fp, "call type_print\n");
            fprintf(fp, "call space_print\n");

            //a
            //fprintf(fp,"xor r11d,r11d\n");
            fprintf(fp, "movsx r11d,cx\n");
            fprintf(fp, "call iprint\n");
            fprintf(fp, "call space_print\n");

            //to
            fprintf(fp, "call to_val_print\n");
            fprintf(fp, "call space_print\n");
            //b
            //fprintf(fp,"xor r11d,r11d\n");
            fprintf(fp, "movsx r11d,dx\n");
            fprintf(fp, "call iprint\n");
        }
        fprintf(fp, "call line_feed_print\n");

        //getting value
        //r10 stores szie
        //r8 is the loop variable
        //r9 stores the relative offset on each array entry
        //now in r11 we are storing the array offset
        fprintf(fp, "mov dx,0\n"); //loop variable

        if(ID->symTableEntry->isOutput==2){
            fprintf(fp,"mov rcx,qword[%s+%d]\n",callingModuleName,offset);
            fprintf(fp,"movsx rax,%s\n",outputID(1,offset+8,1,callingModuleName));
            fprintf(fp,"add rcx,rax\n");
        }else{

            if(isStatic==1){
                fprintf(fp,"mov rcx,%d\n",offset);

            }else{
                int tag=ID->symTableEntry->dtag;
               // printf("movsx rcx, word[%s_offset_%d]\n",callingModuleName,tag);
                fprintf(fp,"movsx rcx, word[%s_offset_%d]\n",callingModuleName,tag);

            }
        }
        fprintf(fp, "getArray_%d:\n", getArray);
        fprintf(fp,"call user_input\n");

        if(ID->symTableEntry->isOutput==2){
            fprintf(fp,"mov %s[rcx],%s\n",getbwd(bytestoread),reg);
        }else{
            fprintf(fp,"mov %s,%s\n",outputIDwithREG(isStatic,"rcx",bytestoread,callingModuleName),reg);
        }
        fprintf(fp, "add rcx,%d\n", bytestoread);
        fprintf(fp, "inc dx\n");
        fprintf(fp, "cmp bx,dx\n");
        fprintf(fp, "jne getArray_%d\n", getArray);
        getArray++;
    }
}

void aoBexpGen(astNode *node, FILE *asmFile, char callingModuleName[30])
{
    astNode *lchild = node->down->start;
    astNode *rchild = node->down->end;

    switch (node->label){   
        
         case (bool_true):{ //checked
        
       // printf("entered true\n");
        fprintf(asmFile, "\tMOV RAX, 1\n");
        fprintf(asmFile, "\tPUSH RAX\n");
      
        break;
    }
    case(bool_false):{
       // printf("entered false\n");
        fprintf(asmFile, "\tMOV RAX, 0\n");
        fprintf(asmFile, "\tPUSH RAX\n");
    break;
    }
    case (num_var):
    {
        //printf("%s\n",lchild->tokenInfo->lexeme);
        fprintf(asmFile, "\tMOV RAX,%s\n", lchild->tokenInfo->lexeme);
        fprintf(asmFile, "\tPUSH RAX\n");
        break;
    }
    case (rnum_var):
    {
        fprintf(asmFile, "\tMOV RAX,%s\n", lchild->tokenInfo->lexeme);
        fprintf(asmFile, "\tPUSH RAX\n");
        break;
    }
    case (array_var):
    {
        
        int isStatic = lchild->symTableEntry->isStatic;
        int isOutput=lchild->symTableEntry->isOutput;
        astNode *index = lchild->next->down->start;
        boundcheck(lchild, lchild->next,asmFile, callingModuleName);
        if (lchild->next->label == index_id) { 
            int indexOffset = index->symTableEntry->offset;
            fprintf(asmFile, "\tMOV bx,%s\n", outputID(1, indexOffset, sizeOfID(index), callingModuleName));
           // printf("%s\n",outputID(1, indexOffset, sizeOfID(index), callingModuleName));
        }else{
            fprintf(asmFile, "\tMOV bx,%s\n", index->tokenInfo->lexeme);  
        }
        fprintf(asmFile,"mov ax,%d\n",sizeOfID(lchild));

        if (isStatic == 1){
                int lower=lchild->symTableEntry->a;
                int offset = lchild->symTableEntry->offset;
                fprintf(asmFile,"sub bx,%d\n",lower);
                fprintf(asmFile,"imul bx\n");
                fprintf(asmFile,"mov bx,ax\n");//relative offset of array
                fprintf(asmFile, "\tADD bx, %d\n", offset);
            }
            else
            {
                if(isOutput==2){
                    int offset=lchild->symTableEntry->offset;
                    fprintf(asmFile, "\tMOV dx,%s\n", outputID(1, offset+9, INT_SIZE, callingModuleName));    
                }else{
                    int m_offset=lchild->symTableEntry->drange_begin->offset;
                    fprintf(asmFile, "\tMOV dx,%s\n", outputID(1, m_offset, INT_SIZE, callingModuleName));
                }
                fprintf(asmFile,"sub bx,dx\n");
                fprintf(asmFile,"imul bx\n");
                fprintf(asmFile,"mov bx,ax\n");
                int tag = lchild->symTableEntry->dtag;

                if(isOutput==2){
                    int offset=lchild->symTableEntry->offset;
                    fprintf(asmFile,"movsx cx,%s\n",outputID(1,offset+8,1,callingModuleName));
                }else{
                fprintf(asmFile, "\tMOV cx,word[%s_offset_%d]\n", callingModuleName, tag);
                }
                fprintf(asmFile, "\t ADD bx,cx\n");
                }

            fprintf(asmFile,"movsx rbx,bx\n");
            if(isOutput==2){
                int offset=lchild->symTableEntry->offset;
                fprintf(asmFile,"add rbx,qword[%s+%d]\n",callingModuleName,offset);
                fprintf(asmFile,"movsx rax, %s[rbx]\n",getbwd(sizeOfID(lchild)));
            }else{
                fprintf(asmFile, "\tmovsx rax,%s\n", outputIDwithREG(isStatic, "RBX", sizeOfID(lchild), callingModuleName));
                
            }
        fprintf(asmFile, "\tPUSH RAX\n");
        
        break;
    }
    case (id_var):
    {

        //printf("%s\n",lchild->symTableEntry->value);
        int offset = lchild->symTableEntry->offset;

        fprintf(asmFile, "\tMOVSX RAX,%s\n", outputID(1, offset, sizeOfID(lchild), callingModuleName));
        fprintf(asmFile, "\tPUSH RAX\n");

        break;
    }

    case (term_node):
    { //op2

        aoBexpGen(lchild, asmFile, callingModuleName);
        aoBexpGen(rchild, asmFile, callingModuleName);
        fprintf(asmFile, "\tPOP RBX\n");
        fprintf(asmFile, "%s\n", "\tPOP RAX");
        if (strncmp(node->tokenInfo->token_t, "MU", 2) == 0)
        {
            fprintf(asmFile, "%s\n", "\tIMUL RBX");
            fprintf(asmFile, "%s\n", "\tPUSH RAX");
        }
        else
        { //divide
            fprintf(asmFile, "\t%s\n", "MOV RDX,0");
            fprintf(asmFile, "%s\n", "\tIDIV RBX");
            fprintf(asmFile, "%s\n", "\tPUSH RAX");
        }
        break;
    }

    case (expfactor_node):
    { //op1
        aoBexpGen(lchild, asmFile, callingModuleName);
        aoBexpGen(rchild, asmFile, callingModuleName);
        fprintf(asmFile, "\tPOP RBX\n");
        fprintf(asmFile, "%s\n", "\tPOP RAX");
        if (strncmp(node->tokenInfo->token_t, "PL", 2) == 0)
        { //plus
            fprintf(asmFile, "%s\n", "\tADD RAX,RBX");
            fprintf(asmFile, "%s\n", "\tPUSH RAX");
        }
        else
        { // minus
            fprintf(asmFile, "%s\n", "\tSUB RAX,RBX");
            fprintf(asmFile, "%s\n", "\tPUSH RAX");
        }
        break;
    }

    case (expterm_node):
    { //relop
        aoBexpGen(lchild, asmFile, callingModuleName);
        aoBexpGen(rchild, asmFile, callingModuleName);
        if (strcmp(node->tokenInfo->token_t, "LT") == 0)
        {
            fprintf(asmFile, "\tPOP RBX\n");
            fprintf(asmFile, "%s\n", "\tPOP RAX");
            fprintf(asmFile, "%s\n", "\tCMP RAX,RBX");
            fprintf(asmFile, "\tJL TRUE%d\n", num_relop);
            fprintf(asmFile, "\tMOV RAX,0\n");
            fprintf(asmFile, "\tJMP FALSE%d\n", num_relop);
            fprintf(asmFile, "\tTRUE%d: MOV RAX,1\n", num_relop);
            fprintf(asmFile, "\tFALSE%d:\n", num_relop);
            fprintf(asmFile, "\tPUSH RAX\n");
        }
        else if (strcmp(node->tokenInfo->token_t, "LE") == 0)
        {
            fprintf(asmFile, "\tPOP RBX\n");
            fprintf(asmFile, "%s\n", "\tPOP RAX");
            fprintf(asmFile, "%s\n", "\tCMP RAX,RBX");
            fprintf(asmFile, "\tJLE TRUE%d\n", num_relop);
            fprintf(asmFile, "\tMOV RAX,0\n");
            fprintf(asmFile, "\tJMP FALSE%d\n", num_relop);
            fprintf(asmFile, "\tTRUE%d: MOV RAX,1\n", num_relop);
            fprintf(asmFile, "\tFALSE%d:\n", num_relop);
            fprintf(asmFile, "\tPUSH RAX\n");
        }
        else if (strcmp(node->tokenInfo->token_t, "GT") == 0)
        {
            fprintf(asmFile, "\tPOP RBX\n");
            fprintf(asmFile, "%s\n", "\tPOP RAX");
            fprintf(asmFile, "%s\n", "\tCMP RAX,RBX");
            fprintf(asmFile, "\tJG TRUE%d\n", num_relop);
            fprintf(asmFile, "\tMOV RAX,0\n");
            fprintf(asmFile, "\tJMP FALSE%d\n", num_relop);
            fprintf(asmFile, "\tTRUE%d: MOV RAX,1\n", num_relop);
            fprintf(asmFile, "\tFALSE%d:\n", num_relop);
            fprintf(asmFile, "\tPUSH RAX\n");
        }
        else if (strcmp(node->tokenInfo->token_t, "GE") == 0)
        {
            fprintf(asmFile, "\tPOP RBX\n");
            fprintf(asmFile, "%s\n", "\tPOP RAX");
            fprintf(asmFile, "%s\n", "\tCMP RAX,RBX");
            fprintf(asmFile, "\tJGE TRUE%d\n", num_relop);
            fprintf(asmFile, "\tMOV RAX,0\n");
            fprintf(asmFile, "\tJMP FALSE%d\n", num_relop);
            fprintf(asmFile, "\tTRUE%d: MOV RAX,1\n", num_relop);
            fprintf(asmFile, "\tFALSE%d:\n", num_relop);
            fprintf(asmFile, "\tPUSH RAX\n");
        }
        else if (strcmp(node->tokenInfo->token_t, "EQ") == 0)
        {
            fprintf(asmFile, "\tPOP RBX\n");
            fprintf(asmFile, "%s\n", "\tPOP RAX");
            fprintf(asmFile, "%s\n", "\tCMP RAX,RBX");
            fprintf(asmFile, "\tJE TRUE%d\n", num_relop);
            fprintf(asmFile, "\tMOV RAX,0\n");
            fprintf(asmFile, "\tJMP FALSE%d\n", num_relop);
            fprintf(asmFile, "\tTRUE%d: MOV RAX,1\n", num_relop);
            fprintf(asmFile, "\tFALSE%d:\n", num_relop);
            fprintf(asmFile, "\tPUSH RAX\n");
        }
        else if (strcmp(node->tokenInfo->token_t, "NE") == 0)
        {
            fprintf(asmFile, "\tPOP RBX\n");
            fprintf(asmFile, "%s\n", "\tPOP RAX");
            fprintf(asmFile, "%s\n", "\tCMP RAX,RBX");
            fprintf(asmFile, "\tJNE TRUE%d\n", num_relop);
            fprintf(asmFile, "\tMOV RAX,0\n");
            fprintf(asmFile, "\tJMP FALSE%d\n", num_relop);
            fprintf(asmFile, "\tTRUE%d: MOV RAX,1\n", num_relop);
            fprintf(asmFile, "\tFALSE%d:\n", num_relop);
            fprintf(asmFile, "\tPUSH RAX\n");
        }

        num_relop++;
        break;
    }

    case (expression_node):
    {
        aoBexpGen(lchild, asmFile, callingModuleName);
        aoBexpGen(rchild, asmFile, callingModuleName);
        fprintf(asmFile, "\tPOP RBX\n");
        fprintf(asmFile, "%s\n", "\tPOP RAX");
        if (strcmp(node->tokenInfo->token_t, "AND") == 0)
        { //and
            fprintf(asmFile, "%s\n", "\tAND RAX,RBX");
            fprintf(asmFile, "%s\n", "\tPUSH RAX");
        }
        else
        { //or
            fprintf(asmFile, "%s\n", "\tOR RAX,RBX");
            fprintf(asmFile, "%s\n", "\tPUSH RAX");
        }

        break;
    }

    case (u_node):  //checked 
    {
        if (strncmp(node->tokenInfo->token_t, "MI",2)==0)
        {
            aoBexpGen(lchild, asmFile, callingModuleName);
            fprintf(asmFile, "%s\n", "\tPOP RAX");
            fprintf(asmFile, "%s\n", "\tMOV RBX, -1");
            fprintf(asmFile, "%s\n", "\tIMUL RBX");
            fprintf(asmFile, "%s\n", "\tPUSH RAX");
        }
        else
        {
            aoBexpGen(lchild, asmFile, callingModuleName);
            fprintf(asmFile, "\tMOV RAX,%s\n", lchild->tokenInfo->lexeme);
            fprintf(asmFile, "\tPUSH RAX\n");
        }
        break;
    }
    }
}




void codeGen_while(astNode *node, FILE *asmFile,astNode *callingModuleID) //checked
{
    
    int dcount=callingModuleID->symTableEntry->dcount;
    char callingModuleName[30];
    strncpy(callingModuleName,callingModuleID->symTableEntry->value,30);

    int whileLocal = whileCount;
    whileCount++; // global var

    fprintf(asmFile, "whileBegin%d:\n", whileLocal);
    astNode *aoBexp = node->down->start;
    aoBexpGen(aoBexp, asmFile, callingModuleName); // pushes the final value on tos before returning
    fprintf(asmFile, "\tPOP RAX\n");
    fprintf(asmFile, "\tCMP RAX, 1\n"); // compare the value of true
    fprintf(asmFile, "\tJNZ whileEnd%d\n", whileLocal);

    astNode *stmts = aoBexp->next;
    while (stmts->label != -7)
    {
        processNode(stmts,asmFile,callingModuleID);
        stmts = stmts->list;
    }
    fprintf(asmFile, "\tJMP whileBegin%d\n", whileLocal);
    fprintf(asmFile, "whileEnd%d:\n", whileLocal); //use atoi for lexeme
}


void codeGen_switch(astNode *root, FILE *fp, astNode *callingModuleID){ //checked 

    char callingModuleName[30];
    strncpy(callingModuleName,callingModuleID->symTableEntry->value,30);
    astNode *ID = root->down->start;
    int offset = ID->symTableEntry->offset;
    int size = sizeOfID(ID);
    
    fprintf(fp, "\tMOVSX EBX, %s\n", outputID(1, offset, size, callingModuleName)); //switch expression
    fprintf(fp, "\tswitch_begin%d:\n", switchCount);
    astNode *cases = ID->next;
    astNode *default_case = cases->next;
    int local = switchCount;
    switchCount++;
    int no; //count of cases
    if (ID->symTableEntry->type == 0)
    { // num
        no = 1;
        while (cases->label != -7)
        { //if not null
       // printf("\tcaseStmt%d_%d:\n", local, no);
            fprintf(fp, "\tcaseStmt%d_%d:\n", local, no);
            int case_value = atoi(cases->down->start->down->start->tokenInfo->lexeme);
            no++;
            if (cases->down->start->label == value_minus_num)
            {
                fprintf(fp, "%s\n", "\tMOV EAX, -1");
                fprintf(fp, "\tMOV ECX,%d\n", case_value);
                fprintf(fp, "%s\n", "\tIMUL ECX");
                fprintf(fp, "%s\n", "MOV ECX, EAX");
            }
            else
            { //value_plus_num // value_num
                fprintf(fp, "\tMOV ECX,%d\n", case_value);
            }
            fprintf(fp, "\tCMP ECX,EBX\n");
            if(cases->list->label!=-7){
            	fprintf(fp, "\tJNE caseStmt%d_%d\n", local, no);
            }else{
                fprintf(fp, "\tJNE default%d\n",local);
            }
            astNode *stmts = cases->down->start->next; //statements
            //fprintf(fp,"push rbx\n");
            while (stmts->label != -7)
            {
                processNode(stmts,fp,callingModuleID);
                stmts = stmts->list;
            }
            fprintf(fp, "\tjmp endSwitch%d\n", local);
            cases = cases->list;
        }
        
        cases = default_case;
        //printf("label:%d\n",cases->label);
        if (cases->label == default_node)
        {
            fprintf(fp, "default%d:\n", local);
            astNode *stmts = cases->down->start;
            while (stmts->label!=-7)
            {
                
                processNode(stmts,fp,callingModuleID);
                stmts = stmts->list;
            }
        }
        //fprintf(fp,"pop rbx\n");
    }
    else
    {                                     //boolean
        fprintf(fp, "\tCMP EBX, 1\n"); // check in asm
        fprintf(fp, "\tJNE case_false%d\n",local);
        fprintf(fp, "\tJMP case_true%d\n", local);
        while (cases->label != -7)
        {
            if (cases->down->start->label == value_true)
            {
                astNode *stmts = cases->down->start->next;
                fprintf(fp, "\tcase_true%d:\n",local);
                while (stmts->label != -7)
                {
                processNode(stmts,fp,callingModuleID);
                    stmts = stmts->list;
                }
                fprintf(fp, "\tjmp endSwitch%d\n", local);
            }
            else
            { // false
                astNode *stmts = cases->down->start->next;
                fprintf(fp, "\tcase_false%d:\n", local);
                while (stmts->label != -7)
                {
                processNode(stmts,fp,callingModuleID);
                    stmts = stmts->list;
                }
                fprintf(fp, "\tjmp endSwitch%d\n", local);
            }
            cases = cases->list;
        }
    }
    fprintf(fp, "endSwitch%d:\n", local);
    
}

void codeGen_declare(astNode *begin, FILE *fp, astNode *callingModuleID)
{
    char callingModuleName[30];
    strncpy(callingModuleName,callingModuleID->symTableEntry->value,30);
    int dcount=callingModuleID->symTableEntry->dcount;
    astNode *IDlist = begin->down->start;
    //printf("%d\n",IDlist->label);
    while(IDlist->label!=-7){
    //printf("%d\n",IDlist->label);
    
    astNode *ID=IDlist->down->start;
    //printf("%s\n");
    if (!ID->symTableEntry->isStatic)
    {
        //dynamic
        int tag = ID->symTableEntry->dtag;
        int a=ID->symTableEntry->a;
        int b=ID->symTableEntry->b;
        if(a==-1&&b==-1){
            printf("hello\n");
            int m_offset = ID->symTableEntry->drange_begin->offset;
            int n_offset = ID->symTableEntry->drange_end->offset;
            fprintf(fp, "mov ax,%s\n", outputID(1, m_offset, INT_SIZE, callingModuleName));
            fprintf(fp, "mov bx,%s\n", outputID(1, n_offset, INT_SIZE, callingModuleName));

        }else if(a==-1&&b!=-1){
            int m_offset = ID->symTableEntry->drange_begin->offset;
            fprintf(fp, "mov ax,%s\n", outputID(1, m_offset, INT_SIZE, callingModuleName));
            fprintf(fp, "mov bx,%d\n",b);

        }else if(a!=-1&&b==-1){
            int n_offset = ID->symTableEntry->drange_end->offset;
            fprintf(fp, "mov ax,%d\n", a);
            fprintf(fp, "mov bx,%s\n",outputID(1, n_offset, INT_SIZE, callingModuleName));


        }
        printf("asda\n");
        fprintf(fp,"mov word[%s_offset_%d + 2],ax \n", callingModuleName, tag);
        fprintf(fp,"mov word[%s_offset_%d + 4],bx\n", callingModuleName, tag );
        fprintf(fp,"cmp bx,ax\n");
        fprintf(fp,"jle incomp_bounds_error\n");
        
        if (tag == 0)
        {
            fprintf(fp,"mov word[%s_offset_0],0\n", callingModuleName);
            
            if(dcount>1){
                // fprintf(fp, "mov ax,%s\n", outputID(1, m_offset, INT_SIZE, callingModuleName));
                // fprintf(fp, "mov bx,%s\n", outputID(1, n_offset, INT_SIZE, callingModuleName));
                fprintf(fp, "sub bx,ax\n");
                fprintf(fp, "inc bx\n"); //size of array
                fprintf(fp, "mov ax, %d\n", sizeOfID(ID));
                fprintf(fp, "imul bx\n"); //total size taken by array;
                fprintf(fp, "mov word[%s_offset_1],ax\n", callingModuleName);
            }
        }
        else if (tag <dcount-1)
        {          
            fprintf(fp, "sub bx,ax\n");
            fprintf(fp, "inc bx\n"); //size of array
            fprintf(fp, "mov ax, %d\n", sizeOfID(ID));
            fprintf(fp, "imul bx\n"); //total size taken by array;
            fprintf(fp, "add ax, word[%s_offset_%d]\n", callingModuleName, tag);
            fprintf(fp, "add ax, 4\n") ;
            fprintf(fp, "mov word[%s_offset_%d], ax\n", callingModuleName, tag + 1);     
        }
    }
    IDlist=IDlist->list;
    }
}

void codeGen_for(astNode *root, FILE *asmFile,astNode *callingModuleID) //checked
{
    int dcount=callingModuleID->symTableEntry->dcount;
    char callingModuleName[30];
    strncpy(callingModuleName,callingModuleID->symTableEntry->value,30);
    int forLocal = forCount; // forCount global var initialise to zero
    forCount++;
astNode *id = root->down->start;
    astNode *range = root->down->start->next;
int i = atoi(range->down->start->tokenInfo->lexeme);
    int j = atoi(range->down->start->next->tokenInfo->lexeme);
    
    fprintf(asmFile, "\tMOV CX,%d\n", i);
    fprintf(asmFile, "forBegin%d:\n", forLocal);    
    fprintf(asmFile, "\tMOV word[%s+%d],CX\n", callingModuleName, id->symTableEntry->offset); // check 
    astNode *stmt = range->next;
    
    while (stmt != NULL)
    {
        processNode(stmt,asmFile,callingModuleID);
        stmt = stmt->list;
    }
    
//fprintf(asmFile, "\t;executed all for stamtemnts inside \n");
	fprintf(asmFile, "\tMOV RCX,0\n");
    fprintf(asmFile, "\tMOV CX,word[%s+%d]\n", callingModuleName ,id->symTableEntry->offset);
    fprintf(asmFile, "\tadd CX, 1\n");
    fprintf(asmFile, "\tMOV word[%s+%d],CX\n",callingModuleName, id->symTableEntry->offset);
    fprintf(asmFile, "\tCMP CX,%d\n", j);
    fprintf(asmFile, "\tJLE forBegin%d\n", forLocal);
    
}
void codeGen_moduleReuse(astNode *begin, FILE *fp, astNode *callingModuleID)
{   
    char callingModuleName[30];
    strncpy(callingModuleName,callingModuleID->symTableEntry->value,30);
    astNode *output = begin->down->start; //optional_node
    astNode *module = output->next;
    char moduleName[30];
    //printf("module Name:%s\n",module->symTableEntry->value);
    strncpy(moduleName, module->symTableEntry->value, 30);
    astNode *input = module->next; //input is the parameters passed to the function
    //function input output parameters
    inout *inputlist = module->symTableEntry->in;
    inout *outputlist = module->symTableEntry->out;

    //fprintf(fp, "pushad\n");

    if (input->label != -7)
    {
        astNode *temp1 = input;
        
        int to_offset = 0;
        while (temp1->label != -7)
        {
           // printf("size11:%d\n",temp1->label);
            astNode *ID=temp1->down->start;
            int size = sizeOfID(ID);
            
            int type = ID->symTableEntry->type;
            if (type == 0 || type == 1 || type == 2)
            {
                //id is not an array
                int from_offset = ID->symTableEntry->offset;
                int to_offset = inputlist->para->offset;
                copy(callingModuleName, moduleName, from_offset, to_offset, size, fp);
            }
            else
            {
                //id is an array
                //assuming the parameter array will be static
                //copyArray(ID, inputlist->para, callingModuleName, moduleName, fp, 0);

                
                if(inputlist->para->isStatic==1){
                arraySize(ID->symTableEntry,fp,"a",callingModuleName,0);
                arraySize(inputlist->para,fp,"b",callingModuleName,1);
                fprintf(fp,"cmp ax,bx\n");
                fprintf(fp,"jne size_mismatch_error\n");

                arrayBounds(ID->symTableEntry,fp,"a","b",callingModuleName,0);
                arrayBounds(inputlist->para,fp,"c","d",callingModuleName,1);
                fprintf(fp,"cmp ax,cx\n");
                fprintf(fp,"jne bound_mismatch_error\n");
                fprintf(fp,"cmp bx,dx\n");
                fprintf(fp,"jne bound_mismatch_error\n");
                }

                
                int from_offset=ID->symTableEntry->offset;
                int to_offset=inputlist->para->offset;
                int isStatic=ID->symTableEntry->isStatic;
                int lower=ID->symTableEntry->a;
                int upper=ID->symTableEntry->b;
                if(ID->symTableEntry->isOutput==0){
                    if(isStatic==1){  
                        fprintf(fp,"mov rax,%s\n",callingModuleName);
                        fprintf(fp,"mov qword[%s+%d],rax\n",moduleName,to_offset);//storing address in in first 8 bytes
                        fprintf(fp,"mov %s,%d\n",outputID(1,to_offset+8,1,moduleName),from_offset);//storing offset in next 1 byte
                        fprintf(fp,"mov %s,%d\n",outputID(1,to_offset+9,2,moduleName),lower);
                        fprintf(fp,"mov %s,%d\n",outputID(1,to_offset+11,2,moduleName),upper);
                }else{
                    int dtag=ID->symTableEntry->dtag;
                    fprintf(fp,"mov rax,%s_dynamic\n",callingModuleName);
                    fprintf(fp,"mov qword[%s+%d],rax\n",moduleName,to_offset);//storing address in in first 8 bytes
                    fprintf(fp,"mov cl,byte[%s_offset_%d]\n",callingModuleName,dtag);
                    fprintf(fp,"mov %s,cl\n",outputID(1,to_offset+8,1,moduleName));//storing offset in next 1 byte

                    fprintf(fp,"mov ax,word[%s_offset_%d+2]\n",callingModuleName,dtag);
                    fprintf(fp,"mov bx,word[%s_offset_%d+4]\n",callingModuleName,dtag);

                    fprintf(fp,"mov %s,ax\n",outputID(1,to_offset+9,2,moduleName));
                    fprintf(fp,"mov %s,bx\n",outputID(1,to_offset+11,2,moduleName));
                
            }
                
        }else if(ID->symTableEntry->isOutput==2){
            fprintf(fp,"mov rax,qword[%s+%d]\n",callingModuleName,from_offset);//buffer determination
            fprintf(fp,"mov qword[%s+%d],rax\n",moduleName,to_offset);
            fprintf(fp,"mov al,%s\n",outputID(1,from_offset+8,1,callingModuleName));//array offset in buffer
            fprintf(fp,"mov %s,al\n",outputID(1,to_offset+8,1,moduleName));
            fprintf(fp,"mov ax,%s\n",outputID(1,from_offset+9,2,callingModuleName));//lower bound
            fprintf(fp,"mov %s,ax\n",outputID(1,to_offset+9,2,moduleName));
            fprintf(fp,"mov ax,%s\n",outputID(1,from_offset+11,2,callingModuleName));//upper bound
            fprintf(fp,"mov %s,ax\n",outputID(1,to_offset+11,2,moduleName));
            
            
            
        }
                
                

            }

            inputlist = inputlist->next;
            temp1 = temp1->list;
        }
    }
    fprintf(fp, "call module_%s\n", moduleName);

    if (output->label != -7)
    {
        
        astNode *temp1 = output->down->start; //ID_list
        while (temp1->label != -7)
        {
            
            astNode *ID=temp1->down->start;
            int size = sizeOfID(ID);
            int type = ID->symTableEntry->type;
            if (type == 0 || type == 1 || type == 2)
            {
               // printf("sizeaa:%d\n",temp1->label);
                //id is not an array
                int to_offset = ID->symTableEntry->offset;
                int from_offset = outputlist->para->offset;
                copy(moduleName, callingModuleName, from_offset, to_offset, size, fp);
            }
            
            outputlist = outputlist->next;
            temp1 = temp1->list;
        }
    }
   
    //fprintf(fp, "popad\n");
}




void codeGen_assignStmt(astNode *node, FILE *asmFile,astNode* callingModuleID)
{
    char callingModuleName[30];
    strncpy(callingModuleName,callingModuleID->symTableEntry->value,30);
    astNode *ID = node->down->start;
    astNode *temp = ID->next;
    astNode *rhs=node->down->end->down->end;
    int size=sizeOfID(ID);
    int type=ID->symTableEntry->type;
    
    if(type==0||type==1||type==2||temp->label==lvalueARRStmt_node){
    fprintf(asmFile,"xor RAX,RAX\n");
    fprintf(asmFile,"xor RBX,RBX\n");
   // printf("---------evaulating RHS--------\n");
    aoBexpGen(rhs, asmFile, callingModuleName);
   // printf("---------RHS Evaluated--------\n");
    }
    if(temp->label==lvalueARRStmt_node){
        int isStatic = ID->symTableEntry->isStatic;
        astNode *index = ID->next->down->start;
        boundcheck(ID,index,asmFile,callingModuleName);
        int isOutput=ID->symTableEntry->isOutput;
        int offset=ID->symTableEntry->offset;
        if (index->label == index_id){
           
        int indexOffset = index->down->start->symTableEntry->offset;
			fprintf(asmFile, "\tMOV rbx, 0\n");         
			fprintf(asmFile, "\tMOV bx,%s\n", outputID(1, indexOffset, sizeOfID(index->down->start), callingModuleName));
            
        }else{
			 fprintf(asmFile, "\tMOV rbx, 0\n");
             fprintf(asmFile, "\tMOV bx,%s\n", index->down->start->tokenInfo->lexeme);     
        }
        fprintf(asmFile, "\tMOV rax, 0\n");    
        fprintf(asmFile,"mov ax,%d\n",sizeOfID(ID));
        if (isStatic == 1){
              //  printf("static\n");
                int lower=ID->symTableEntry->a;
                int offset = ID->symTableEntry->offset;
                fprintf(asmFile,"sub bx,%d\n",lower);
                fprintf(asmFile,"imul bx\n");
                fprintf(asmFile,"mov bx,ax\n");//relative offset of array
                fprintf(asmFile, "\tADD bx, %d\n", offset);
            
            }
            else
            {
               // printf("dyanmic\n");

                if(isOutput==2){
                    fprintf(asmFile,"mov dx,%s\n",outputID(1,offset+9,INT_SIZE,callingModuleName));
                }else{
                    int m_offset=ID->symTableEntry->drange_begin->offset;
                    fprintf(asmFile, "\tMOV dx,%s\n", outputID(1, m_offset, INT_SIZE, callingModuleName));
                }
                fprintf(asmFile,"sub bx,dx\n");
                fprintf(asmFile,"imul bx\n");
                fprintf(asmFile,"mov bx,ax\n");

                if(isOutput==2){
                    fprintf(asmFile,"movsx cx,%s\n",outputID(1,offset+8,1,callingModuleName));
                }else{
                    int tag = ID->symTableEntry->dtag;
                    fprintf(asmFile, "\tMOV cx,word[%s_offset_%d]\n", callingModuleName, tag);
                }
                fprintf(asmFile, "\t ADD bx,cx\n");
            }
            fprintf(asmFile,"pop rax\n");
            fprintf(asmFile,"movsx rbx,bx\n");
            if(isOutput==2){
                fprintf(asmFile,"add rbx,qword[%s+%d]\n",callingModuleName,offset);
                fprintf(asmFile,"movsx %s[rbx],rax\n",getbwd(sizeOfID(ID)));
            }else{
                fprintf(asmFile, "\tmov %s,%s\n", outputIDwithREG(isStatic, "RBX", sizeOfID(ID), callingModuleName),getReg("a",sizeOfID(ID)));
                            
            }

                    
    }else
        { //lvalueIDStmt_node  //RHS
            
            if(type==0||type==1||type==2){
            int offset = ID->symTableEntry->offset;
            fprintf(asmFile,"pop rax\n");
            fprintf(asmFile, "MOV %s,%s\n", outputID(1, offset, sizeOfID(ID), callingModuleName),getReg("a",sizeOfID(ID))); // MEM access // may change
        }else{
           // printf("hello dasda\n");
            copyArray(ID,rhs->down->start,asmFile,callingModuleName);

        }
        }
    //fprintf(asmFile,"XOR RAX,RAX\n");
}


//-----------------------------driver functions--------------------------------------
void processNode(astNode *stmts, FILE *fp, astNode* callingModuleID)
{
    switch (stmts->label)
    {
    case 7:
        //printf("--------getValue start---------\n");
        codeGen_getvalue(stmts, fp, callingModuleID);
        //printf("--------getValue end---------\n");

        break;
    case 8:
       // printf("--------print start---------\n");
        codeGen_print(fp, stmts, callingModuleID);
       // printf("--------print end---------\n");
        break;
    case 14:
       // printf("--------assignment start---------\n");
        codeGen_assignStmt(stmts, fp, callingModuleID);
       // printf("--------assignment start---------\n");
        break;
    case 19:
       // printf("--------moduleReuse start---------\n");
        codeGen_moduleReuse(stmts, fp, callingModuleID);
       // printf("--------moduleReuse end---------\n");
        break;
    case 46:
       // printf("--------declare start---------\n");
        codeGen_declare(stmts, fp,callingModuleID);
       // printf("--------declare end---------\n");
        break;
    case 47:
       // printf("--------switch start---------\n");
        codeGen_switch(stmts, fp,callingModuleID);
       // printf("--------switch end---------\n");
        break;
    case 53:
       // printf("--------for start---------\n");
        codeGen_for(stmts,fp,callingModuleID);
       // printf("--------for end---------\n");
        //handle_for_stmt(temp1,exists->outal,0);
        break;
    case 54:
       // printf("--------while start---------\n");
        codeGen_while(stmts, fp,callingModuleID);
       // printf("--------while end---------\n");
        break;
    }
}


void codeGen_driverModule(astNode *begin, FILE *fp)
{

    astNode *module = begin->down->start;
    astNode *stmts = module;
    char moduleName[30];
    strncpy(moduleName, begin->symTableEntry->value, 30);
    int functionSize = begin->symTableEntry->offset;
    int dcount = begin->symTableEntry->dcount;
    fprintf(fp, "global main\n");
    fprintf(fp, "main:\n");

    while (stmts->label != -7)
    {
        processNode(stmts,fp,begin);
        stmts = stmts->list; //next statement
    }
    fprintf(fp, "mov eax,1; The system call for exit\n");
    fprintf(fp, "mov ebx,0; Exit with return code of 0 (no error)\n");
    fprintf(fp, "int 80h\n");
    fprintf(fp, "ret\n");
    fprintf(fp, "error_code:\n");
    fprintf(fp, "call array_error_print\n");
    fprintf(fp, "mov eax,1; The system call for exit\n");
    fprintf(fp, "mov ebx,0; Exit with return code of 0 (no error)\n");
    fprintf(fp, "int 80h\n");
    fprintf(fp, "ret\n");

    fprintf(fp, "bound_mismatch_error:\n");
    fprintf(fp, "call bound_mismatch_print\n");
    fprintf(fp, "mov eax,1; The system call for exit\n");
    fprintf(fp, "mov ebx,0; Exit with return code of 0 (no error)\n");
    fprintf(fp, "int 80h\n");
    fprintf(fp, "ret\n");

    fprintf(fp, "size_mismatch_error:\n");
    fprintf(fp, "call size_mismatch_print\n");
    fprintf(fp, "mov eax,1; The system call for exit\n");
    fprintf(fp, "mov ebx,0; Exit with return code of 0 (no error)\n");
    fprintf(fp, "int 80h\n");
    fprintf(fp, "ret\n");

    fprintf(fp, "incomp_bounds_error:\n");
    fprintf(fp, "call incomp_bounds_print\n");
    fprintf(fp, "mov eax,1; The system call for exit\n");
    fprintf(fp, "mov ebx,0; Exit with return code of 0 (no error)\n");
    fprintf(fp, "int 80h\n");
    fprintf(fp, "ret\n");
}

void codeGen_Module(astNode *begin, FILE *fp)
{
    astNode *module = begin->down->start;
    int functionSize = module->symTableEntry->offset;
    int dcount = module->symTableEntry->dcount;
    char moduleName[30];
    strncpy(moduleName, module->symTableEntry->value, 30);
    fprintf(fp, "module_%s:\n", moduleName);
    simplePush(fp);
    //fprintf(fp, "pushad\n");

    astNode *stmts = module->next->next->next; //input->output->body
    while (stmts->label != -7)
    { //stmts is dealing with statements
        processNode(stmts,fp,module);
        stmts = stmts->list; //next statement
    }

    simplePop(fp);
    fprintf(fp, "ret\n");
}

void codeGen_driver(astNode *root, FILE *fp)
{

    codeGen_dataInit(fp);
    astNode *bss = root;
    codeGen_bss(bss, fp);

    astNode *control;
    if (root == NULL)
        return;
    control = root->down->start; //module declarations
    control = control->next;     //now on module definition
    if (control->label != -7)
    {
       // printf("---------codeGen module1 starting----------\n");
        astNode *local = control;
        while (local->label != -7)
        {
            codeGen_Module(local, fp);
            local = local->list;
        }
      //  printf("---------codeGen module complete----------\n");
    }

    control = control->next; //driver module
    //printf("-------------codeGen driverModule starting--------------\n");
    codeGen_driverModule(control, fp);
   // printf("-------------codeGen driverModule Done--------------\n");
    control = control->next; // Definitions after driver module
   // printf("-------------codeGen Module2 starting--------------\n");

    if (control->label != -7)
    {
        astNode *local = control;
        while (local->label != -7)
        {
            codeGen_driverModule(local, fp);
        }

     //   printf("---------------codeGen module2 Done-------------\n");
    }

    control = control->next;
    printf(".............Code Generation phase completed................... \n");
    
}
