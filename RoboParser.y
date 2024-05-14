%{
#include "RoboHeader.h"

%}

%union{
    AstNode *node;
}

%token TRASH EMPTY BIN

%token START STOP

%token GRAB_TRASH DROP_TRASH

%token UP DWN LFT RGHT

%token NUM
%token IF ELSE WHILE
%token EQL NEQL  

%type <node> program_body program_field condition inequality els number step_direction

%start program

%%

program:
    START program_body STOP {FoldAst($2); FreeAst($2); fprintf(yyout, "Program completed succsessfully");}
    |START STOP {fprintf(yyout, "Exiting program without completion");}
    |{fprintf(yyout, "Program file is empty");}
    ;

program_body:
    IF '(' condition ')' '{' program_body '}' {$$ = CrtLogicNode(NODE_IF, $3, $6, NULL);}
    |IF '(' condition ')' '{' program_body '}' els {$$ = CrtLogicNode(NODE_IF, $3, $6, $8);}
    |WHILE '(' condition ')' '{' program_body '}' {$$ = CrtLogicNode(NODE_WHILE, $3, $6, NULL);}
    |program_field ';' {$$ = CrtNode(NODE_SEMICOL, $1, NULL);}
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
els:
    ELSE '{'program_body'}' {$$ = CrtNode(NODE_ELSE, $3, NULL);}
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