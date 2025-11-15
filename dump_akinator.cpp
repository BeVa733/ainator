#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "akinator.h"
#include "dump_akinator.h"

FILE* tree_dump_file = NULL;
int image_counter = 0;

char error_msg[STR_MAX_LEN] = "";
unsigned int verify_result = 0;

void tree_dump_init(void)
{
    tree_dump_file = fopen("akinator_dump.html", "w");
    if (!tree_dump_file)
    {
        printf("Failed to open akinator_dump.html for writing\n");
        return;
    }

    fprintf(tree_dump_file, "<html><head><title>Akinator Tree Dump</title></head><body>\n");
    fprintf(tree_dump_file, "<h1>Akinator Tree Dump</h1>\n");
    fflush(tree_dump_file);
}

void tree_dump_close(void)
{
    if (tree_dump_file)
    {
        fprintf(tree_dump_file, "</body></html>\n");
        fclose(tree_dump_file);
        tree_dump_file = NULL;
    }
}

void tree_dump_add_info(const char* info, node_t* tree)
{
    if (!tree_dump_file)
        return;

    fprintf(tree_dump_file, "<h2>%s</h2>\n", info);

    if (tree)
    {
        char filename[STR_MAX_LEN];
        sprintf(filename, "tree_dump_%d", image_counter++);

        tree_graph_to_file(tree, filename);
        fprintf(tree_dump_file, "<img src='%s.png'><br>\n", filename);

        fprintf(tree_dump_file, "<h2>Tree text info</h2>");
        write_node(head, tree_dump_file);
    }

    fflush(tree_dump_file);
}

unsigned int tree_verif(node_t* node)
{
    unsigned int errors = TREE_NO_ERRORS;

    if (node == NULL)
        return errors | TREE_BAD_MAIN_PTR;

    if (node->data == NULL)
        errors |= TREE_BAD_DATA_PTR;

    const int MAX_NODES = 1000;
    bool* visited_nodes = (bool*)calloc(MAX_NODES, sizeof(bool));
    int visited_count = 0;

    if (tree_verify_recursive(node, visited_nodes, &visited_count, MAX_NODES))
        errors |= TREE_CYCLE_DETECTED;

    free(visited_nodes);
    return errors;
}

bool tree_verify_recursive(node_t* node, bool* visited_nodes, int* visited_count, int capacity)
{
    if (node == NULL)
        return false;

    for (int i = 0; i < *visited_count; i++)
    {
        if (visited_nodes[i] && &visited_nodes[i] == (bool*)node)
            return true;
    }

    if (*visited_count >= capacity)
        return false;

    visited_nodes[(*visited_count)++] = (bool)node;

    if (node->data == NULL)
        return false;

    if (node->info == OBJECT)
    {
        if (node->yes != NULL || node->no != NULL)
            return false;
    }
    else if (node->info == QUESTION)
    {
        if (node->yes == NULL || node->no == NULL)
            return false;
    }

    bool cycle_in_yes = false;
    bool cycle_in_no = false;

    if (node->yes != NULL)
        cycle_in_yes = tree_verify_recursive(node->yes, visited_nodes, visited_count, capacity);

    if (node->no != NULL)
        cycle_in_no = tree_verify_recursive(node->no, visited_nodes, visited_count, capacity);

    return cycle_in_yes || cycle_in_no;
}
