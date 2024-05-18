#include <stdio.h>
#include <stdlib.h>

#define N 2 // size for robo pos arr
#define BIN_CAPACITY 3

int yylex(void);
void yyerror(char *er);

extern int yylineno;

extern int bin_left;
extern int trash_qnty;
extern int line_number;

extern FILE* file_yyin;
extern FILE* file_yyout;

// PROGRAM OBJECTS 
typedef struct _Coords {
    int l; // length
    int w; // width
}Coords; 

typedef struct _Robo {
    Coords crds;
    int bin;
    int trash_cnt;
}Robo; 

typedef struct _Trash {
    Coords crds;
}Trash;

typedef struct _Base {
    Coords crds;
}Base;  

// AST

enum node_types {
    NODE_SEMICOL,
    NODE_SPDIRECTION,
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

typedef struct _AstNode {        // All non-special kinds of nodes in AST
    enum node_types node_type;
    struct _AstNode *left_node;      
    struct _AstNode *right_node;
} AstNode; 

typedef struct _NumNode {        // Numbers
    enum node_types node_type;              
    int num;
} NumNode;

typedef struct _LogicNode {   // If, while
    enum node_types node_type;           
    AstNode *cond;      
    AstNode *body;       
    AstNode *els;        
} LogicNode;

typedef struct _InequalNode{
    enum node_types node_type;           
    AstNode *up;      
    AstNode *body;       
    AstNode *els;        
} InequalNode;

extern Robo robo;
extern Base base;
extern Trash *trash;

void ReadEnvFile(FILE* file_env);

AstNode *CrtNode(enum node_types nd_type, AstNode *l_nd, AstNode *r_nd);
AstNode *CrtNumNode(int digit);
AstNode *CrtLogicNode(enum node_types nd_type, AstNode *cndn, AstNode *bdy, AstNode *es);

int FoldAst(AstNode *);
void FreeAst(AstNode *);