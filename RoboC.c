#include <RoboHeader.h>

int main(void)
{
    char *path_in = "RoboInput.txt";
    FILE *file_input = fopen(path_in, "r");
    if (file_input == NULL) {
        fprintf(stderr, "Error: unable to open file %s or it doesn't exist,", path_in);
        exit(EXIT_FAILURE);
    }
    file_yyin = file_input;

    char *path_env = "RoboEnv.txt";
    FILE *file_env = fopen(path_env, "r");
    if (file_env == NULL) {
        fprintf(stderr, "Error: unable to open file %s or it doesn't exist,", path_env);
        exit(EXIT_FAILURE);
    }
    
    char *path_out = "RoboOutput.txt";
    FILE *file_output = fopen(path_out, "w");
    if (file_output == NULL) {
        fprintf(stderr, "Error: unable to open file %s or it doesn't exist,", path_out);
        exit(EXIT_FAILURE);
    }
    file_yyout = file_output;

    ReadEnvFile(file_env);

    yyparse();

    fclose(file_yyin);
    fclose(file_output);
    free(trash);

    return 0;
}

void yyerror(char *er)
{
    fprintf(stderr, "Error: %s, in line %d", er, yylineno);
    exit(EXIT_FAILURE);
}

void ReadEnvFile(FILE* file_env) {
    
    char obj;
    int line;
    int lines_cnt = 0;    
    int length, width;
    int t_index = 0; 
    
    while ((line = fgetc(file_env)) !=EOF) {
        if (line == '\n') lines_cnt ++;
    }

    trash_qnty = lines_cnt - 2;
    trash = (Trash *)malloc((trash_qnty) * sizeof(Trash));  // -2 bcs of base and robo lines
    if (trash == NULL) {
        fprintf(file_yyout, "Cannot allocate memory for trash array \n");
        exit(EXIT_FAILURE);
    }
    
    rewind(file_env);
    
    while ((fscanf(file_env, "%c %d,%d", &obj, &length, &width)) != EOF) {        
        switch(obj){
            case('b'):
                base.crds.l = length;
                base.crds.w = width;
                break;
            case('r'):
                robo.crds.l = length;
                robo.crds.w = width;
                robo.bin = BIN_CAPACITY;
                robo.trash_cnt = 0;
                break;
            case('t'):
                trash[t_index].crds.l = length;
                trash[t_index].crds.w = width;
                t_index++;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }
    fclose(file_env);
}

AstNode *CrtNode(enum node_types nd_type, AstNode *l_nd, AstNode *r_nd) {
    AstNode *a_node = (AstNode *)malloc(sizeof(AstNode));
    if (a_node == NULL) {
        fprintf(file_yyout, "Cannot allocate memory for ast node \n");
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
        fprintf(file_yyout, "Cannot allocate memory for num node \n");
        exit(EXIT_FAILURE);
    }
    num_node->node_type = NODE_NUM;
    num_node->num = digit;
    return (AstNode*)num_node;
}

AstNode *CrtLogicNode(enum node_types nd_type, AstNode *cndn, AstNode *bdy, AstNode *es) {
    LogicNode *logic_node = malloc(sizeof(NumNode));
    if (logic_node == NULL) {
        fprintf(file_yyout, "Cannot allocate memory for logic node \n");
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
        if (FoldAst(((LogicNode*)ast)->cond) == 1) { // if cond true
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
        for (int i = 0; i < trash_qnty; i++) {
            if ((robo.crds.l == trash[i].crds.l) && (robo.crds.w == trash[i].crds.w)) {
                fold_res = 0;
                break;
            } 
        }       
        break;

    case NODE_TRASH:
        fold_res = 0;
        for (int i = 0; i < trash_qnty; i++) {
            if ((robo.crds.l == trash[i].crds.l) && (robo.crds.w == trash[i].crds.w)) {
                fold_res = 1;
                break;
            } 
        }       
        break;

    case NODE_BIN:
        AstNode *equal = ast->left_node;
        int num_cmpr = FoldAst(ast->right_node);
        
        if (equal->node_type == 16) { // if bin >(16) num - true(1) else false(0) 
            if (bin_left > num_cmpr) {
                fold_res = 1;            
            }
            else fold_res = 0;
        }

        else if (equal->node_type == 17) {
            if (bin_left < num_cmpr) {
                fold_res = 1;            
            }
            else fold_res = 0;
        }

        else if (equal->node_type == 18) {
            if (bin_left == num_cmpr) {
                fold_res = 1;            
            }
            else fold_res = 0;
        }

        else if (equal->node_type == 19) {
            if (bin_left != num_cmpr) {
                fold_res = 1;            
            }
            else fold_res = 0;
        }

        else {
            fprintf(file_yyout, "Error in comparison case BIN \n");
            exit(EXIT_FAILURE);
        }
        break;
        
    case NODE_GRAB:
        bin_left --;  // get 1 piece of junk
        line_number ++;
        frprintf(file_yyout, "%d. Robo got 1 piece of trash at [%d length][%d width]. Bin capacity = (%d) more", line_number, robo.crds.l, robo.crds.w, robo.bin);    
        break;

    case NODE_DROP:
        bin_left = BIN_CAPACITY; // drop all junk on base
        line_number ++;
        fprintf(file_yyout, "%d. Robo dropped all trash at base [%d length][%d width]. Bin capacity restored", line_number, base.crds.l, base.crds.w);
        break;    

    case NODE_SPDIRECTION:
        int steps = FoldAst(ast->left_node);
        int direction = FoldAst(ast->right_node);
            switch (direction)
            {
            case 1:
                if (robo.crds.w - steps < 1 || robo.crds.w - steps > 20) {
                    fprintf(file_yyout, "Robo can't go UP beyond 20x20 borders. Program has been stoped!");
                    exit(EXIT_FAILURE);
                }
                line_number ++;
                robo.crds.w = robo.crds.w - steps;
                fprintf(file_yyout, "%d. Robo moved UP to [%d][%d]", line_number, robo.crds.l, robo.crds.w);
                break;

            case 2:
                if (robo.crds.w + steps < 1 || robo.crds.w + steps > 20) {
                    fprintf(file_yyout, "Robo can't go DOWN beyond 20x20 borders. Program has been stoped!");
                    exit(EXIT_FAILURE);
                }
                line_number ++;
                robo.crds.w = robo.crds.w + steps;
                fprintf(file_yyout, "%d. Robo moved DOWN to [%d][%d]", line_number, robo.crds.l, robo.crds.w);
                break;

            case 3:
                if (robo.crds.l - steps < 1 || robo.crds.l - steps > 20) {
                    fprintf(file_yyout, "Robo can't go LEFT beyond 20x20 borders. Program has been stoped!");
                    exit(EXIT_FAILURE);
                }
                line_number ++;
                robo.crds.w = robo.crds.l - steps;
                fprintf(file_yyout, "%d. Robo moved LEFT to [%d][%d]", line_number, robo.crds.l, robo.crds.w);
                break;         

            case 4:
                if (robo.crds.l + steps < 1 || robo.crds.l + steps > 20) {
                    fprintf(file_yyout, "Robo can't go RIGHT beyond 20x20 borders. Program has been stoped!");
                    exit(EXIT_FAILURE);
                }
                line_number ++;
                robo.crds.w = robo.crds.l + steps;
                fprintf(file_yyout, "%d. Robo moved RIGHT to [%d][%d]", line_number, robo.crds.l, robo.crds.w);
                break;     

            default:
                break;
            }

        break;

    case NODE_UP:
        fold_res = 1; // 1 - UP
        break;
    case NODE_DOWN:
        fold_res = 2; // 2 - DOWN
        break;
    case NODE_LEFT:
        fold_res = 3; // 3 - LEFT
        break;
    case NODE_RIGHT:
        fold_res = 4; // 4 - RIGHT
        break;

    default:
        fprintf(file_yyout, "Error in AST folding");
        exit(EXIT_FAILURE);        
    }
    return fold_res;
}

void FreeAst(AstNode *ast) {
    
    switch(ast->node_type){ 
        case NODE_IF:
            
            if ((((LogicNode *)ast)->body) != NULL) {
                FreeAst(((LogicNode *)ast)->body);
            }
            

    }
    

}