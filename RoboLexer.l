%option yylineno

%{
#include "RoboHeader.h"
#include "RoboParser.tab.h"  
%}

%%

"START" { return START; }
"STOP" { return STOP; }

"GRAB" { return GRAB_TRASH; }
"DROP" { return DROP_TRASH; }

"UP" { return UP; }
"DOWN" { return DWN; }
"LEFT" { return LFT; }
"RIGHT" { return RGHT; }

"TRASH" { return TRASH; }
"EMPTY" { return EMPTY; }
"BIN" { return BIN; }

[0-9]+ {yylval.integer = atoi(yytext); return NUM;}
"if" { return IF; }
"else" { return ELSE; }
"while" { return WHILE; }
">" { return *yytext; }
"<" { return *yytext; }
"==" { return EQL; }
"!=" { return NEQL; }
[(){};] { return yytext[0]; }
[ \n\t]+ {}
. {printf("UNKNOWN INPUT %s", yytext);}

%%