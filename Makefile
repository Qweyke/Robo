Robo: RoboLexer.l RoboParser.y RoboC.c
	bison -d -t -Wcounterexamples RoboParser.y 
	flex RoboLexer.l
	gcc -o Robo RoboParser.tab.c lex.yy.c RoboC.c -lfl