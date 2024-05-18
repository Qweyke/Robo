#include "RoboHeader.h"
#include "RoboParser.tab.h"

int yyparse(void);

FILE *file_env;

Robo robo;
Base base;
Trash *trash;
int trash_qnty;
int line_number = 0;

int main(void)
{
    char *path_in = "files/RoboInput.txt";
    yyin = fopen(path_in, "r");
    if (yyin == NULL) {
        fprintf(stderr, "Error: unable to open file %s or it doesn't exist \n", path_in);
        exit(EXIT_FAILURE);
    }

    char *path_env = "files/RoboEnv.txt";
    file_env = fopen(path_env, "r");
    if (file_env == NULL) {
        fprintf(stderr, "Error: unable to open file %s or it doesn't exist \n", path_env);
        exit(EXIT_FAILURE);
    }
    
    char *path_out = "files/RoboOutput.txt";
    yyout = fopen(path_out, "w");
    if (yyout == NULL) {
        fprintf(stderr, "Error: unable to open file %s or it doesn't exist \n", path_out);
        exit(EXIT_FAILURE);
    }
    
    ReadEnvFile(file_env);
    fprintf(yyout, "Map (20 x 20) \n");
    
    yydebug = 1;
    yyparse();

    fclose(yyin);
    fclose(yyout);
    free(trash);

    return 0;
}

void yyerror(char *er)
{
    fprintf(stderr, "Error: %s, in line %d \n", er, yylineno);
    exit(EXIT_FAILURE);
}

void ReadEnvFile(FILE* file_env) {
    
    char obj;
    int line;
    int lines_cnt = 0;    
    int length, width;
    int t_index = 0; 
    
    while ((line = fgetc(file_env)) != EOF) {
        if (line == '\n') lines_cnt ++;
    }
    if (lines_cnt > 0) lines_cnt ++; // last line doesn't have a '/n'

    trash_qnty = lines_cnt - 2;
    trash = (Trash *)malloc((trash_qnty) * sizeof(Trash));  // -2 bcs of base and robo lines
    if (trash == NULL) {
        fprintf(stderr, "Cannot allocate memory for trash array \n");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "trash qnty = %d, lines_cnt = %d \n", trash_qnty, lines_cnt );
    
    rewind(file_env);
    
    while ((fscanf(file_env, " %c %d,%d", &obj, &length, &width)) != EOF) {        
        switch(obj){
            case('b'):
                base.crds.l = length;
                base.crds.w = width;
                //fprintf(stdout, "Case %c, length = %d - %d, width = %d - %d \n ", obj, length, base.crds.l, width, base.crds.w);
                break;
            case('r'):
                robo.crds.l = length;
                robo.crds.w = width;
                robo.bin = BIN_CAPACITY;
                //fprintf(stdout, "Case %c, length = %d - %d, width = %d - %d \n ", obj, length, robo.crds.l, width, robo.crds.w);
                break;
            case('t'):
                trash[t_index].crds.l = length;
                trash[t_index].crds.w = width;
                //fprintf(stdout, "Case %c, length = %d - %d, width = %d - %d \n ", obj, length, trash[t_index].crds.l, width, trash[t_index].crds.w);
                t_index++;
                break;
            default:
                fprintf(stderr, "Unnknown case %c \n ", obj);
                exit(EXIT_FAILURE);
        }
    }
    fclose(file_env);
    fprintf(stdout, "env done" );
}

AstNode *CrtNode(enum node_types nd_type, AstNode *l_nd, AstNode *r_nd) {
    AstNode *a_node = malloc(sizeof(AstNode));
    if (a_node == NULL) {
        fprintf(stderr, "Cannot allocate memory for ast node \n");
        exit(EXIT_FAILURE);
    }
    a_node->node_type = nd_type;
    a_node->left_node = l_nd;
    a_node->right_node = r_nd;
    return a_node;
}

AstNode *CrtNumNode(int digit) {
    NumNode *num_node = malloc(sizeof(NumNode));
    if (num_node == NULL) {
        fprintf(stderr, "Cannot allocate memory for num node \n");
        exit(EXIT_FAILURE);
    }
    num_node->node_type = NODE_NUM;
    num_node->num = digit;
    return (AstNode*)num_node;
}

AstNode *CrtLogicNode(enum node_types nd_type, AstNode *cndn, AstNode *bdy, AstNode *es) {
    LogicNode *logic_node = malloc(sizeof(LogicNode));
    if (logic_node == NULL) {
        fprintf(stderr, "Cannot allocate memory for logic node \n");
        exit(EXIT_FAILURE);
    }
    logic_node->node_type = nd_type;
    logic_node->cond = cndn;
    logic_node->body = bdy;
    logic_node->els = es;
    return (AstNode*)logic_node;
}

int FoldAst(AstNode *ast) {

    int fold_res = 0;

    switch (ast->node_type)
    {
    case NODE_NUM:
        fold_res = ((NumNode*)ast)->num;  // exception for numbers
        break;

    case NODE_IF:
        if (FoldAst(((LogicNode*)ast)->cond) == 1) { // if cond true
            if ((((LogicNode*)ast)->body) != NULL) {
                FoldAst(((LogicNode*)ast)->body);
            }
        }
        else {
            if ((((LogicNode*)ast)->els) != NULL) {
                FoldAst((((LogicNode*)ast)->els));
            }
        }
        break;

    case NODE_WHILE:
        while (FoldAst(((LogicNode*)ast)->cond) == 1) { // if cond true
            if ((((LogicNode*)ast)->body) != NULL) {
                FoldAst(((LogicNode*)ast)->body);
            }
        }
        break;

    case NODE_ELSE:
        FoldAst(ast->left_node);
        break;

    case NODE_SEMICOL:
        FoldAst(ast->left_node);
        break;

    case NODE_EMPTY:
        fold_res = 1;
        //fprintf(yyout, "Empty at [%d][%d] \n", robo.crds.l, robo.crds.w);
        for (int i = 0; i < trash_qnty; i++) {
            if ((robo.crds.l == trash[i].crds.l) && (robo.crds.w == trash[i].crds.w)) {
                fold_res = 0;
                //fprintf(yyout, "Hit at [%d][%d] \n", robo.crds.l, robo.crds.w);
                break;
            } 
        }       
        break;

    case NODE_TRASH:
        fold_res = 0;
        for (int i = 0; i < trash_qnty; i++) {
            if ((robo.crds.l == trash[i].crds.l) && (robo.crds.w == trash[i].crds.w)) {
                fold_res = 1;
                robo.trash_i = i;
                break;
            } 
        }       
        break;

    case NODE_BIN:
        AstNode *equal = ast->left_node;
        int num_cmpr = FoldAst(ast->right_node);
        
        if (equal->node_type == 16) { // if bin >(16) num - true(1) else false(0) 
            if (robo.bin > num_cmpr) {
                fold_res = 1;            
            }
            else fold_res = 0;
        }

        else if (equal->node_type == 17) {
            if (robo.bin < num_cmpr) {
                fold_res = 1;            
            }
            else fold_res = 0;
        }

        else if (equal->node_type == 18) {
            if (robo.bin == num_cmpr) {
                fold_res = 1;            
            }
            else fold_res = 0;
        }

        else if (equal->node_type == 19) {
            if (robo.bin != num_cmpr) {
                fold_res = 1;            
            }
            else fold_res = 0;
        }

        else {
            fprintf(stderr, "Error in comparison case BIN \n");
            exit(EXIT_FAILURE);
        }
        break;
        
    case NODE_GRAB:
        robo.bin = robo.bin - 1;  // get 1 piece of junk
        line_number ++;
        trash[robo.trash_i].crds.l = 0; // deleting junk from map
        trash[robo.trash_i].crds.w = 0; 
        fprintf(yyout, "%d. Robo wiped 1 piece of trash at [%d][%d]. Bin capacity = (%d) \n", line_number, robo.crds.l, robo.crds.w, robo.bin);    
        break;

    case NODE_DROP:
        robo.bin = BIN_CAPACITY; // drop all junk on base
        line_number ++;
        if (robo.crds.l == base.crds.l && robo.crds.w == base.crds.w) {
            fprintf(yyout, "%d. Robo dropped all trash at base [%d][%d]. Bin capacity restored \n", line_number, base.crds.l, base.crds.w);
        }
        else {
            fprintf(yyout, "%d. What a mess! Robo dropped all junk on the floor at [%d][%d]!", line_number, robo.crds.l, robo.crds.w);
        }
        
        break;    

    case NODE_SPDIRECTION:
        int steps = FoldAst(ast->right_node);
        //fprintf(yyout, "node type %d\n", ast->right_node->node_type);
        int direction = FoldAst(ast->left_node);
            switch (direction)
            {
            case 8:
                //fprintf(yyout, "case 1\n");
                for (int i = 1; i <= steps; i++) {
                    if (robo.crds.w - 1 < 1) {
                        fprintf(yyout, "Robo can't go UP beyond 20x20 borders. Program has been stoped! \n");
                        exit(EXIT_FAILURE);
                    }
                    line_number ++;
                    robo.crds.w = robo.crds.w - 1;
                    fprintf(yyout, "%d. Robo moved UP to [%d][%d]\n", line_number, robo.crds.l, robo.crds.w);                   
                }
                break;
                

            case 2:
                //fprintf(yyout, "case 2\n");
                for (int i = 1; i <= steps; i++) {
                    if (robo.crds.w + 1 > 20) {
                        fprintf(yyout, "Robo can't go DOWN beyond 20x20 borders. Program has been stoped! \n");
                        exit(EXIT_FAILURE);
                    }
                    line_number ++;
                    robo.crds.w = robo.crds.w + 1;
                    fprintf(yyout, "%d. Robo moved DOWN to [%d][%d]\n", line_number, robo.crds.l, robo.crds.w);
                }
                break;

            case 4:
                //fprintf(yyout, "case 3\n");
                for (int i = 1; i <= steps; i++) {
                    if (robo.crds.l - 1 < 1) {
                        fprintf(yyout, "Robo can't go LEFT beyond 20x20 borders. Program has been stoped! \n");
                        exit(EXIT_FAILURE);
                    }
                    line_number ++;
                    robo.crds.l = robo.crds.l - 1;
                    fprintf(yyout, "%d. Robo moved LEFT to [%d][%d]\n", line_number, robo.crds.l, robo.crds.w);
                }
                break;         

            case 6:
                //fprintf(yyout, "case 4\n");
                for (int i = 1; i <= steps; i++) {
                    if (robo.crds.l + 1 > 20) {
                        fprintf(yyout, "Robo can't go RIGHT beyond 20x20 borders. Program has been stoped! \n");
                        exit(EXIT_FAILURE);
                    }
                    line_number ++;
                    robo.crds.l = robo.crds.l + 1;
                    fprintf(yyout, "%d. Robo moved RIGHT to [%d][%d] \n", line_number, robo.crds.l, robo.crds.w);
                }
                break;     

            default:
                fprintf(yyout, "default in directions\n");
                exit(EXIT_FAILURE);
            }
        break;

    case NODE_UP: // like numpad arrows
        fold_res = 8; // 8 - UP
        break;
    case NODE_DOWN:
        fold_res = 2; // 2 - DOWN
        break;
    case NODE_LEFT:
        fold_res = 4; // 4 - LEFT
        break;
    case NODE_RIGHT:
        fold_res = 6; // 6 - RIGHT
        break;

    default:
        fprintf(stderr, "Error in AST folding \n");
        exit(EXIT_FAILURE);        
    }
    return fold_res;
}

void FreeAst(AstNode *ast) {
    
    switch(ast->node_type){ 
        
        case NODE_WHILE:
        case NODE_IF:            
            if ((((LogicNode *)ast)->body) != NULL) {
                FreeAst(((LogicNode *)ast)->body);
            }
            if ((((LogicNode *)ast)->els) != NULL) {
                FreeAst(((LogicNode *)ast)->els);
            }
            FreeAst(((LogicNode *)ast)->cond);
            free(ast);
            break;

        case NODE_SPDIRECTION:
        case NODE_BIN:
            FreeAst(ast->left_node);
            FreeAst(ast->right_node);
            free(ast);
            break;

        case NODE_ELSE:
        case NODE_SEMICOL:
            FreeAst(ast->left_node);
            free(ast);
            break;

        case NODE_NUM:
        case NODE_UP:
        case NODE_DOWN:
        case NODE_LEFT:
        case NODE_RIGHT:
        case NODE_DROP:
        case NODE_GRAB:
        case NODE_MORE:
        case NODE_LESS:
        case NODE_EQL:
        case NODE_NEQL:
        case NODE_EMPTY:
        case NODE_TRASH:
            free(ast);
            break;     
        
        default:
            printf("Error in freeing, bad nodetype = %c,%d\n",
                    ast->node_type, ast->node_type);
            break;
    }
}