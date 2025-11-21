#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "akinator.h"
#include "dump_akinator.h"
#include "graphics.h"

#define STACK_TYPE node_t*
#include "stack.h"

#include "commands.h"

void command_guess(void)
{
    text_out("\nРежим отгадывания\n");
    tree_dump_add_info("Before guess()", head);
    tree_guess(head);
    tree_dump_add_info("After guess()", head);
}

void command_definition(void)
{
    text_out("\nРежим определения\n");
    char question[STR_MAX_LEN] = "Введите имя объекта для определения: \n";
    text_out(question);

    char* object_name = strdup (myInputBox(question));

    tree_dump_add_info("Before definition()", head);
    get_definition(object_name);
    tree_dump_add_info("After definition()", head);

    free(object_name);
}

void command_compare(void)
{
    text_out("\nРежим сравнения\n");

    text_out("Введите имя первого объекта: \n");
    char* object1 = strdup (myInputBox());

    text_out("Введите имя второго объекта: \n");
    char* object2 = strdup (myInputBox());

    tree_dump_add_info("Before compare()", head);
    compare_objects(object1, object2);
    tree_dump_add_info("After compare()", head);

    free(object1);
    free(object2);
}

void command_show_tree(void)
{
    text_out("\nВизуализация дерева\n");
    tree_dump_add_info("Manual tree view", head);
    tree_graph(head);
    system("tree.png");
}

void command_exit(void)
{
    text_out("\nСохранение базы данных...\n");

    tree_dump_add_info("Before exit", head);
    tree_save(head, "tree_text.txt");
    tree_dtor(head);

    txPlaySound("sounds\\bye.wav");
    text_out("До свидания!\n");
    txSleep(500);
}

node_t* find_object(node_t* node, const char* name)
{
    assert(node);

    if (node->info == OBJECT && strcmp(node->data, name) == 0)
        return node;

    node_t* found = find_object(node->yes, name);
    if (found)
        return found;

    return find_object(node->no, name);
}

bool find_path_stack(node_t* node, const char* target, stack_t* stk)
{
    if (node == NULL)
        return false;

    if (node->info == OBJECT && strcmp(node->data, target) == 0)
    {
        stack_push(stk, node);
        return true;
    }

    if (node->no != NULL)
    {
        if (find_path_stack(node->no, target, stk))
        {
            stack_push(stk, node);
            return true;
        }
    }

    if (node->yes != NULL)
    {
        if (find_path_stack(node->yes, target, stk))
        {
            stack_push(stk, node);
            return true;
        }
    }

    return false;
}


void tree_guess(node_t* node)
{
    char temp_output[STR_MAX_LEN] = "";

    if (node == NULL)
        return;

    if (node->info == OBJECT)
    {
        sprintf(temp_output, "Вы загадали %s?\n", node->data);
        text_out(temp_output);

        if(get_users_answer(temp_output))
            make_success();

        else
        {
            make_fail();
            text_out("А кого вы загадали?\n");

            char* users_object = NULL;
            char* users_question = NULL;

            users_object = strdup (myInputBox());

            sprintf(temp_output, "А чем %s отличается от %s? Он... \n", users_object, node->data);
            text_out(temp_output);

            users_question = strdup (myInputBox());

            append_near(node, users_object, users_question);
            text_out("Спасибо, я запомнил\n");
        }
    }

    else
    {
        sprintf(temp_output, "%s?\n", node->data);
        text_out(temp_output);

        if(get_users_answer(temp_output, false))
            tree_guess(node->yes);

        else
            tree_guess(node->no);
    }
}

void get_definition(const char* object_name)
{
    stack_t path = {};
    stack_ctor(&path, 10);

    char output[STR_MAX_LEN] = "";

    if (find_path_stack(head, object_name, &path))
        print_definition(&path, object_name);

    else
    {
        sprintf(output, "Объект '%s' не найден в базе знаний.\n", object_name);
        text_out(output);
    }

    stack_dtor(&path);

}

void print_definition(stack_t* path, const char* object_name)
{
    if (path->size == 0)
    {
        text_out("неизвестно\n");
        return;
    }

    size_t buffer_size = STR_MAX_LEN;
    char* definition_text = (char*)calloc(buffer_size, sizeof(char));
    if (!definition_text)
        return;

    char temp_str[STR_MAX_LEN] = "";

    snprintf(definition_text, buffer_size, "Определение для %s:\n это ", object_name);

    for (int i = path->size - 2; i >= 0; i--)
    {
        node_t* current_node = path->data[i];
        node_t* next_node = path->data[i + 1];

        if (current_node->yes == next_node)
            snprintf(temp_str, sizeof(temp_str), "%s", current_node->data);

        else if (current_node->no == next_node)
            snprintf(temp_str, sizeof(temp_str), "не %s", current_node->data);

        else
            snprintf(temp_str, sizeof(temp_str), "%s", current_node->data);

        strcat(definition_text, temp_str);

        if (i > 0)
            strcat(definition_text, ", ");
    }

    strcat(definition_text, ".\n\nНажмите ENTER чтобы продолжить\n");
    text_out(definition_text);
    myInputBox(false);

    free(definition_text);
}

void compare_objects(const char* object1, const char* object2)
{
    stack_t path_1 = {};
    stack_ctor(&path_1, 10);

    stack_t path_2 = {};
    stack_ctor(&path_2, 10);

    char error_text[STR_MAX_LEN] = "";

    bool found1 = find_path_stack(head, object1, &path_1);
    bool found2 = find_path_stack(head, object2, &path_2);

    if (found1 && found2)
        print_comparison(&path_1, &path_2, object1, object2);

    else
    {
        if (!found1)
        {
            sprintf(error_text, "Объект '%s' не найден.\n", object1);
            text_out(error_text);
        }

        if (!found2)
        {
            sprintf(error_text, "Объект '%s' не найден.\n", object2);
            text_out(error_text);
        }
    }

    stack_dtor(&path_1);
    stack_dtor(&path_2);
}

void print_comparison(stack_t* path_1, stack_t* path_2, const char* object1, const char* object2)
{
    int size1 = path_1->size;
    int size2 = path_2->size;

    int common_length = 0;

    for (int i = 0; i < size1 && i < size2; i++)
    {
        node_t* node1 = path_1->data[size1 - 1 - i];
        node_t* node2 = path_2->data[size2 - 1 - i];

        if (node1 != node2)
            break;

        common_length++;
    }

    size_t buffer_size = STR_MAX_LEN;
    char* compare_text = (char*)calloc(buffer_size, sizeof(char));
    if (!compare_text)
        return;

    char temp_str[STR_MAX_LEN] = "";

    snprintf(compare_text, buffer_size, "Сравнение %s и %s:\n", object1, object2);

    if (common_length > 1)
    {
        strcat(compare_text, "Они похожи тем, что ");
        for (int i = 1; i < common_length; i++)
        {
            node_t* current_node = path_1->data[size1 - 1 - i];
            node_t* next_node = path_1->data[size1 - i];

            if (current_node->yes == next_node)
            {
                snprintf(temp_str, sizeof(temp_str), "%s", current_node->data);
            }
            else
            {
                snprintf(temp_str, sizeof(temp_str), "не %s", current_node->data);
            }

            strcat(compare_text, temp_str);

            if (i < common_length - 1)
                strcat(compare_text, ", ");
        }
        strcat(compare_text, ".\n");
    }

    if (common_length < size1)
    {
        snprintf(temp_str, sizeof(temp_str), "Но %s ", object1);
        strcat(compare_text, temp_str);

        for (int i = common_length; i < size1 - 1; i++)
        {
            node_t* current_node = path_1->data[size1 - 1 - i];
            node_t* next_node = path_1->data[size1 - i];

            if (current_node->yes == next_node)
                snprintf(temp_str, sizeof(temp_str), "%s", current_node->data);

            else
                snprintf(temp_str, sizeof(temp_str), "не %s", current_node->data);

            strcat(compare_text, temp_str);

            if (i < size1 - 2)
                strcat(compare_text, ", ");
        }
        strcat(compare_text, ".\n");
    }

    if (common_length < size2)
    {
        snprintf(temp_str, sizeof(temp_str), "А %s ", object2);
        strcat(compare_text, temp_str);

        for (int i = common_length; i < size2 - 1; i++)
        {
            node_t* current_node = path_2->data[size2 - 1 - i];
            node_t* next_node = path_2->data[size2 - i];

            if (current_node->yes == next_node)
            {
                snprintf(temp_str, sizeof(temp_str), "%s", current_node->data);
            }
            else
            {
                snprintf(temp_str, sizeof(temp_str), "не %s", current_node->data);
            }

            strcat(compare_text, temp_str);

            if (i < size2 - 2)
                strcat(compare_text, ", ");
        }
        strcat(compare_text, ".\n");
    }

    strcat(compare_text, "\nНажмите ENTER чтобы продолжить\n");
    text_out(compare_text);
    myInputBox(false);

    free(compare_text);
}
