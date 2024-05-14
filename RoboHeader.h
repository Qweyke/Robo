#include <stdio.h>
#include <stdlib.h>

#define N 2 // size for robo pos arr
#define BIN_CAPACITY 3

int yylex(void);
void yyerror(char *er);

extern int yylineno;

extern FILE* file_yyin;
extern FILE* file_yyout;

// PROGRAM OBJECTS 
typedef struct {
    int l; // length
    int w; // width
}Coords; 

typedef struct {
    Coords crds;
    int bin;
    int trash_cnt;
}Robo; 

typedef struct {
    Coords crds;
}Trash;

typedef struct {
    Coords crds;
}Base;  

// AST

enum node_types {
    NODE_SEMICOL,
    NODE_DIRECTION,
    //NODE_START,
    //NODE_STOP,
    NODE_BREAK,
    NODE_GRAB,
    NODE_DROP,
    NODE_BIN,
    NODE_UP,
    NODE_DOWN,
    NODE_LEFT,
    NODE_RIGHT,
    NODE_TRASH,
    NODE_EMPTY,
    NODE_NUM,
    
    NODE_IF,
    NODE_ELSE,
    NODE_WHILE,

    NODE_MORE,
    NODE_LESS,
    NODE_EQL,
    NODE_NEQL
};


typedef struct {        // All kind of nodes in AST
    enum node_types node_type;
    struct AstNode *left_node;      
    struct AstNode *right_node;
}AstNode; 

typedef struct {        // Numbers
    enum node_types node_type;              
    int num;
}NumNode;

typedef struct {
    enum node_types node_type;           
    AstNode *cond;      
    AstNode *body;       
    AstNode *els;        
}LogicNode;

typedef struct {
    enum node_types node_type;           
    AstNode *up;      
    AstNode *body;       
    AstNode *els;        
}InequalNode;

Robo robo;
Base base;
Trash *trash;
int bin_left = BIN_CAPACITY;
int trash_qnty;

AstNode *CrtNode(enum node_types nd_type, AstNode *l_nd, AstNode *r_nd);
AstNode *CrtNumNode(int digit);
AstNode *CrtLogicNode(enum node_types nd_type, AstNode *cndn, AstNode *bdy, AstNode *es);

int FoldAst(AstNode *);
//void FoldNumNode(int steps, char direction);
void FoldLogicNode(char logic_type);

void FreeAst(AstNode *);

void ReadEnvFile(FILE* file_env);