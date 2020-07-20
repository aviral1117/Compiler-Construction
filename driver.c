/* # Group 01
    Aviral Sethi 	: 2016B3A70532P
    SANJEET MALHOTRA 	: 2016B4A70601P
    PRIYANKA VERMA 	: 2016B3A70492P
    ANIRUDH VIJAY 	: 2016B3A70525P  
*/
#include "codeGen.h"
#include<sys/time.h>
#include<time.h>

void checkAst(astNode* root){
    if(!root) return;
    astNode* cur = root->down->start;
    while(cur){
        checkAst(cur);
        cur = cur->next;
    }
    if(root->label==-1){

        if(root->symTableEntry==NULL){
            printf("Here NULL\n");
            fflush(stdout);
        }
        printf("id: %s %d\n", root->tokenInfo->lexeme, root->tokenInfo->lno);
        fflush(stdout);
    }
    checkAst(root->list);
}

node* parseRoot = NULL;
astNode* astRoot = NULL;
table* scopeRoot = NULL;

int main(int argc, char* argv[]){
    printf("---------------------------------------\n");
    printf("LEVEL 4: All-Stage1/AST/Symbol table/Type Checking/ Semantic rules/ Code Generation modules works/ Successfully Handled static and dynamic arrays in type checking and code generation.\nAll modules implemented completely and are working fine.\n");
    int mode=-1;
   // printf("Enter the desired option to be executed:\n");
   // printf("0 Exit.\n 1: Lexer\n 2: Parser\n 3: AST\n 4: Memory\n 5: Semantics\n");
    while(1){
        printf("---------------------------------------\n(Group #1) COMPILER OPTIONS FOR ERPLAG\n---------------------------------------\n");
        //printf("LEVEL 4: All-Stage1/AST/Symbol table/Type Checking/ Semantic rules modules works/ Successfully Handled static and dynamic arrays in type checking and code generation\n. Everything is working fine to the best of our knowledge");
        printf("Enter the desired option to be executed:\n");
        printf(" 0: Exit.\n 1: Lexer\n 2: Parser\n 3: AST\n 4: Memory\n 5: Print Symbol Table\n 6: Print Activation Record \n 7: Print Arrays \n 8: Semantics\n 9: Code Gen(Full Compiler)\n");
        scanf("%d",&mode);
        if(mode==0){
            printf("Exiting...\n");
            break;
        }
        else if(mode==1){
            printf("Printing Token List after Lexical Analysis...\n\n");
            int m = lexerDependencies(argv[1]);
            if(!m){
                printf("LINE_Number\t\t\t LEXEME \t\t\t\tToken_Name\n\n");
                while(1){
                    token* tk = getNextToken();
                    if(tk->tag==-1){
                        printToken(tk);
                        printf("\nDollar is the last token just for reference\n");
                        break;
                    }
                    printToken(tk);
                    printf("\n");
                }
            }
        }
        else if(mode==2){
            printf("Parsing Source Code...\n");
            parseRoot = parserDriver(argc,argv);
            printParseTree(parseRoot/*, argv[2]*/);
            /*if(root!=NULL)
                freeMemory(root);*/
            if(correct){
                printf("\n...........Input source code is syntactically correct...........\n");
            }
            else{
                printf("\n...........Input source code is not syntactically correct...........\n"); 
            }
        }
        else if(mode==3){
            printf("Parsing Source Code...\n");
            parseRoot = parserDriver(argc,argv);
            if(!correct){
                printf("Code is not syntactically correct. Hence cannot build AST\n");
            }
            if(correct){
                astRoot = constructAst(parseRoot);
                parseRoot = NULL;
                printAstTree(astRoot);
            }
        }
        else if(mode==4){
            int parseCount = 0;
            int astCount = 0;
            parseRoot = parserDriver(argc,argv);
            if(!correct){
                printf("Code is not syntactically correct. Hence cannot build AST\n");
            }
            if(correct){
                computeParseTreeSize(parseRoot, &parseCount);
                astRoot = constructAst(parseRoot);
                parseRoot = NULL;
                computeAstSize(astRoot, &astCount);
                int parseSize = parseCount*sizeof(node);
                int astSize = astCount*sizeof(astNode);
                printf("Parse Tree Number of node = %d        Allocated Memory = %d Bytes\n",parseCount, parseSize);
                printf("AST Number of node = %d        Allocated Memory = %d Bytes\n",astCount, astSize);
                double compression = (double)(((double)parseSize-(double)astSize)/(double)parseSize)*100;
                printf("Compression precentage = ((%d-%d)/%d)*100 = %0.2lf\n",parseSize,astSize,parseSize,compression);
            }
        }
        else if(mode==5){
            parseRoot = parserDriver(argc,argv); 
            if(!correct){
                printf("Code is not syntactically correct. Hence cannot build AST and cannot do semantic analysis\n");
            }
            if(correct){
                astRoot = constructAst(parseRoot);
                parseRoot = NULL;
                scopeRoot = scopeDriver(astRoot);
                printf("\nPrinting Symbol Table...\nA full scope is printed then all its child scopes and finally the sibling scopes. A scope is not splitted and is printed together.\n\n");
                printSymbolTable(scopeRoot,"root");
            }
        }
        else if(mode==6){
            parseRoot = parserDriver(argc,argv); 
            if(!correct){
                printf("\nCode is not syntactically correct. Hence cannot build AST and cannot do semantic analysis\n");
            }
            if(correct){
                astRoot = constructAst(parseRoot);
                parseRoot = NULL;
                scopeRoot = scopeDriver(astRoot);
                printf("\nPrinting Activation Record...\n%-20s\t%-100s\n","moduleName","Activation_recordSize (Includes Input and Output Parameters)");
                printActivation(scopeRoot);
            }
        }
        else if(mode==7){
            parseRoot = parserDriver(argc,argv); 
            if(!correct){
                printf("\nCode is not syntactically correct. Hence cannot build AST and cannot do semantic analysis\n");
            }
            if(correct){
                parrays=1;
                astRoot = constructAst(parseRoot);
                parseRoot = NULL;
                scopeRoot = scopeDriver(astRoot);
                printf("Printing Array list...\n%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t%-20s\t\n","module_name","scope","variable_name","static/dynamic","range","type");
                printSymbolTable(scopeRoot,"root");
                parrays=0;
            }
        }
        else if(mode==8){
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            start_time = clock();
            parseRoot = parserDriver(argc,argv); 
            if(!correct){
                printf("Code is not syntactically correct. Hence cannot build AST and cannot do semantic analysis\n");
            }
            if(correct){
                astRoot = constructAst(parseRoot);
                parseRoot = NULL;
                scopeRoot = scopeDriver(astRoot);
                checkSemantics(astRoot);
            }
            end_time = clock();
            if(correct&&!errFlag&&!semErr){
                printf("...........Passed: Input source code is semantically correct...........\n");
                printf("...........Code Compiles Successfully.............\n");
            }
            else if(correct){
                printf(".........Fail: Input source code is not semantically correct...........\n"); 
            }
            total_CPU_time  =  (double) (end_time - start_time);
            printf("Total number of ticks: %lf ticks...\n",total_CPU_time);
            total_CPU_time_in_seconds = (total_CPU_time / CLOCKS_PER_SEC);
            printf("Total Time Taken by the Compiler: ");
            printf("%lf seconds...\n\n",total_CPU_time_in_seconds);
        }
        else if(mode==9){
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            start_time = clock();
            parseRoot = parserDriver(argc,argv); 
            if(!correct){
                printf("Fail: Code is not syntactically correct. Hence cannot build AST and cannot do semantic analysis\n");
            }
            if(correct){
                astRoot = constructAst(parseRoot);
                parseRoot = NULL;
                scopeRoot = scopeDriver(astRoot);
                checkSemantics(astRoot);
            }
            if(correct&&!errFlag&&!semErr){
                end_time = clock();
		if(argc<3)
		  printf("Error: Missing Code Gen File Argument\n");
		else{
		        FILE *fp = fopen(argv[2],"w");
		        codeGen_driver(astRoot,fp);
		        printf("\n...........Input source code is semantically correct...........\n");
		        printf("...........Code Compiles Successfully.............\n");	
			printf("...........Passed: %s file generated successfully.............\n\n",argv[2]);
		}
            }
            else if(correct){
                end_time = clock();
                printf("\n...........Input source code is not semantically correct. Hence asm file generation cannot be done...........\n"); 
            }
            total_CPU_time  =  (double) (end_time - start_time);
            printf("Total number of ticks: %lf ticks...\n",total_CPU_time);
            total_CPU_time_in_seconds = (total_CPU_time / CLOCKS_PER_SEC);
            printf("Total Time Taken by the Compiler: ");
            printf("%lf seconds...\n\n",total_CPU_time_in_seconds);
        }
        else{
            printf("Invalid Option Number Entered...\n");
        }
    }
    //parserDriver(argc,argv);
}
