#include <stdio.h>

//надо, чтобы нормально перемeнные выводил

#include "draw_tree.h"

static void make_file_names(char* const input_file_name, char* const output_file_name, size_t enter_cnt, Err_param *const error);

static void make_nodes(const Node* const node, const Node* const definite_node, char* const input_file_data, Err_param *const error);
static void make_connections(const Node* const node, char* const input_file_data, Err_param *const error);

static void fill_input_file(const char* const  input_file_name, const char* const input_file_data, Err_param *const error);

static void do_graph_cmd(const char* const input_file_name, const char* const output_file_name, Err_param *const error);

void graph_dump(Node* const node, Node* const definite_node, Err_param *const error)
{
    assert(node);
    assert(definite_node);
    assert(error);

    int sprintf_res = 0;

    static size_t enter_cnt = 0;

    char  input_file_name[MAX_STR_LEN] = {};
    char output_file_name[MAX_STR_LEN] = {};

    make_file_names(input_file_name, output_file_name, enter_cnt, error);

    char input_file_data[INPUT_FILE_SIZE] = {};

    sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%sdigraph G\n{\n\tnode [shape = Mrecord; fillcolor = \"#9FDFDA\";];\n", input_file_data);
    SPRINTF_CHECK

    make_nodes(node, definite_node, input_file_data, error);
    
    RETURN_VOID

    //printf("NODES %s\n", input_file_data);

    sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\n", input_file_data);
    SPRINTF_CHECK

    make_connections(node, input_file_data, error);
    RETURN_VOID

    //printf("CONNECTIONS %s\n", input_file_data);

    sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\n}\n", input_file_data);
    SPRINTF_CHECK
    fill_input_file(input_file_name, input_file_data, error);
    RETURN_VOID

    //printf("RESULT %s\n", input_file_data);

    do_graph_cmd(input_file_name, output_file_name, error);
    RETURN_VOID

    enter_cnt++;
}


void make_file_names(char* const input_file_name, char* const output_file_name, size_t enter_cnt, Err_param *const error)
{
    assert(input_file_name);
    assert(output_file_name);
    assert(error);

    int sprintf_res = 0;

    static char name_base[MAX_STR_LEN] = {};

    strncpy(output_file_name, input_file_name, MAX_STR_LEN);
    CPY_CHECK(output_file_name)

    sprintf_res = sprintf_s(input_file_name, MAX_STR_LEN, "%s%lld.dot", name_base, enter_cnt);
    SPRINTF_CHECK

    sprintf_res = sprintf_s(output_file_name, MAX_STR_LEN, "%s%lld.png", name_base, enter_cnt);
    SPRINTF_CHECK
}


//make_nudes
void make_nodes(const Node* const node, const Node* const definite_node, char* const input_file_data, Err_param *const error) 
{
    assert(node);
    assert(input_file_data);
    assert(error);

    int sprintf_res = 0;

    if (!node)
        return;

    if (node == definite_node)
    {
        if (node->type == NUM)
        {
            sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\tnode%p [style = filled; fillcolor = \"#E64F72\"; label = \"{<f0> %s | %lg  |{<f1> left%p | <f2> right%p}} \"];\n", \
                input_file_data, node, NUM_DEF, node->value, node->Left, node->Right);
            SPRINTF_CHECK
        }
        else if (node->type == OP)
        {
            sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\tnode%p [style = filled; fillcolor = \"#E64F72\"; label = \"{<f0> %s | %s  |{<f1> left%p | <f2> right%p}} \"];\n", \
                input_file_data, node, OP_DEF, operations[(int)node->value]->name, node->Left, node->Right);
            SPRINTF_CHECK
        }
        else if (node->type == ID)
        {
            sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\tnode%p [style = filled; fillcolor = \"#E64F72\"; label = \"{<f0> %s | %s  |{<f1> left%p | <f2> right%p}} \"];\n", \
                input_file_data, node, VAR_DEF, /*variables[(int)node->value]->name*/, node->Left, node->Right);
            SPRINTF_CHECK
        }
    } 
    else if (node->type == NUM)
    {
        sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\tnode%p [style = filled; fillcolor = \"#177E89\"; label = \"{<f0> %s | %lg  |{<f1> left%p | <f2> right%p}} \"];\n", \
            input_file_data, node, NUM_DEF, node->value, node->Left, node->Right);
        SPRINTF_CHECK
    }
    else if (node->type == ID)
    {
        sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\tnode%p [style = filled; fillcolor = \"#084C61\"; label = \"{<f0> %s | %s  |{<f1> left%p | <f2> right%p}} \"];\n", \
            input_file_data, node, VAR_DEF, variables[(int)node->value]->name, node->Left, node->Right);
        SPRINTF_CHECK
    }
    else if (node->type == OP)
    {
        sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\tnode%p [style = filled; fillcolor = \"#DB3A34\"; label = \"{<f0> %s | %s  |{<f1> left%p | <f2> right%p}} \"];\n", \
            input_file_data, node, OP_DEF, operations[(int)node->value]->name, node->Left, node->Right);
        SPRINTF_CHECK
    }
    
    if (node->Left)
    {
        make_nodes(node->Left, definite_node, input_file_data, error);
        RETURN_VOID
    }
        
    if (node->Right)
    {
        make_nodes(node->Right, definite_node, input_file_data, error);
        RETURN_VOID
    }
}


void make_connections(const Node* const node, char* const input_file_data, Err_param *const error)
{
    assert(node);
    assert(input_file_data);
    assert(error);

    int sprintf_res = 0;

    if (!node)
        return;

    if (node->Left && node->Right)
    {
        sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\t{rank = same; node%p; node%p}\n", \
            input_file_data, node->Left, node->Right);
        SPRINTF_CHECK
    }
        
    if (node->Left)
    {
        sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\tnode%p:<f1> -> node%p:<f0> [weight = 0.15; color = \"#1D638B\";];\n", \
            input_file_data, node, node->Left);
        SPRINTF_CHECK

        make_connections(node->Left, input_file_data, error);
        RETURN_VOID
    }

    if (node->Right)
    {
        sprintf_res = sprintf_s(input_file_data, INPUT_FILE_SIZE, "%s\tnode%p:<f2> -> node%p:<f0> [weight = 0.15; color = \"#1D638B\";];\n", \
            input_file_data, node, node->Right);
        SPRINTF_CHECK

        make_connections(node->Right, input_file_data, error);
        RETURN_VOID
    }
}

void fill_input_file(const char* const  input_file_name, const char* const input_file_data, Err_param *const error)
{
    assert(input_file_name);
    assert(input_file_data);
    assert(error);

    FILE* input_file;
    input_file = fopen(input_file_name, "w");
    FILE_CHECK(input_file)

    fwrite(input_file_data, sizeof(char), INPUT_FILE_SIZE, input_file); 

    int close_res = fclose(input_file);
    CLOSE_CHECK
}

void do_graph_cmd(const char* const input_file_name, const char* const output_file_name, Err_param *const error)
{
    assert(input_file_name);
    assert(output_file_name);
    assert(error);

    char cmd[MAX_STR_LEN] = {};
    int sprintf_res = sprintf_s(cmd, MAX_STR_LEN, "dot %s -Tpng -o %s", input_file_name, output_file_name);
    SPRINTF_CHECK

    int system_res = system(cmd);
    CMD_CHECK
}