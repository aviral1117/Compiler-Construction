/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#include<stdio.h>
#include<stdlib.h>
#include "semantics.h"

int semErr = 0;
int whileAssign = 0;
int whileAvail = 0;

int null(astNode* root){
    if(root==NULL) return 1;
    else if(root->label==-7) return 1;
    else return 0;
}

void passUpIsDynamic(astNode* root, astNode* child){
    root->isDynamic |= child->isDynamic;
}

int isSafe(astNode* var){
    int l = var->label;
    if(l==-2&&var->symTableEntry==NULL){
        return 0;
    }
    else if(l==-1&&var->symTableEntry==NULL){
        return 0;
    }
    else if(l==index_id){
        astNode* varId = var->down->start;
        return isSafe(varId);
    }
    else if(l==id_var){
        astNode* varId = var->down->start;
        return isSafe(varId);
    }
    else if(l==array_var){
        astNode* varId = var->down->start;
        astNode* indx = var->down->end;
        return isSafe(varId)&&isSafe(indx);
    }
    else if(l==input_list){
        astNode* varId = var->down->start;
        int t = var->down->end->label;
        int flag = 1;
        if(t==dataType_array){
            astNode* rangeNode = var->down->end->down->start;
            astNode* low = rangeNode->down->start;
            astNode* high = rangeNode->down->end;
            flag = isSafe(low)&&isSafe(high);
        }
        return flag&&isSafe(varId);
    }
    else if(l==output_list_){
        astNode* varId = var->down->start;
        return isSafe(varId);
    }
    else if(l==id_list){
        astNode* varId = var->down->start;
        return isSafe(varId);
    }
    return 1;
}

int checkArrayDeclaration(astNode* root){
    //printf(">Array declarartion\n");
    //fflush(stdout);
    astNode* low = root->down->start;
    astNode* high = root->down->end;
    if(low->label==index_num&&high->label==index_num){
        int l = atoi(low->down->start->tokenInfo->lexeme);
        int r = atoi(high->down->start->tokenInfo->lexeme);
        if(l>r){
            printf("Line %d: Error as left index '%s' is greater than right index '%s' in array declaration\n", low->down->start->tokenInfo->lno, low->down->start->tokenInfo->lexeme, high->down->start->tokenInfo->lexeme);
            semErr |= 1;
            return 0;
        }
    }
    return 1;
    //printf("<Array declarartion\n");
    //fflush(stdout);
}

int handleConstId(astNode* var){
    //printf(">constId\n");
    //fflush(stdout);
    if(var->label==-2){
        var->type = function;
    }
    else if(var->label==-3){
        var->type = integer;
    }
    else if(var->label==value_num||var->label==value_minus_num||var->label==value_plus_num||var->label==num_var){
        astNode *varId = var->down->start;
        handleConstId(varId);
        var->type = integer;
    }
    else if(var->label==index_num){
        astNode *varId = var->down->start;
        handleConstId(varId);
        var->type = integer;
    }
    else if(var->label==-4){
        var->type = real;
    }
    else if(var->label==rnum_var){
        astNode* varId = var->down->start;
        handleConstId(varId);
        var->type = real;
    }
    else if(var->label==-5||var->label==-6){
        var->type = bool;
    }
    else if(var->label==bool_false||var->label==bool_true||var->label==value_true||var->label==value_false){
        astNode* varId = var->down->start;
        handleConstId(varId);
        var->type = bool;
    }
    else if(var->label==-1){
        int t = var->symTableEntry->type;
        if(t==0) var->type = integer;
        else if(t==1) var->type = real;
        else if(t==2) var->type = bool;
        else if(t==3){
            if(var->parent->label==array_var||(var->next!=NULL&&var->next->label==lvalueARRStmt_node)) var->type = integer;
            else var->type = int_array;
            if(var->symTableEntry->isStatic==0) var->isDynamic = 1;
            else if(var->type==integer&&(var->next->label==index_id||var->next->down->start->label==index_id)) var->isDynamic = 1;
        }
        else if(t==4){
            if(var->parent->label==array_var||(var->next!=NULL&&var->next->label==lvalueARRStmt_node)) var->type = real;
            else var->type = real_array;
            if(var->symTableEntry->isStatic==0) var->isDynamic = 1;
            else if(var->type==real&&(var->next->label==index_id||var->next->down->start->label==index_id)) var->isDynamic = 1;
        }
        else if(t==5){
            if(var->parent->label==array_var||(var->next!=NULL&&var->next->label==lvalueARRStmt_node)) var->type = bool;
            else var->type = bool_array;
            if(var->symTableEntry->isStatic==0) var->isDynamic = 1;
            else if(var->type==bool&&(var->next->label==index_id||var->next->down->start->label==index_id)) var->isDynamic = 1;
        }
    }
    else if(var->label==index_id){
        astNode* varId = var->down->start;
        handleConstId(varId);
        var->type = varId->type;
        if(var->type!=integer){
            printf("Line %d: Error as index '%s' should be of type integer\n", varId->tokenInfo->lno, varId->tokenInfo->lexeme);
            semErr |= 1;
            return 0;
        }
    }
    else if(var->label==id_var){
        astNode* varId = var->down->start;
        handleConstId(varId);
        var->type = varId->type;
        passUpIsDynamic(var,varId);
        if(var->isDynamic==1){
            varId->dynamicAction = dynamicTypeCompute;
        }
    }
    else if(var->label==array_var){
        astNode* varId = var->down->start;
        astNode* indx = var->down->end;
        handleConstId(varId);
        var->type = varId->type;
        passUpIsDynamic(var,varId);
        if(varId->symTableEntry->type!=int_array&&varId->symTableEntry->type!=real_array&&varId->symTableEntry->type!=bool_array){
            printf("Line %d: Error as non-array variable '%s' can't be indexed\n",varId->tokenInfo->lno, varId->tokenInfo->lexeme);
            semErr |= 1;
            return 0;
        }
        if(handleConstId(indx)==0) return 0;
        if(var->isDynamic==1){
            varId->dynamicAction = dynamicBoundCheck;
        }
        else{
            astNode* idx = varId->next->down->start;
            int low = varId->symTableEntry->a;
            int high = varId->symTableEntry->b;
            int indx = atoi(idx->tokenInfo->lexeme);
            if(low>indx||high<indx){
                printf("Line %d: Error as array index for '%s[%s]' is out of bounds\n", varId->tokenInfo->lno, varId->tokenInfo->lexeme, idx->tokenInfo->lexeme);
                semErr |= 1;
                return 0;
            }
        }
    }
    else if(var->label==input_list){
        astNode* varId = var->down->start;
        handleConstId(varId);
        var->type = varId->type;
        passUpIsDynamic(var,varId);
        if(var->isDynamic==1){
            varId->dynamicAction = dynamicDeclaration;
            astNode* rangeNode = var->down->end->down->start;
            astNode* low = rangeNode->down->start;
            astNode* high = rangeNode->down->end;
            if(!handleConstId(low)||!handleConstId(high)) return 0;
        }
        else if(var->type==int_array||var->type==real_array||var->type==bool_array){
            astNode* rangeNode = var->down->end->down->start;
            astNode* low = rangeNode->down->start;
            astNode* high = rangeNode->down->end;
            if(!handleConstId(low)||!handleConstId(high)||!checkArrayDeclaration(rangeNode)) return 0;
        }
    }
    else if(var->label==output_list_){
        astNode* varId = var->down->start;
        handleConstId(varId);
        var->type = varId->type;
        passUpIsDynamic(var, varId);
        if(var->type==int_array||var->type==real_array||var->type==bool_array){
            printf("Line %d: Error as output parameter '%s' is of array type\n",varId->tokenInfo->lno, varId->tokenInfo->lexeme);
            semErr |= 1;
            return 0;
        }
    }
    else if(var->label==id_list){
        astNode* varId = var->down->start;
        handleConstId(varId);
        var->type = varId->type;
        passUpIsDynamic(var,varId);
    }
    return 1;
    //printf("<constId\n");
    //fflush(stdout);
}

void assignmentCheck(astNode* root, int stmtType){
    //printf(">assignment Check\n");
    //fflush(stdout);
    if(root->symTableEntry->semFor==1){
        switch(stmtType){
            case assignmentStmt_node:{
                printf("Line %d: Error as loop iterator '%s' is reassigned a value in assignment statement\n", root->tokenInfo->lno, root->tokenInfo->lexeme);
                semErr |= 1;
                break;
            }
            case moduleReuseStmt_node:{
                printf("Line %d: Error as loop iterator '%s' is reassigned a value in function call\n", root->tokenInfo->lno, root->tokenInfo->lexeme);
                semErr |= 1;
                break;
            }
            case ioStmt_get:{
                printf("Line %d: Error as loop iterator '%s' is reassigned a value in I/O get statement\n", root->tokenInfo->lno, root->tokenInfo->lexeme);
                semErr |= 1;
                break;
            }
        } 
    }
    if(root->symTableEntry->semWhile==1){
        if(stmtType==assignmentStmt_node||stmtType==moduleReuseStmt_node||stmtType==ioStmt_get){
            int *tmp = root->symTableEntry->index;
            if(tmp[0]==1){
                tmp[0] = 0;
            }
        }
    }
    if(root->symTableEntry->semFunc==1){
        if(stmtType==assignmentStmt_node||stmtType==moduleReuseStmt_node||stmtType==ioStmt_get){
            root->symTableEntry->semFunc = 0;
        }
    }
    //printf("<assignment Check\n");
    //fflush(stdout);
}

void checkIoGet(astNode* root){
    //printf(">get I/O\n");
    //fflush(stdout);
    astNode* var = root->down->start;
    if(!isSafe(var)) return;
    handleConstId(var);
    passUpIsDynamic(root,var);
    int t = root->down->start->type;
    if(t==int_array||t==real_array||t==bool_array){
        //printf("Line %d: Error as input variable can't be of array type\n",root->tokenInfo->lno);
        //semErr |= 1;
    }
    assignmentCheck(var,ioStmt_get);
    //printf("<get I/O\n");
    //fflush(stdout);
}

void checkIoPrint(astNode* root){
    //printf(">print I/O\n");
    //fflush(stdout);
    astNode* var = root->down->start;
    if(!isSafe(var)) return;
    handleConstId(var);
    passUpIsDynamic(root,var);
    if(var->label==id_var&&(var->type==int_array||var->type==real_array||var->type==bool_array)){
        //printf("Line %d: Error as array variable can't be printed\n",root->tokenInfo->lno);
        //semErr |= 1;
    }
    //printf("<print I/O\n");
    //fflush(stdout);
}

int checkExpression(astNode* root){
    //printf(">expression\n");
    //fflush(stdout);
    if(null(root)) return 1;
    int l = root->label;
    if(l==id_var||l==array_var||l==num_var||l==rnum_var||l==bool_true||l==bool_false){
        return isSafe(root)&&handleConstId(root);
    }
    else if(l==expression_node){
        astNode* lexpr = root->down->start;
        astNode* rexpr = root->down->end;
        if(!checkExpression(lexpr)||!checkExpression(rexpr)) return 0;
        if(lexpr->type==bool&&rexpr->type==bool){
            root->type = bool;
        }
        else{
            printf("Line %d: Error as logical operation can only be applied on boolean operands\n", root->tokenInfo->lno);
            semErr |= 1;
            return 0;
        }
        passUpIsDynamic(root,lexpr);
        passUpIsDynamic(root,rexpr);
    }
    else if(l==expterm_node){
        astNode* lexpr = root->down->start;
        astNode* rexpr = root->down->end;
        if(!checkExpression(lexpr)||!checkExpression(rexpr)) return 0;
        if(lexpr->type==integer&&rexpr->type==integer){
            root->type = bool;
        }
        else if(lexpr->type==real&&rexpr->type==real){
            root->type = bool;
        }
        else{
            printf("Line %d: Error as relational operation can only be applied on both integer or both real operands\n", root->tokenInfo->lno);
            semErr |= 1;
            return 0;
        }
        passUpIsDynamic(root,lexpr);
        passUpIsDynamic(root,rexpr);
    }
    else if(l==expfactor_node||l==term_node){
        astNode* lexpr = root->down->start;
        astNode* rexpr = root->down->end;
        if(!checkExpression(lexpr)||!checkExpression(rexpr)) return 0;
        if(lexpr->type==integer&&rexpr->type==integer){
            root->type = integer;
        }
        else if(lexpr->type==real&&rexpr->type==real){
            root->type = real;
        }
        else{
            printf("Line %d: Error as arithematic operator can only be applied on both integer or both real operands\n", root->tokenInfo->lno);
            semErr |= 1;
            return 0;
        }
        passUpIsDynamic(root,lexpr);
        passUpIsDynamic(root,rexpr);
    }
    else if(l==u_node){
        astNode* expr = root->down->start;
        if(!checkExpression(expr)) return 0;
        if(expr->type!=integer&&expr->type!=real){
            printf("Line %d: Error as signed expressions should be of both integer or both real type\n", root->tokenInfo->lno);
            semErr |= 1;
            return 0;
        }
        else{
            root->type = expr->type;
        }
        passUpIsDynamic(root,expr);
    }
    else return 0;
    checkExpression(root->list);
    //printf("<expression\n");
    //fflush(stdout);
    return 1;
}

void checkAssignment(astNode* root){
    //printf(">assignment\n");
    //fflush(stdout);
    int lflag, iflag, rflag;
    astNode* var = root->down->start;
    astNode* expr = root->down->end->down->end;
    lflag = isSafe(var);
    if(lflag) handleConstId(var);
    if(var->next->label==lvalueARRStmt_node){
        astNode* idx = root->down->end->down->start;
        iflag = isSafe(idx);
        if(iflag) handleConstId(idx);
        if(lflag&&var->symTableEntry->type!=int_array&&var->symTableEntry->type!=real_array&&var->symTableEntry->type!=bool_array){
            printf("Line %d: Error as non-array variable '%s' can't be indexed\n",var->tokenInfo->lno, var->tokenInfo->lexeme);
            semErr |= 1;
        }
        else if(lflag&&iflag&&var->isDynamic==1){
            var->dynamicAction = dynamicBoundCheck;
            passUpIsDynamic(root,var);
        }
        else if(lflag&&iflag&&var->isDynamic==0){
            int low = var->symTableEntry->a;
            int high = var->symTableEntry->b;
            int indx = atoi(idx->down->start->tokenInfo->lexeme);
            if(indx<low||indx>high){
                printf("Line %d: Error as '%s[%s]' is out of bounds\n",var->tokenInfo->lno,var->tokenInfo->lexeme,idx->down->start->tokenInfo->lexeme);
                semErr |= 1;
            }
        }
        else if(lflag&&iflag){
            assignmentCheck(var,assignmentStmt_node);
        }
        rflag = checkExpression(expr);
        passUpIsDynamic(expr->parent,expr);
        passUpIsDynamic(root,expr->parent);
        if(lflag&&iflag&&rflag&&var->type!=expr->type){
            printf("Line %d: Error due to type mismatch b/w left and right expression\n",var->next->tokenInfo->lno);
            semErr |= 1;
        }
    }
    else{
        if(lflag){
            assignmentCheck(var,assignmentStmt_node);
        }
        rflag = checkExpression(expr);
        if(lflag&&rflag&&var->type!=expr->type){
            printf("Line %d: Error due to type mismatch b/w left and right expression\n",var->next->tokenInfo->lno);
            semErr |= 1; 
        }
        else if(lflag&&rflag&&var->type==expr->type&&(var->isDynamic==1||expr->isDynamic==1)){
            if(var->isDynamic==1){
                var->dynamicAction = dynamicTypeCompute;
                passUpIsDynamic(root,var);
            }
            if(expr->isDynamic==1){
                passUpIsDynamic(expr->parent,expr);
                passUpIsDynamic(root,expr->parent);
            }
            root->dynamicAction = dynamicTypeCheck;
        }
        else if(lflag&&rflag&&var->type==expr->type&&(var->type==int_array||var->type==real_array||var->type==bool_array)){
            astNode* arrId = expr->down->start;
            int la = var->symTableEntry->a, lb = var->symTableEntry->b;
            int ra = arrId->symTableEntry->a, rb = arrId->symTableEntry->b;
            if((la!=ra||lb!=rb)){
                printf("Line %d: Error due to type mismatch b/w left and right expression\n",var->next->tokenInfo->lno);
                semErr |= 1; 
            }
        }
    }
    //printf("<assignment\n");
    //fflush(stdout);
}

void checkFunctionCall(astNode* root){
    //printf(">function call\n");
    //fflush(stdout);
    astNode* outArg = root->down->start;
    astNode* funcId = root->down->start->next;
    astNode* inArg = root->down->end;
    if(!isSafe(funcId)){
        printf("Line %d: Error as function name '%s' is neither declared nor defined above\n",funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
        semErr |= 1;
        return;
    }
    handleConstId(funcId);
    if(funcId->symTableEntry->isDefined==0){
        printf("Line %d: Error as function name '%s' is not defined\n",funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
        semErr |= 1;
        return;
    }
    else if(funcId->donotproceed==2){
        printf("Line %d: Error as function name '%s' is a recursive call\n",funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
        semErr |= 1;
        return;
    }
    inout* fcur = funcId->symTableEntry->out;
    if(null(outArg)&&fcur){
        printf("Line %d: Error in '%s' as actual output parameters are absent\n", funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
        semErr |= 1;
    }
    else if(!null(outArg)&&!fcur){
        printf("Line %d: Error in '%s' as function does not return any value\n", funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
        semErr |= 1;
        astNode* cur = outArg->down->start;
        while(!null(cur)){
            if(!isSafe(cur)){
                cur = cur->list;
                continue;
            }
            handleConstId(cur);
            astNode* var = cur->down->start;
            assignmentCheck(var,moduleReuseStmt_node);
            cur = cur->list;
        }
    }
    else if(!null(outArg)){
        astNode* cur = outArg->down->start;
        while(!null(cur)&&fcur){
            if(!isSafe(cur)){
                cur = cur->list;
                fcur = fcur->next;
                continue;
            }
            handleConstId(cur);
            astNode* var = cur->down->start;
            symhashnode* fvar = fcur->para;
            if(cur->type==int_array||cur->type==real_array||cur->type==bool_array){
                printf("Line %d: Error in '%s' as actual output parameter '%s' is of array type\n",var->tokenInfo->lno, funcId->tokenInfo->lexeme, var->tokenInfo->lexeme);
                semErr |= 1;
            }
            else if(var->type!=fvar->type){
                printf("Line %d: Error in '%s' as actual output parameter '%s' and formal output parameter '%s' do not have same type\n", var->tokenInfo->lno, funcId->tokenInfo->lexeme, var->tokenInfo->lexeme, fvar->value);
                semErr |= 1;
            }
            assignmentCheck(var,moduleReuseStmt_node);
            cur = cur->list;
            fcur = fcur->next;
        }
        if(!null(cur)){
            printf("Line %d: Error in '%s' as more output arguments provided than expected\n", funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
            semErr |= 1;
            while(!null(cur)){
                if(!isSafe(cur)){
                    cur = cur->list;
                    continue;
                }
                handleConstId(cur);
                astNode* var = cur->down->start;
                assignmentCheck(var,moduleReuseStmt_node);
                cur = cur->list;
            }
        }
        else if(fcur){
            printf("Line %d: Error in '%s' as less output arguments provided than expected\n", funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
            semErr |= 1;
        }
    }
    astNode* cur = inArg;
    fcur = funcId->symTableEntry->in;
    while(!null(cur)&&fcur){
        if(!isSafe(cur)){
            cur = cur->list;
            fcur = fcur->next;
            continue;
        }
        handleConstId(cur);
        astNode* var = cur->down->start;
        symhashnode* fvar = fcur->para;
        if(var->type!=fvar->type){
            printf("Line %d: Error in '%s' as actual input parameter '%s' and formal input parameter '%s' do not have same type\n", var->tokenInfo->lno, funcId->tokenInfo->lexeme, var->tokenInfo->lexeme, fvar->value);
            semErr |= 1;
        }
        else if((var->type==int_array||var->type==real_array||var->type==bool_array)&&var->isDynamic==0&&fvar->isStatic==1){
            int al = var->symTableEntry->a, ar = var->symTableEntry->b;
            int fl = fvar->a, fr = fvar->b;
            if(fl!=al||fr!=ar){
                printf("Line %d: Error in '%s' as actual input parameter '%s' and formal input parameter '%s' do not have same type\n", var->tokenInfo->lno, funcId->tokenInfo->lexeme, var->tokenInfo->lexeme, fvar->value);
                semErr |= 1;    
            }
        }
        else if(cur->isDynamic==1){
            var->dynamicAction = dynamicTypeCompute;
            passUpIsDynamic(root,cur);
            root->dynamicAction = dynamicTypeCheck;
        }
        cur = cur->list;
        fcur = fcur->next;
    }
    if(!null(cur)){
        printf("Line %d: Error in '%s' as more arguments provided than expected\n", funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
        semErr |= 1;
        while(!null(cur)){
            if(!isSafe(cur)){
                cur = cur->list;
                continue;
            }
            handleConstId(cur);
            cur = cur->list;
        }
    }
    else if(fcur){
        printf("Line %d: Error in '%s' as less arguments provided than expected\n", funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
        semErr |= 1;
    }
    //printf("<function call\n");
    //fflush(stdout);
}

void checkDeclaration(astNode* root){
    //printf(">declarartion\n");
    //fflush(stdout);
    astNode* varList = root->down->start;
    astNode* cur = varList;
    while(!null(cur)){
        if(!isSafe(cur)){
            cur = cur->list;
            continue;
        }
        handleConstId(cur);
        astNode* var = cur->down->start;
        if(cur->isDynamic==1){
            var->dynamicAction = dynamicDeclaration;
            passUpIsDynamic(root,cur);
        }
        cur = cur->list;
    }
    int t = root->down->end->label;
    if(t==dataType_array){
        astNode* rangeNode = root->down->end->down->start;
        astNode* low = rangeNode->down->start;
        astNode* high = rangeNode->down->end;
        if(isSafe(low)) handleConstId(low);
        if(isSafe(high)) handleConstId(high);
        checkArrayDeclaration(rangeNode);
    }
    //printf("<declarartion\n");
    //fflush(stdout);
}

void checkSwitch(astNode* root){
    //printf(">switch\n");
    //fflush(stdout);
    astNode* switchId = root->down->start;
    astNode* caseList = root->down->start->next;
    astNode* defaultNode = root->down->end;
    if(switchId->donotproceed==4){
        printf("Line %d: Error as variable '%s' is neither of integer or boolean type\n", switchId->tokenInfo->lno, switchId->tokenInfo->lexeme);
        semErr |= 1;
        return;
    }
    int flag = isSafe(switchId);
    if(flag) handleConstId(switchId);
    /*if(flag&&switchId->type!=integer&&switchId->type!=bool){
        printf("Line %d: Error as variable- %s -is neither of integer or boolean type\n", switchId->tokenInfo->lno, switchId->tokenInfo->lexeme);
        semErr |= 1;
    }*/
    astNode* cur = caseList;
    while(!null(cur)){
        astNode* caseVal = cur->down->start;
        astNode* stmts = cur->down->end;
        handleConstId(caseVal);
        astNode* var = caseVal->down->start;
        if(flag&&switchId->type==integer&&caseVal->type!=integer){
            printf("Line %d: Error as value in case statement is not of integer type\n", var->tokenInfo->lno);
            semErr |= 1;
        }
        else if(flag&&switchId->type==bool&&caseVal->type!=bool){
            printf("Line %d: Error as value in case statement is not of boolean type\n", var->tokenInfo->lno);
            semErr |= 1;
        }
        handleStatements(stmts);
        cur = cur->list;
    }
    if(flag&&switchId->type==integer&&defaultNode->label==-7){
        printf("Line %d: Error as default statement is missing in switch construct\n", root->endLno);
        semErr |= 1;
    }
    else if(flag&&switchId->type==bool&&defaultNode->label!=-7){
        printf("Line %d: Error as there is default statement in switch construct\n", defaultNode->tokenInfo->lno);
        semErr |= 1;
    }
    if(defaultNode->label!=-7){
        astNode* stmts = defaultNode->down->start;
        handleStatements(stmts);
    }
    //printf("<switch\n");
    //fflush(stdout);
}

void checkFor(astNode* root){
    //printf(">for\n");
    //fflush(stdout);
    astNode* iterId = root->down->start;
    astNode* low = root->down->start->next->down->start;
    astNode* high = root->down->start->next->down->end;
    astNode* body = root->down->end;
    int flag = isSafe(iterId);
    if(flag) handleConstId(iterId);
    if(flag&&iterId->type!=integer){
        printf("Line %d: Error as for loop iterator '%s' is not of integer type\n", iterId->tokenInfo->lno, iterId->tokenInfo->lexeme);
        semErr |= 1;
    }
    handleConstId(low);
    handleConstId(high);
    int l = atoi(low->tokenInfo->lexeme);
    int r = atoi(high->tokenInfo->lexeme);
    if(l<=0){
        //printf("Line %d: Error as range in for should comprise of positive integers\n", low->tokenInfo->lno);
        //semErr |= 1;
    }
    if(r<=0){
        //printf("Line %d: Error as range in for should comprise of positive integers\n", high->tokenInfo->lno);
        //semErr |= 1;
    }
    if(l>r){
        printf("Line %d: Error as '%s' is greater than '%s' in for loop\n", low->tokenInfo->lno, low->tokenInfo->lexeme, high->tokenInfo->lexeme);
        semErr |= 1;
    }
    int curFor;
    if(flag){
        curFor = iterId->symTableEntry->semFor;
        iterId->symTableEntry->semFor = 1;
    }
    handleStatements(body);
    if(flag){
        iterId->symTableEntry->semFor = curFor;
    }
    //printf("<for\n");
    //fflush(stdout);
}

queue* createQueue(){
    queue* q = (queue*)malloc(sizeof(queue));
    q->head = NULL;
    q->tail = NULL;
    q->cur = NULL;
    q->size = 0;
}

void pushQueue(queue* q, astNode* root){
    //printf(">push queue\n");
    //fflush(stdout);
    int* indx = root->symTableEntry->index;
    int isWhile = root->symTableEntry->semWhile;
    entry* tmp = (entry*)malloc(sizeof(entry));
    tmp->symNode = root->symTableEntry;
    tmp->oldWhile = isWhile;
    tmp->index = indx;
    tmp->next = NULL;
    if(q->head==NULL){
        q->head = tmp;
        q->tail = tmp;
    }
    else{
        q->tail->next = tmp;
        q->tail = tmp;
    }
    q->cur = q->head;
    q->size++;
    //printf("<push queue\n");
    //fflush(stdout);
}

void popQueue(queue* q){
    //printf(">pop queue\n");
    //fflush(stdout);
    if(q->head==NULL){
        //printf("Underflow\n");
        return;
    }
    entry* tmp = q->head;
    q->head = tmp->next;
    if(q->head==NULL){
        q->tail = NULL;
    }
    free(tmp);
    q->cur = q->head;
    q->size--;
    //printf("<pop queue\n");
    //fflush(stdout);
}

void whileUtil(astNode* root, queue* q, int mode){
    //printf(">while util\n");
    //fflush(stdout);
    if(null(root)) return;
    astNode* cur = root->down->start;
    while(cur){
        whileUtil(cur,q,mode);
        cur = cur->next;
    }
    if(root->label==-1&&isSafe(root)){
        switch(mode){
            case 0:{
                //printf("mode %d current: %s\n",mode,root->tokenInfo->lexeme);
                pushQueue(q,root);
                break;
            }
            case 1:{
                //printf("mode %d current: %s\n",mode,root->tokenInfo->lexeme);
                int isWhile = root->symTableEntry->semWhile;
                if(isWhile!=3){
                    root->symTableEntry->semWhile = 3;
                    int *tmp = (int*)malloc(sizeof(int));
                    tmp[0] = -1;
                    root->symTableEntry->index = tmp;
                }
                break;
            }
            case 2:{
                //printf("mode %d current: %s\n",mode,root->tokenInfo->lexeme);
                root->symTableEntry->semWhile = 1;
                int *tmp = root->symTableEntry->index;
                tmp[0] = 1;
                whileAvail = 1;
                break;
            }
            case 3:{
                //printf("mode %d current: %s\n",mode,root->tokenInfo->lexeme);
                int *tmp = root->symTableEntry->index;
                if(tmp[0]==0){
                    whileAssign = 1;
                }
                break;
            }
            case 4:{
                //printf("mode %d current: %s\n",mode,q->cur->symNode->value);
                int isWhile = root->symTableEntry->semWhile;
                int* oldIndx = q->cur->index;
                if(isWhile!=4){
                    root->symTableEntry->semWhile = 4;
                    int* tmp = root->symTableEntry->index;
                    if(oldIndx!=NULL&&oldIndx[0]==1&&tmp[0]==0){
                        oldIndx[0] = 0;
                    }
                    root->symTableEntry->index = oldIndx;
                    free(tmp);
                }
                q->cur = q->cur->next;
                break;
            }
            case 5:{
                //printf("mode %d current: %s\n",mode,root->tokenInfo->lexeme);
                int oldWhile = q->head->oldWhile;
                root->symTableEntry->semWhile = oldWhile;
                popQueue(q);
                break;
            }
        }
    }
    whileUtil(root->list,q,mode);
    //printf("<while util\n");
    //fflush(stdout);
}

void checkWhile(astNode* root){
    //printf(">while\n");
    //fflush(stdout);
    astNode* condition = root->down->start;
    astNode* body = root->down->end;
    int flag = checkExpression(condition);
    if(flag&&condition->type!=bool){
        printf("Line %d: Error as condtion in while loop is not of boolean type\n", root->tokenInfo->lno);
        semErr |= 1;
    }
    queue* q = createQueue();
    /*mode 0: save old state
    mode 1: initialize new state
    mode 2: set new state
    mode 3: check for any set value becoming 0
    mode 4: get old state back
    mode 5: destroy queue*/
    whileUtil(condition,q,0); 
    whileUtil(condition,q,1);
    whileAvail = 0;
    whileUtil(condition,q,2);
    int idAvail = whileAvail;
    handleStatements(body);
    whileAssign = 0;
    whileUtil(condition,q,3);
    if(whileAssign==0&&idAvail==1){
        printf("Line %d: Error as no variable in condition of while loop is being assigned a value in the body of while loop\n", root->endLno);
        semErr |= 1;
    }
    q->cur = q->head;
    whileUtil(condition,q,4);
    whileUtil(condition,q,5);
    free(q);
    //printf("<while\n");
    //fflush(stdout);
}

void handleStatements(astNode* body){
    //printf(">statements\n");
    //fflush(stdout);
    astNode* stmt = body;
    while(!null(stmt)){
      int l = stmt->label;
        switch(l){
            case ioStmt_get:{
                checkIoGet(stmt);
                break;
            }
            case ioStmt_print:{
                checkIoPrint(stmt);
                break;
            }
            case assignmentStmt_node:{
                checkAssignment(stmt);
                break;
            }
            case moduleReuseStmt_node:{
                checkFunctionCall(stmt);
                break;
            }
            case declareStmt_node:{
                checkDeclaration(stmt);
                break;
            }
            case switchStmt_node:{
                checkSwitch(stmt);
                break;
            }
            case forStmt_node:{
                checkFor(stmt);
                break;
            }
            case whileStmt_node:{
                checkWhile(stmt);
                break;
            }
        }
        stmt = stmt->list;
    } 
    //printf("<statements\n");
    //fflush(stdout); 
}

void handleFunctions(astNode* funcDef){
    //printf(">functions\n");
    //fflush(stdout);
    astNode* cur = funcDef;
    while(!null(cur)){
        astNode* funcId = cur->down->start;
        astNode* funcInput = cur->down->start->next;
        astNode* funcOutput = cur->down->end->prev;
        astNode* body = cur->down->end;
        handleConstId(funcId);
        if(cur->donotproceed==1){
            printf("Line %d: Error as this is overloaded definition for function '%s'\n",funcId->tokenInfo->lno, funcId->tokenInfo->lexeme);
            semErr |= 1;
            cur = cur->list;
            continue;
        }
        astNode* fcur = funcInput;
        while(!null(fcur)){
            if(!isSafe(fcur)){
                fcur = fcur->list;
                continue;
            }
            handleConstId(fcur);
            fcur = fcur->list;
        }
        if(!null(funcOutput)){
            fcur = funcOutput;
            while(!null(fcur)){
                if(!isSafe(fcur)){
                    fcur = fcur->list;
                    continue;
                }
                astNode* var = fcur->down->start;
                handleConstId(fcur);
                var->symTableEntry->semFunc = 1;
                fcur = fcur->list;
            }
        }
        handleStatements(body);
        if(!null(funcOutput)){
            fcur = funcOutput;
            while(!null(fcur)){
                if(!isSafe(fcur)){
                    fcur = fcur->list;
                    continue;
                }
                astNode* var = fcur->down->start;
                if(var->symTableEntry->semFunc==1){
                    printf("Line %d: Error in '%s' as output parameter '%s' is not assigned a value in function body\n", cur->endLno, funcId->tokenInfo->lexeme, var->tokenInfo->lexeme);
                    semErr |= 1;
                }
                var->symTableEntry->semFunc = -1;
                fcur = fcur->list;
            }
        }
        cur = cur->list;
    }
    //printf("<functions\n");
    //fflush(stdout);
}

void checkSemantics(astNode* root){
    //printf(">semantics\n");
    //fflush(stdout);
    astNode* funcDec = root->down->start;
    astNode* funcDef1 = root->down->start->next;
    astNode* funcDriver = root->down->end->prev;
    astNode* funcDef2 = root->down->end;
    handleFunctions(funcDef1);
    handleStatements(funcDriver->down->start);
    handleFunctions(funcDef2);
    //printf("<semantics\n");
    //fflush(stdout);
}