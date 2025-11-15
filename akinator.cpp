#include <TXLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#include "akinator.h"
#include "dump_akinator.h"

node_t* head = NULL;

const command_table_entry command_table[] =
{
    {GUESS,      "О", "тгадать",           command_guess},
    {DEFINITION, "Д", "ать определение",   command_definition},
    {COMPARE,    "С", "равнить",           command_compare},
    {SHOW_TREE,  "П", "оказать дерево",    command_show_tree},
    {EXIT,       "В", "ыход",              command_exit}
};

const int command_count = sizeof(command_table) / sizeof(command_table[0]);

int main()
{
    tree_dump_init();

    head = read_tree("tree_text.txt");

    TREE_VERIFY

    printf("Вас приветствует Акинатор!\nБаза знаний загружена успешно\n\n");

    while (true)
    {
        printf("Выберите режим:\n");

        for (int i = 0; i < command_count; i++)
        {
            printf("[%s]%s", command_table[i].name, command_table[i].description);
            if (i < command_count - 1) printf(", ");
        }
        printf(": ");

        enum user_commands user_cmd = get_user_cmd();

        command_table[user_cmd].function();

        printf("\n");

        if(user_cmd == EXIT)
            break;
    }

    tree_dump_close();

    return 0;
}

enum user_commands get_user_cmd(void)
{
    char cmd_smbl = (char)getchar();
    while (getchar() != '\n');

    for (int i = 0; i < command_count; i++)
    {
        if (command_table[i].name[0] == cmd_smbl)
        {
            return command_table[i].cmd;
        }
    }

    printf("Неизвестная команда. Попробуйте снова.\n");
    return get_user_cmd();
}

bool get_users_answer(void)
{
    const int LEN_STR = 10;
    const char* answer_yes[] = {"YES", "yes", "Yes", "y", "Y", "да", "Да", "ДА", "д", "Д"};
    const char* answer_no[] = {"NO", "no", "No", "n", "N", "н", "нет", "НЕТ", "Н", "Нет"};
    const int yes_count = 10;
    const int no_count = 10;

    while(true)
    {
        char answer[LEN_STR];
        scanf("%9s", answer);
        getchar();

        for (int i = 0; i < yes_count; i++)
        {
            if (strcmp(answer_yes[i], answer) == 0)
                return true;
        }

        for (int i = 0; i < no_count; i++)
        {
            if (strcmp(answer_no[i], answer) == 0)
                return false;
        }

        printf("Введите YES/NO или ДА/НЕТ: ");
    }
}

node_t* read_tree (const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file)
    {
        printf("file opening error\n");
        return NULL;
    }

    long int file_size = chek_file_size(file);

    char* buffer = (char*)calloc(file_size + 1, sizeof(char));
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, sizeof(char), file_size, file);

    char* curr_pos = buffer;
    node_t* new_head = read_node(&curr_pos);

    free(buffer);

    return new_head;
}

long int chek_file_size(FILE* file)
{
    struct stat file_info = {};
    int fd = fileno(file);
    if (fstat(fd, &file_info)== -1)
    {
        printf("ERROR: check number of lines incorrect\n");
        return -1;
    }

    return file_info.st_size;
}

node_t* read_node(char** curr_pos)
{
    if (**curr_pos  == '(')
    {
        node_t* new_node = node_ctor();
        (*curr_pos)++;

        if (**curr_pos == '"')
            new_node->data = read_name(curr_pos);

        else
        {
            printf("Reading error\n");
            return NULL;
        }

        (*curr_pos) = strchr((*curr_pos), '"');
        (*curr_pos)++;

        new_node->no = read_node(curr_pos);
        new_node->yes = read_node(curr_pos);

        if (new_node->no != NULL || new_node->yes != NULL)
            new_node->info = QUESTION;
        else
            new_node->info = OBJECT;

        (*curr_pos)++;

        return new_node;
    }

    else if (**curr_pos == ' ' || !strncmp(++(*curr_pos), "nil", 3))
    {
        (*curr_pos) += 4;
        return NULL;
    }

    else
    {
        printf("empty file or reading error\n");
        return NULL;
    }
}

char* read_name(char** curr_pos)
{
    (*curr_pos)++;

    char* start = *curr_pos;

    *curr_pos = strchr(*curr_pos, '"');
    if (*curr_pos == NULL)
    {
        printf("Error: unmatched quote\n");
        return NULL;
    }

    size_t len = *curr_pos - start;

    char* name = (char*)calloc(len + 1, sizeof(char));
    strncpy(name, start, len);
    name[len] = '\0';

    return name;
}

char* cp1251_to_utf8(const char* cp1251_str)
{
    assert (cp1251_str);

    size_t input_len = strlen(cp1251_str);
    char* utf8_str = (char*)calloc(input_len * 2 + 1, sizeof(char));
    if (!utf8_str)
        return NULL;

    size_t output_pos = 0;

    for (size_t i = 0; i < input_len; i++)
    {
        unsigned char c = (unsigned char)cp1251_str[i];

        if (c < 0x80)
            utf8_str[output_pos++] = c;

        else if (c >= 0xC0 && c <= 0xDF)
        {
            utf8_str[output_pos++] = (char)0xD0;
            utf8_str[output_pos++] = (char)(0x90 + (c - 0xC0));
        }

        else if (c >= 0xE0 && c <= 0xEF)
        {
            utf8_str[output_pos++] = (char)0xD0;
            utf8_str[output_pos++] = (char)(0xB0 + (c - 0xE0));
        }

        else if (c >= 0xF0 && c < 0xFF)
        {
            utf8_str[output_pos++] = (char)0xD1;
            utf8_str[output_pos++] = (char)(0x80 + (c - 0xF0));
        }

        else
            utf8_str[output_pos++] = c;
    }

    utf8_str[output_pos] = '\0';

    return utf8_str;
}
