/* # Group 1

    Aviral Sethi : 2016B3A70532P
    SANJEET MALHOTRA : 2016B4A70601P
    PRIYANKA VERMA : 2016B3A70492P
    ANIRUDH VIJAY : 2016B3A70525P
    
*/
#include "ast.h"
#include<stdio.h>
#include<stdlib.h>

char* wordLabel[] = {"program_node","moduleDeclaration_node","module_node","dataType_int","dataType_real","dataType_bool","dataType_array",
"ioStmt_get","ioStmt_print","bool_true","bool_false","num_var","rnum_var","array_var","assignmentStmt_node","lvalueIDStmt_node",
"lvalueARRStmt_node","index_id","index_num","moduleReuseStmt_node","optional_node","input_list","output_list","id_list","expression_node",
"logicalOp_and","logicalOp_or","expterm_node","expterm1_node","relationalOp_NT","relationalOp_LT","relationalOp_LE","relationalOp_GT",
"relationalOp_GE","relationalOp_EQ","relationalOp_NE","expfactor_node","op1_plus","op1_minus","term_node","op2_mul","op2_div","u_node",
"unary_plus","unary_minus","u1_node","declareStmt_node","switchStmt_node","value_num","value_true","value_false","value_plus_num",
"value_minus_num","forStmt_node","whileStmt_node","range1_node","range2_node","default_node","driverModule_node","type_int","type_real",
"type_bool","caseStmt_node","id_var", "---"};

astNode* createAstNode(Label label, astNode* parent, astNode* next, astNode* prev, token* tkn){
    astNode* nd = (astNode*)malloc(sizeof(astNode));
    nd->parent = parent;
    nd->next = next;
    nd->prev = prev;
    nd->label = label;
    nd->list = NULL;
    nd->down = createSubTree(NULL,NULL,0);
    nd->tokenInfo = tkn;
    nd->left = NULL;
    nd->right = NULL;
    nd->startLno = -1;
    nd->endLno = -1;
    nd->symTableEntry = NULL;
    nd->donotproceed = -1;
    nd->type = -1;
    nd->dynamicAction = -1;
    nd->isDynamic = 0;
    return nd;
}

subTree* createSubTree(astNode* start, astNode* end, int numOfNodes){
    subTree* sub = (subTree*)malloc(sizeof(subTree));
    sub->start= start;
    sub->end = end;
    sub->numOfNodes = numOfNodes;
    return sub;
}

subTree* populateSubTree(subTree* sub,astNode* nd){
    if(nd==NULL) return sub;
    else if(sub->start==NULL){
        sub->start = nd;
        sub->end = nd;
        nd->prev = NULL;
    }
    else{
        sub->end->next = nd;
        nd->prev = sub->end;
        sub->end = sub->end->next;
    }
    nd->next = NULL;
    sub->numOfNodes++;
    return sub;
}

astNode* insertAtBegin(astNode* nd,astNode* list){
    if(nd==NULL){
        return list;
    }
    else{
        nd->list = list;
        return nd;
    }
}

void destroyNode(node* pnode){
    if(pnode->parent==NULL){
        free(pnode->children);
        free(pnode);
        return;
    }
    else if(pnode->parent->down==pnode){
        pnode->parent->down = pnode->right;
        if(pnode->right) pnode->right->left = NULL;
    }
    else{
        pnode->left->right = pnode->right;
        if(pnode->right) pnode->right->left = pnode->left;
    }
    free(pnode->children);
    free(pnode);
}

void setParent(astNode* nd){
    astNode* cur = nd->down->start;
    while(cur){
        cur->parent = nd;
        astNode* list_cur = cur->list;
        while(list_cur){
            list_cur->parent = nd;
            list_cur = list_cur->list;
        }
        cur = cur->next;
    }
}

void toParentBuffer(node* pnode,astNode* nd){
    subTree* sub = pnode->parent->children;
    if(nd==NULL) return;
    else if(sub->start==NULL){
        sub->start = nd;
        sub->end = nd;
        nd->left = NULL;
    }
    else{
        sub->end->right = nd;
        nd->left = sub->end;
        sub->end = nd;
    }
    nd->right = NULL;
    sub->numOfNodes++;
}

void astInh(node* pnode){
    int rule = pnode->ruleNo;
    switch(rule){
        case 60:{
            pnode->inh = pnode->children->start;
            break;
        }
        case 61:{
            token* tk = pnode->children->start->tokenInfo;
            astNode* nd = createAstNode(expression_node,NULL,NULL,NULL,tk);
            pnode->inh = nd;
            nd->down = populateSubTree(nd->down,pnode->parent->inh);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            setParent(nd);
            astNode* tmp = pnode->children->start;
            pnode->children->start = tmp->right;
            tmp->right->left = NULL;
            pnode->children->numOfNodes--;
            free(tmp->down);
            free(tmp);
            break;
        }
        case 75:{
            pnode->inh = pnode->children->start;
            break;
        }
        case 76:{
            token* tk = pnode->children->start->tokenInfo;
            astNode* nd = createAstNode(expfactor_node,NULL,NULL,NULL,tk);
            pnode->inh = nd;
            nd->down = populateSubTree(nd->down,pnode->parent->inh);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            setParent(nd);
            astNode* tmp = pnode->children->start;
            pnode->children->start = tmp->right;
            tmp->right->left = NULL;
            pnode->children->numOfNodes--;
            free(tmp->down);
            free(tmp);
            break;
        }
        case 80:{
            pnode->inh = pnode->children->start;
            break;
        }
        case 81:{
            token* tk = pnode->children->start->tokenInfo;
            astNode* nd = createAstNode(term_node,NULL,NULL,NULL,tk);
            pnode->inh = nd;
            nd->down = populateSubTree(nd->down,pnode->parent->inh);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            setParent(nd);
            astNode* tmp = pnode->children->start;
            pnode->children->start = tmp->right;
            tmp->right->left = NULL;
            pnode->children->numOfNodes--;
            free(tmp->down);
            free(tmp);
            break;
        }
    }
}

astNode* astSyn(node* pnode){
    astNode* root = NULL;
    int rule = pnode->ruleNo;
    switch(rule){
        case 1:{
            astNode* nd = createAstNode(program_node,NULL,NULL,NULL,NULL);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            nd->down = populateSubTree(nd->down,pnode->children->start->right->right);
            nd->down = populateSubTree(nd->down,pnode->children->start->right->right->right);
            setParent(nd);
            destroyNode(pnode);
            root = nd;
            break;
        }
        case 2:{
            astNode* nd = insertAtBegin(pnode->children->start,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 3:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 4:{
            astNode* nd = createAstNode(moduleDeclaration_node,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-2,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 5:{
            astNode* nd = insertAtBegin(pnode->children->start,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 6:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 7:{
            astNode* nd = createAstNode(driverModule_node,NULL,NULL,NULL,NULL);
            //nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down, pnode->children->start->down->start);
            nd->startLno = pnode->children->start->startLno;
            nd->endLno = pnode->children->start->endLno;
            toParentBuffer(pnode,nd);
            setParent(nd);
            astNode* tmp = pnode->children->start;
            pnode->children->start = NULL;
            pnode->children->end = NULL;
            pnode->children->numOfNodes--;
            free(tmp->down);
            free(tmp);
            destroyNode(pnode);
            break;
        }
        case 8:{
            astNode* nd = createAstNode(module_node,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-2,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            //nd->down = populateSubTree(nd->down,pnode->children->start->right->right);
            nd->down = populateSubTree(nd->down,pnode->children->start->right->right->down->start);
            nd->startLno = pnode->children->start->right->right->startLno;
            nd->endLno = pnode->children->start->right->right->endLno;
            toParentBuffer(pnode,nd);
            setParent(nd);
            astNode* temp = pnode->children->end;
            pnode->children->end = temp->left;
            pnode->children->numOfNodes--;
            free(temp->down);
            free(temp);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 9:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 10:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 11:
        case 12:{
            astNode* nd = createAstNode(input_list,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd = insertAtBegin(nd,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 13:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 14:
        case 15:{
            astNode* nd = createAstNode(output_list_,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd = insertAtBegin(nd,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 16:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 17:{
            astNode* nd = createAstNode(dataType_int,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 18:{
            astNode* nd = createAstNode(dataType_real,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 19:{
            astNode* nd = createAstNode(dataType_bool,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 20:{
            astNode* nd = createAstNode(dataType_array,NULL,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 21:{
            astNode* nd = createAstNode(type_int,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 22:{
            astNode* nd = createAstNode(type_real,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 23:{
            astNode* nd = createAstNode(type_bool,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 24:{
            //toParentBuffer(pnode,pnode->children->start);
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->startLno = pnode->down->lex->lno;
            nd->endLno = pnode->down->right->lex->lno;
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 25:{
            astNode* nd = insertAtBegin(pnode->children->start,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 26:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 32:{
            astNode* nd = createAstNode(ioStmt_get,NULL,NULL,NULL,pnode->down->lex);
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->right->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 33:{
            astNode* nd = createAstNode(ioStmt_print,NULL,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 34:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 35:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 36:{
            astNode* nd = createAstNode(bool_true,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-5,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 37:{
            astNode* nd = createAstNode(bool_false,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-6,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 38:{
            int l = pnode->children->start->label;
            astNode* nd = NULL;
            switch(l){
                case -7:{
                    nd = createAstNode(id_var,NULL,NULL,NULL,NULL);
                    break;
                }
                default:{
                    nd = createAstNode(array_var,NULL,NULL,NULL,NULL);
                }
            }
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 39:{
            astNode* nd = createAstNode(num_var,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-3,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 40:{
            astNode* nd = createAstNode(rnum_var,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-4,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 41:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 42:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 43:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 44:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 45:{
            astNode* nd = createAstNode(assignmentStmt_node,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 46:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 47:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 48:{
            astNode* nd = createAstNode(lvalueIDStmt_node,NULL,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 49:{
            astNode* nd = createAstNode(lvalueARRStmt_node,NULL,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 50:{
            astNode* nd = createAstNode(index_num,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-3,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 51:{
            astNode* nd = createAstNode(index_id,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 52:{
            astNode* nd = createAstNode(moduleReuseStmt_node,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-2,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,tmp);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 53:{
            astNode* nd = createAstNode(optional_node,NULL,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 54:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 55:
        case 56:{
            astNode* nd = createAstNode(id_list,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            nd = insertAtBegin(nd,pnode->children->start);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 57:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        /*-------------------------------------------------------------------------------------------*/
        case 58:
        case 59:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 60:{
            toParentBuffer(pnode,pnode->children->end);
            destroyNode(pnode);
            break;
        }
        case 61:{
            toParentBuffer(pnode,pnode->children->end);
            destroyNode(pnode);
            break;
        }
        case 62:{
            toParentBuffer(pnode,pnode->parent->inh);
            destroyNode(pnode);
            break;
        }
        case 63:{
            astNode* nd = createAstNode(logicalOp_and,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 64:{
            astNode* nd = createAstNode(logicalOp_or,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 65:{
            int l = pnode->children->start->right->label;
            switch(l){
                case -7:{
                    toParentBuffer(pnode,pnode->children->start);
                    break;
                }
                default:{
                    token* tk = pnode->children->start->right->tokenInfo;
                    astNode* nd = createAstNode(expterm_node,NULL,NULL,NULL,tk);
                    nd->down = populateSubTree(nd->down,pnode->children->start);
                    nd->down = populateSubTree(nd->down,pnode->children->start->right->down->start);
                    toParentBuffer(pnode,nd);
                    setParent(nd);
                    astNode* tmp = pnode->children->start->right;
                    free(tmp->down);
                    free(tmp);
                    break;
                }
            }
            destroyNode(pnode);
            break;
        }
        case 66:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 67:{
            token* tk = pnode->children->start->tokenInfo;
            astNode* nd =  createAstNode(expterm1_node,NULL,NULL,NULL,tk);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            astNode* tmp = pnode->children->start;
            free(tmp->down);
            free(tmp);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode);
            break;
        }
        case 68:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 69:{
            astNode* nd = createAstNode(relationalOp_LT,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 70:{
            astNode* nd = createAstNode(relationalOp_LE,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 71:{
            astNode* nd = createAstNode(relationalOp_GT,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 72:{
            astNode* nd = createAstNode(relationalOp_GE,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 73:{
            astNode* nd = createAstNode(relationalOp_EQ,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 74:{
            astNode* nd = createAstNode(relationalOp_NE,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 75:{
            toParentBuffer(pnode,pnode->children->end);
            destroyNode(pnode);
            break;
        }
        case 76:{
            toParentBuffer(pnode,pnode->children->end);
            destroyNode(pnode);
            break;
        }
        case 77:{
            toParentBuffer(pnode,pnode->parent->inh);
            destroyNode(pnode);
            break;
        }
        case 78:{
            astNode* nd = createAstNode(op1_plus,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 79:{
            astNode* nd = createAstNode(op1_minus,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 80:{
            toParentBuffer(pnode,pnode->children->end);
            destroyNode(pnode);
            break;
        }
        case 81:{
            toParentBuffer(pnode,pnode->children->end);
            destroyNode(pnode);
            break;
        }
        case 82:{
            toParentBuffer(pnode,pnode->parent->inh);
            destroyNode(pnode);
            break;
        }
        case 83:{
            astNode* nd = createAstNode(op2_mul,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 84:{
            astNode* nd = createAstNode(op2_div,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 85:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 86:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 87:{
            token* tk = pnode->children->start->tokenInfo;
            astNode* nd = createAstNode(u_node,NULL,NULL,NULL,tk);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            astNode* tmp = pnode->children->start;
            pnode->children->start = tmp->right;
            tmp->right->left = NULL;
            pnode->children->numOfNodes--;
            free(tmp->down);
            free(tmp);
            destroyNode(pnode);
            break;
        }
        case 88:{
            astNode* nd = createAstNode(unary_plus,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 89:{
            astNode* nd = createAstNode(unary_minus,NULL,NULL,NULL,pnode->down->lex);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 90:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        case 91:{
            toParentBuffer(pnode,pnode->children->start);
            destroyNode(pnode);
            break;
        }
        /*-------------------------------------------------------------------------------------------*/
        case 92:{
            astNode* nd = createAstNode(declareStmt_node,NULL,NULL,NULL,NULL);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode);
            break;
        }
        case 93:{
            astNode* nd = createAstNode(switchStmt_node,NULL,NULL,NULL,pnode->down->lex);
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->right->lex);
            nd->down = populateSubTree(nd->down,tmp);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            nd->startLno = pnode->down->right->right->lex->lno;
            nd->endLno = pnode->down->right->right->right->lex->lno;
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 94:
        case 95:{
            astNode* nd = createAstNode(caseStmt_node,NULL,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            nd = insertAtBegin(nd,pnode->children->start->right->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 96:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 97:{
            astNode* nd = createAstNode(value_num,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-3,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 98:{
            astNode* nd = createAstNode(value_true,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-5,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 99:{
            astNode* nd = createAstNode(value_false,NULL,NULL,NULL,NULL);
            astNode* tmp = createAstNode(-6,nd,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 100:{
            astNode* nd = createAstNode(value_plus_num,NULL,NULL,NULL,pnode->down->lex);
            astNode* tmp = createAstNode(-3,nd,NULL,NULL,pnode->down->right->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 101:{
            astNode* nd = createAstNode(value_minus_num,NULL,NULL,NULL,pnode->down->lex);
            astNode* tmp = createAstNode(-3,nd,NULL,NULL,pnode->down->right->lex);
            nd->down = populateSubTree(nd->down,tmp);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 102:{
            astNode* nd = createAstNode(default_node,NULL,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 103:{
            astNode* nd = createAstNode(-7,NULL,NULL,NULL,NULL);
            toParentBuffer(pnode,nd);
            destroyNode(pnode);
            break;
        }
        case 104:{
            astNode* nd = createAstNode(forStmt_node,NULL,NULL,NULL,pnode->down->lex);
            astNode* tmp = createAstNode(-1,nd,NULL,NULL,pnode->down->right->lex);
            nd->down = populateSubTree(nd->down,tmp);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            nd->startLno = pnode->down->right->right->lex->lno;
            nd->endLno = pnode->down->right->right->right->lex->lno;
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 105:{
            astNode* nd = createAstNode(whileStmt_node,NULL,NULL,NULL,pnode->down->lex);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            nd->startLno = pnode->down->right->lex->lno;
            nd->endLno = pnode->down->right->right->lex->lno;
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 106:{
            astNode* nd = createAstNode(range1_node,NULL,NULL,NULL,NULL);
            astNode* tmp1 = createAstNode(-3,nd,NULL,NULL,pnode->down->lex);
            astNode* tmp2 = createAstNode(-3,nd,NULL,NULL,pnode->down->right->lex);
            nd->down = populateSubTree(nd->down,tmp1);
            nd->down = populateSubTree(nd->down,tmp2);
            toParentBuffer(pnode,nd);
            destroyNode(pnode->down);
            destroyNode(pnode->down);
            destroyNode(pnode);
            break;
        }
        case 107:{
            astNode* nd = createAstNode(range2_node,NULL,NULL,NULL,NULL);
            nd->down = populateSubTree(nd->down,pnode->children->start);
            nd->down = populateSubTree(nd->down,pnode->children->start->right);
            toParentBuffer(pnode,nd);
            setParent(nd);
            destroyNode(pnode);
            break;
        }
    }
    return root;
}

int toKeep(int val){
    if(val==INTEGER||val==REAL||val==BOOLEAN||val==ARRAY||val==GET_VALUE||val==PRINT||val==TRUE||val==FALSE||val==AND||val==OR||val==FOR||val==SWITCH||val==CASE||val==DEFAULT||val==WHILE||val==ID||val==NUM||val==RNUM||val==PLUS||val==MINUS||val==MUL||val==DIV||val==LT||val==LE||val==GT||val==GE||val==EQ||val==NE||val==ASSIGNOP||val==START||val==END)
        return 1;
    return 0;
}

astNode* postOrder(node* pnode){
    astNode* root = NULL;
    if(!pnode) return root;
    int rule = pnode->ruleNo;
    //printf("rule: %d\n",rule);
    //fflush(stdout);
    node* cur = pnode->down;
    if(rule==60||rule==61||rule==75||rule==76||rule==80||rule==81){
        while(cur&&cur->right){
            if(cur->tag==terminal&&toKeep(cur->value)){
                cur = cur->right;
            }
            else if(cur->tag==terminal){
                node* tmp = cur;
                cur = cur->right;
                destroyNode(tmp);
            }
            else{
                node* tmp = cur;
                cur = cur->right;
                postOrder(tmp);
            }
        }
        if(cur->tag==terminal&&toKeep(cur->value)){
            cur = cur->right;
        }
        else if(cur->tag==terminal){
            node* tmp = cur;
            cur = cur->right;
            destroyNode(tmp);
        }
        else{
            node* tmp = cur;
            cur = cur->right;
            //printf("AST Inh rule %d\n",rule);
            //fflush(stdout);
            astInh(pnode);
            postOrder(tmp);
            //printf("AST rule %d\n",rule);
            //fflush(stdout);
            root = astSyn(pnode);
        }
    }
    else{
        while(cur){
            if(cur->tag==terminal&&toKeep(cur->value)){
                cur = cur->right;
            }
            else if(cur->tag==terminal){
                node* tmp = cur;
                cur = cur->right;
                destroyNode(tmp);
            }
            else{
                node* tmp = cur;
                cur = cur->right;
                postOrder(tmp);
            }
        }
        //printf("AST rule %d\n",rule);
        //fflush(stdout);
        root = astSyn(pnode);

    }
    return root;
}

/*---------------------------------------------------------DANGER ZONE-------------------------------------------------------*/

int isNull(astNode* root){
    if(root==NULL) return 1;
    else if(root->label==-7) return 1;
    else return 0;
}

void computeAstSize(astNode* root, int *count){
    if(!root) return;
    astNode* child = root->down->start;
    while(child){
        computeAstSize(child,count);
        child = child->next;
    }
    if(!isNull(root)){
        (*count)++;
    }
    computeAstSize(root->list,count);
}

void computeParseTreeSize(node* pnode, int *count){
    if(!pnode) return;
    node* child = pnode->down;
    while(child){
        computeParseTreeSize(child,count);
        child = child->right;
    }
    (*count)++;
}

void printSpace(int data){
    for(int i=1;i<=20-data;i++){
        printf(" ");
    }
}

int getLen(char* s){
    int cnt = 0;
    for(int i=0;s[i]!='\0';i++){
        cnt++;
    }
    return cnt;
}

void printTreeUtil(astNode* root){
    if(isNull(root)) return;
    int l = root->label;
    int p = 64;
    if(!isNull(root->parent)&&root->parent->label>=0){
        p = root->parent->label;
    }
    if(l>=0){
        printf("%s",wordLabel[l]);
        printSpace(getLen(wordLabel[l]));
        if(root->tokenInfo==NULL){
            printf("---");
            printSpace(3);
        }
        else{
            printf("%s",root->tokenInfo->lexeme);
            printSpace(getLen(root->tokenInfo->lexeme));
        }
        printf("%s",wordLabel[p]);
        printSpace(getLen(wordLabel[p]));
    }
    else{
        printf("%s",root->tokenInfo->token_t);
        printSpace(getLen(root->tokenInfo->token_t));
        printf("%s",root->tokenInfo->lexeme);
        printSpace(getLen(root->tokenInfo->lexeme));
        printf("%s",wordLabel[p]);
        printSpace(getLen(wordLabel[p]));
    }
    printf("\n");
}

void printAst(astNode* root){
    if(!root) return;
    astNode* fChild = root->down->start;
    while(fChild&&fChild->label==-7){
        fChild = fChild->next;
    }
    printAst(fChild);
    printTreeUtil(root);
    if(fChild){
        astNode* cur = fChild->next;
        while(cur){
            printAst(cur);
            cur = cur->next;
        }
    }
    printAst(root->list);
}

void printAstTree(astNode* root){
    printf("\nInorder Traversal(Leftmost child ‐‐> parent node‐‐> remaining siblings (excluding the leftmost child)):\n");
    printf("Label");
    printSpace(5);
    printf("Lexeme");
    printSpace(6);
    printf("Parent Label");
    printf("\n\n");

    printAst(root);
    //printf("%s",ntr[root->value]);
    //fclose(fp);
}

void removeAst(astNode* root){
    if(!root) return;
    astNode* cur = root->down->start;
    while(cur){
        astNode* tmp = cur;
        cur = cur->next;
        tmp->prev = NULL;
        removeAst(tmp);
    }
    free(root->down);
    free(root);
}

astNode* constructAst(node* pnode){
    astNode* root = postOrder(pnode);
    return root;
}

/*
int main(){
    astNode* root = constructAst(NULL,1);
}
*/