%{
#include "RoboHeader.h"
%}

%union{
    AstNode *node;
    int integer;
}

%token TRASH EMPTY BIN

%token START STOP

%token GRAB_TRASH DROP_TRASH

%token UP DWN LFT RGHT

%token IF ELSE WHILE
%token EQL NEQL  


%token <integer> NUM
%type <node> program_field condition inequality els number step_direction code_action

%start program

%%

program:
    START program_body STOP {fprintf(yyout, "Program completed succsessfully\n");}
    |START STOP {fprintf(yyout, "Exiting program without completion \n");}
    ;

program_body:
    program_body code_action {FoldAst($2); FreeAst($2);}
    |code_action {FoldAst($1); FreeAst($1);}
    ;

code_action:
    |IF '(' condition ')' '{' code_action '}' {$$ = CrtLogicNode(NODE_IF, $3, $6, NULL);}
    |IF '(' condition ')' '{' code_action '}' els {$$ = CrtLogicNode(NODE_IF, $3, $6, $8);}
    |WHILE '(' condition ')' '{' code_action '}' {$$ = CrtLogicNode(NODE_WHILE, $3, $6, NULL);}   
    |program_field ';' {$$ = CrtNode(NODE_SEMICOL, $1, NULL);}
    ;

els:
    ELSE '{'code_action'}' {$$ = CrtNode(NODE_ELSE, $3, NULL);}
    ;

condition:
    EMPTY {$$ = CrtNode(NODE_EMPTY, NULL, NULL);}
    |TRASH {$$ = CrtNode(NODE_TRASH, NULL, NULL);}
    |BIN inequality number {$$ = CrtNode(NODE_BIN, $2, $3);}
    ;

inequality: // no need to fold // getting type only
    '>' {$$ = CrtNode(NODE_MORE, NULL, NULL);}
    |'<' {$$ = CrtNode(NODE_LESS, NULL, NULL);}
    |EQL {$$ = CrtNode(NODE_EQL, NULL, NULL);}
    |NEQL {$$ = CrtNode(NODE_NEQL, NULL, NULL);}
    ;

program_field:
    GRAB_TRASH {$$ = CrtNode(NODE_GRAB, NULL, NULL);}
    |DROP_TRASH {$$ = CrtNode(NODE_DROP, NULL, NULL);}
    |step_direction number {$$ = CrtNode(NODE_SPDIRECTION, $1, $2);}
    ;

step_direction:
    UP {$$ = CrtNode(NODE_UP, NULL, NULL);}
    |DWN {$$ = CrtNode(NODE_DOWN, NULL, NULL);}
    |LFT {$$ = CrtNode(NODE_LEFT, NULL, NULL);}
    |RGHT {$$ = CrtNode(NODE_RIGHT, NULL, NULL);}
    ;
number:
    NUM {$$ = CrtNumNode($1);}
    ;  

%%