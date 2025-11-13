#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "akinator.h"

void command_guess(void)
{
    printf("\n=== Режим отгадывания ===\n");
    tree_guess(head);
}

void command_definition(void)
{
    printf("\n=== Режим определения ===\n");
    printf("Введите имя объекта для определения: ");

    char* object_name = NULL;
    size_t str_len = 50;
    getline_simple(&object_name, &str_len, stdin);

    get_definition(object_name);
    free(object_name);
}

void command_compare(void)
{
    printf("\n=== Режим сравнения ===\n");
    printf("Введите имя первого объекта: ");

    char* object1 = NULL;
    size_t str_len = 50;
    getline_simple(&object1, &str_len, stdin);

    printf("Введите имя второго объекта: ");

    char* object2 = NULL;
    getline_simple(&object2, &str_len, stdin);

    compare_objects(object1, object2);
    free(object1);
    free(object2);
}

void command_show_tree(void)
{
    printf("\n=== Визуализация дерева ===\n");
    tree_graph(head);
    printf("Дерево знаний сохранено в tree.png\n");
}

void command_exit(void)
{
    printf("\nСохранение базы знаний...\n");
    tree_save(head, "tree_text.txt");
    tree_dtor(head);
    printf("До свидания!\n");
}

node_t* find_object(node_t* node, const char* name)
{
    if (node == NULL) return NULL;

    if (node->info == OBJECT && strcmp(node->data, name) == 0)
        return node;

    node_t* found = find_object(node->yes, name);
    if (found != NULL) return found;

    return find_object(node->no, name);
}

bool find_path(node_t* node, const char* target, path_t* path)
{
    path->size = 0;
    path->capacity = 10;
    path->path = (char**)calloc(path->capacity, sizeof(char*));

    return find_path_recursive(node, target, path);
}

bool find_path_recursive(node_t* node, const char* target, path_t* path)
{
    if (node == NULL)
        return false;

    if (node->info == OBJECT && strcmp(node->data, target) == 0)
        return true;

    if (node->no != NULL)
    {
        if (path->size + 1 >= path->capacity)
        {
            path->capacity *= 2;
            path->path = (char**)realloc(path->path, path->capacity * sizeof(char*));
        }

        char* negative_question = (char*)calloc(strlen(node->data) + 10, sizeof(char));
        sprintf(negative_question, "не %s", node->data);
        path->path[path->size] = negative_question;
        path->size++;

        if (find_path_recursive(node->no, target, path))
            return true;

        path->size--;
        free(negative_question);
    }

    if (node->yes != NULL)
    {
        if (path->size + 1 >= path->capacity)
        {
            path->capacity *= 2;
            path->path = (char**)realloc(path->path, path->capacity * sizeof(char*));
        }

        path->path[path->size] = strdup(node->data);
        path->size++;

        if (find_path_recursive(node->yes, target, path))
            return true;

        path->size--;
        free(path->path[path->size]);
    }

    return false;
}


void tree_guess(node_t* node)
{
    if (node == NULL)
        return;

    if (node->info == OBJECT)
    {
        printf("Вы загадали %s?\n", node->data);

        if(get_users_answer())
            printf("Я знал!\n");

        else
        {
            printf("А кого вы загадали?\n");

            char* users_object = NULL;
            char* users_question = NULL;
            size_t str_len = 50;

            getline_simple(&users_object, &str_len, stdin);

            printf("А чем %s отличается от %s? Он... ", users_object, node->data);
            getline_simple(&users_question, &str_len, stdin);

            append_near(node, users_object, users_question);
            printf("Спасибо, я запомнил\n");
        }
    }

    else
    {
        printf("%s?\n", node->data);

        if(get_users_answer())
            tree_guess(node->yes);

        else
            tree_guess(node->no);
    }
}

void get_definition(const char* object_name)
{
    path_t path = {};
    if (find_path(head, object_name, &path))
    {
        printf("Определение для %s: ", object_name);
        print_definition(&path);

        for (int i = 0; i < path.size; i++)
            free(path.path[i]);
        free(path.path);
    }
    else
    {
        printf("Объект '%s' не найден в базе знаний.\n", object_name);
    }
}

void print_definition (path_t* path)
{
    if (path->size == 0)
    {
        printf("неизвестно\n");
        return;
    }

    printf("это ");
    for (int i = 0; i < path->size; i++)
    {
        printf("%s", path->path[i]);
        if (i < path->size - 1) printf(", ");
    }
    printf(".\n");
}

void compare_objects(const char* object1, const char* object2)
{
    path_t path_1 = {};
    path_t path_2 = {};

    bool found1 = find_path(head, object1, &path_1);
    bool found2 = find_path(head, object2, &path_2);

    if (found1 && found2)
        print_comparison(&path_1, &path_2);

    else
    {
        if (!found1) printf("Объект '%s' не найден.\n", object1);
        if (!found2) printf("Объект '%s' не найден.\n", object2);
    }

    for (int i = 0; i < path_1.size; i++)
        free(path_1.path[i]);

    for (int i = 0; i < path_2.size; i++)
        free(path_2.path[i]);

    free(path_1.path);
    free(path_2.path);
}

void print_comparison (path_t* path_1, path_t* path_2)
{
    int common_length = 0;
    while (common_length < path_1->size && common_length < path_2->size)
    {
        if(strcmp(path_1->path[common_length], path_2->path[common_length]) != 0)
            break;

        common_length++;
    }

    printf("Сравнение:\n");

    if (common_length > 0)
    {
        printf("Общие характеристики: ");
        for (int i = 0; i < common_length; i++)
        {
            printf("%s", path_1->path[i]);
            if (i < common_length - 1) printf(", ");
        }
        printf(".\n");
    }

    if (common_length < path_1->size)
    {
        printf("Но %s: ", path_1->path[common_length]);
        for (int i = common_length; i < path_1->size; i++)
        {
            printf("%s", path_1->path[i]);
            if (i < path_1->size - 1) printf(", ");
        }
        printf(".\n");
    }

    if (common_length < path_2->size)
    {
        printf("А %s: ", path_2->path[common_length]);
        for (int i = common_length; i < path_2->size; i++)
        {
            printf("%s", path_2->path[i]);
            if (i < path_2->size - 1) printf(", ");
        }
        printf(".\n");
    }
}
