#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>

#include "akinator.h"

node_t* node_ctor(void)
{
    node_t* new_node = (node_t*)calloc(1, sizeof(node_t));

    new_node->data   = NULL;
    new_node->no     = NULL;
    new_node->yes    = NULL;
    new_node->parent = NULL;
    new_node->info   = OBJECT;

    return new_node;
}

void place_after(node_t* node_before, node_t* new_node, enum ans_info answer)
{
    if (node_before == NULL)
    {
        head = new_node;
        return;
    }

    if (answer == YES)
        node_before->yes = new_node;

    else
        node_before->no = new_node;

    node_before->info = QUESTION;
}

void append_near(node_t* current, char* users_object, char* users_question)
{
    if (current == NULL) return;

    node_t* old_object = node_ctor();
    old_object->data   = current->data;
    old_object->info   = OBJECT;
    old_object->parent = current;

    node_t* new_object = node_ctor();
    new_object->data   = users_object;
    new_object->info   = OBJECT;
    new_object->parent = current;

    current->data = users_question;
    current->info = QUESTION;

    current->no = old_object;
    current->yes = new_object;
}

void tree_dtor(node_t* node)
{
    if (node == NULL)
        return;

    tree_dtor(node->yes);
    tree_dtor(node->no);

    free(node->data);
    free(node);
}

void tree_save (node_t* node, const char* filename)
{
    if(node == NULL)
        return;

    FILE* file = fopen(filename, "w");
    if (!file)
    {
        printf("file opening error\n");
        return;
    }

    write_node(node, file);

    fclose(file);
}

void write_node(node_t* node, FILE* file)
{
    fprintf(file, "(\"%s\"", node->data);

    if (!node->no)
        fprintf(file, " nil nil");
    else
    {
        write_node(node->no, file);
        write_node(node->yes, file);
    }

    fprintf(file, ")");
}

void tree_dot(FILE* dot_file, node_t* node)
{
    if (node == NULL)
        return;

    fprintf(dot_file, "    node%p [label=\"{{<f0>PTR: %p}|{%s}|{{<fl>НЕТ | %p}|{<fr>ДА | %p}}}\", shape=Mrecord];\n",
                        node, node, cp1251_to_utf8(node->data), node->no, node->yes);

    if (node->no)
    {
        fprintf(dot_file, "    node%p:<fl> -> node%p;\n", node, node->no);
        tree_dot(dot_file, node->no);
    }

    if (node->yes)
    {
        fprintf(dot_file, "    node%p:<fr> -> node%p;\n", node, node->yes);
        tree_dot(dot_file, node->yes);
    }
}

void tree_graph(node_t* node)
{
    FILE* dot_file = fopen("tree.dot", "w");
    if (!dot_file) {
        fprintf(stderr, "Cannot open file\n");
        return;
    }

    fprintf(dot_file, "digraph G {\n");
    fprintf(dot_file, "    charset=\"UTF-8\";\n");

    tree_dot(dot_file, node);

    fprintf(dot_file, "}\n");
    fclose(dot_file);

    system("dot -Tpng tree.dot -o tree.png");
}
